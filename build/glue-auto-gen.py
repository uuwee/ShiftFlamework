import os
from clang.cindex import CursorKind, Index, TokenKind
import clang
import json
import argparse

def get_class_tree(input_filenames):
    classes = {}
    typealiases = {}
    for input_file in input_filenames:
        index = Index.create()
        translation_unit = index.parse(input_file, args=['-x', 'c++'])

        namespace = []
        def search_namespace(cursor):
            if cursor.kind == CursorKind.NAMESPACE:
                if cursor.spelling != "std":
                    namespace.append(cursor.spelling)
                    for child in cursor.get_children():
                        search_namespace(child)
                    namespace.pop()

            if cursor.kind == CursorKind.CLASS_DECL:
                classes[cursor.spelling] = {
                    "filename": cursor.location.file.name,
                    "line_number": cursor.location.line,
                    "namespace": namespace.copy(),
                    "base_classes": [base.spelling for base in cursor.get_children() if base.kind == CursorKind.CXX_BASE_SPECIFIER],
                    "methods": {},
                    "constructors": [method.spelling for method in cursor.get_children() if method.kind == CursorKind.CONSTRUCTOR],
                    "destructor": [method.spelling for method in cursor.get_children() if method.kind == CursorKind.DESTRUCTOR],
                }
                for method in cursor.get_children():
                    if not method.kind == CursorKind.CXX_METHOD:
                        continue
                    classes[cursor.spelling]["methods"][method.spelling] = {
                        "return_type": method.result_type.spelling,
                        "parameters": {},
                        "access_specifier": method.access_specifier.name,
                    }
                    
                    for c in method.get_children():
                        if c.kind == CursorKind.PARM_DECL:
                            classes[cursor.spelling]["methods"][method.spelling]["parameters"][c.spelling] = c.type.spelling

            if cursor.kind == CursorKind.TYPE_ALIAS_DECL:
                typealiases[cursor.spelling] = {
                    "filename": cursor.location.file.name,
                    "line_number": cursor.location.line,
                    "namespace": namespace.copy(),
                    "type": cursor.underlying_typedef_type.spelling,
                }
        for cursor in translation_unit.cursor.get_children():
            search_namespace(cursor)
    return classes, typealiases

def main():
    index = Index.create()
    translation_unit = index.parse("./include/screenspace_mesh.hpp", args=["-x", "c++", "-std=c++20"])

    classes, type_aliases = get_class_tree(["./include/screenspace_mesh.hpp"])
    print(json.dumps(classes, indent=4))

    pass

if __name__ == '__main__':
    print("Running glue-auto-gen.py")

    parser = argparse.ArgumentParser()
    parser.add_argument("--clang-path", type=str , help="Path to clang", required=True)
    args = parser.parse_args()

    clang.cindex.Config.set_library_path(args.clang_path)
    main()