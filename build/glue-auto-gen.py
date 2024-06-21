import os
from clang.cindex import CursorKind, Index, TokenKind
import clang
import json
import argparse


def get_translation_units(input_filenames):
    index = Index.create()
    translation_units = []
    for input_file in input_filenames:
        translation_unit = index.parse(input_file, args=['-x', 'c++'])
        translation_units.append(translation_unit)

    return translation_units

def enumerate_all_headers():
    headers = []
    for root, dirs, files in os.walk("include"):
        for file in files:
            if file.endswith(".hpp"):
                headers.append(os.path.join(root, file))

    for root, dirs, files in os.walk(os.path.join("out", "win", "lib", "dawn", "gen", "include", "dawn")):
        for file in files:
            if file.endswith(".h"):
                headers.append(os.path.join(root, file))

    return headers


def enumerate_all_export_classes(headers: list[str]):
    units = get_translation_units(headers)
    export_classes = {}

    for unit in units:
        for cursor in unit.cursor.walk_preorder():

            def fully_qualified(c):
                if c is None:
                    return ''
                elif c.kind == CursorKind.TRANSLATION_UNIT:
                    return ''
                else:
                    res = fully_qualified(c.semantic_parent)
                    if res != '':
                        return res + '::' + c.spelling
                return c.spelling
            
            def deriving_from_export_object(cls):
                derived_from_export_object = False
                for child in cls.get_children():
                    if child.kind == CursorKind.CXX_BASE_SPECIFIER:
                        base = child.get_definition()
                        if (base.spelling == "ExportObject") or (base in export_classes):
                            cls_def = cls
                            # print("Found export class: ", cls_def.spelling,
                            #       cls_def.location.file.name, cls_def.location.line)
                            export_classes[fully_qualified(cls_def)] = cls_def
                            derived_from_export_object = True
                        else:
                            if deriving_from_export_object(base):
                                cls_def = cls
                                # print("Found export class: ", cls_def.spelling,
                                #       cls_def.location.file.name, cls_def.location.line)
                                export_classes[fully_qualified(cls_def)] = cls_def
                                derived_from_export_object = True

                return derived_from_export_object

            if cursor.kind == CursorKind.CLASS_DECL:
                cls = cursor
                # print("Found class: ", cursor.spelling)
                deriving_from_export_object(cls)

    print("Export classes: ", [
          v.spelling for v in export_classes.values()])
    return export_classes


if __name__ == '__main__':
    print("Running glue-auto-gen.py")

    # configure clang path
    parser = argparse.ArgumentParser()
    parser.add_argument("--clang-path", type=os.path.abspath,
                        help="Path to directory where libclang.dll exists", required=True)
    parser.add_argument("--output", type=os.path.abspath,
                        help="Output file", required=True)
    args = parser.parse_args()

    clang.cindex.Config.set_library_path(args.clang_path)

    print("Enumerating all headers")
    headers = enumerate_all_headers()
    print("Found headers: ", headers)

    print("Enumerating all definitions")
    export_classes = enumerate_all_export_classes(headers)

    print("Generating glue code")
    code = ""
    # includes
    code += f"#include <memory>\n"
    for headers in headers:
        code += f"#include \"{headers}\"\n"

    for k, v in export_classes.items():
        print("Generating glue code for: ", k)
        
        for child in v.get_children():
            if child.access_specifier == clang.cindex.AccessSpecifier.PUBLIC:
                # print(child.spelling, child.kind)
                
                if child.kind == CursorKind.CONSTRUCTOR:
                    constructor = f"""
EXPORT void* {k.replace('::', '_')}_Constructor() {{
    auto ptr = new {k}();
    ptr->add_reference();
    return ptr;
}}
                    """
                    # print(constructor)
                    code += constructor
                
                if child.kind == CursorKind.DESTRUCTOR:
                    destructor = f"""
EXPORT void {k.replace('::', '_')}_Destructor(void* self) {{
    auto obj = static_cast<{k}*>(self);
    obj->remove_reference();
}}
                    """
                    # print(destructor)
                    code += destructor

                if child.kind == CursorKind.CXX_METHOD:
                    # for token in child.get_tokens():
                    #     print(token.spelling, token.kind)
                    print(child.spelling, child.result_type.get_canonical().spelling, child.result_type.get_canonical().get_declaration().spelling)
                    argments = []
                    argments.append((k, "self"))
                    for arg in child.get_arguments():
                        # print(arg.type.spelling, arg.spelling)
                        argments.append((arg.type.get_canonical().spelling, arg.spelling))
                        # print(argments[-1][0], argments[-1][1])
                    
                    method = f"""
EXPORT {child.result_type.get_canonical().spelling} {k.replace('::', '_')}_{child.spelling}(void* self"""
                    for arg in argments[1:]:
                        method += f", {arg[0]} {arg[1]}"
                    method += """) {"""
                    method += f"""
    auto obj = static_cast<{k}*>(self);
    return obj->{child.spelling}("""
                    for arg in argments[1:]:
                        method += arg[1]
                        if arg != argments[-1]:
                            method += ", "
                    method += """);
}
                    """
                    # print(method)
                    code += method
        print("\n")

    # write to file
    with open(args.output, "w") as f:
        f.write(code)
