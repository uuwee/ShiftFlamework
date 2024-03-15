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

def get_base_classes(target, classes: dict):
    if target not in classes:
        return []
    
    base_classes = classes[target]["base_classes"]
    for base in base_classes:
        base_classes += get_base_classes(base, classes)

    return base_classes

def enumerate_all_headers():
    headers = []
    for root, dirs, files in os.walk("include"):
        for file in files:
            if file.endswith(".hpp"):
                headers.append(os.path.join(root, file))

    return headers

def enumerate_all_export_classes(headers: list[str]):
    units = get_translation_units(headers)
    for unit in units:
        


if __name__ == '__main__':
    print("Running glue-auto-gen.py")

    # configure clang path
    parser = argparse.ArgumentParser()
    parser.add_argument("--clang-path", type=str , help="Path to clang", required=True)
    parser.add_argument("--output", type=str , help="Output file", required=True)
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
        code += generate_glue_code((k, v))

    # write to file
    with open(args.output, "w") as f:
        f.write(code)