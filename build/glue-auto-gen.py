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
                    "properties": {},
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

                for property in cursor.get_children():
                    if not property.kind == CursorKind.FIELD_DECL:
                        continue
                    classes[cursor.spelling]["properties"][property.spelling] = {
                        "return_type": property.type.spelling,
                        "access_specifier": property.access_specifier.name,
                    }

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
    classes = {}
    structs = {}
    type_aliases = {}

    index = Index.create()
    classes, type_aliases = get_class_tree(headers)

    export_objects = {}
    for k, v in classes.items():
        if "ExportObject" in get_base_classes(k, classes):
            export_objects[k] = v

    return export_objects
    pass

def generate_glue_code(class_tree):
    name, tree = class_tree
    print("generate code for", name)
    # print(json.dumps(tree, indent=4))

    perfect_class_name = ""
    for namespace in tree["namespace"]:
        perfect_class_name += namespace + "_"
    perfect_class_name += name

    cpp_perfect_class_name = ""
    for namespace in tree["namespace"]:
        cpp_perfect_class_name += namespace + "::"
    cpp_perfect_class_name += name

    code = ""
    # constructor
    # EXPORT void* ShiftFlamework_ExampleClass_Construcotor() {
    #     auto ptr = new ShiftFlamework::ExampleClass();
    #     ptr->add_reference();
    #     return ptr;
    # }
    code += f"EXPORT void* {perfect_class_name}_Constructor() {{\n"
    code += f"    auto ptr = new {cpp_perfect_class_name}();\n"
    code += f"    ptr->add_reference();\n"
    code += f"    return ptr;\n"
    code += "}\n\n"

    # destructor
    # EXPORT void ShiftFlamework_ExampleClass_Destructor(void* self) {
    #     auto obj = (ShiftFlamework::ExampleClass*)self;
    #     obj->remove_reference();
    # }
    code += f"EXPORT void {perfect_class_name}_Destructor(void* self) {{\n"
    code += f"    auto obj = ({cpp_perfect_class_name}*)self;\n"
    code += f"    obj->remove_reference();\n"
    code += "}\n\n"

    # methods
    for method_name, method in tree["methods"].items():
        # public only
        if method["access_specifier"] != "PUBLIC":
            continue
        # EXPORT void ShiftFlamework_ExampleClass_Method(void* self, const int a, const int b) {
        #     auto obj = (ShiftFlamework::ExampleClass*)self;
        #     obj->method(a, b);
        # }
        code += f"EXPORT {method['return_type']} {perfect_class_name}_{method_name}(void* self"
        for param_name, param_type in method["parameters"].items():
            code += f", {param_type} {param_name}"
        code += ") {\n"
        code += f"    auto obj = ({cpp_perfect_class_name}*)self;\n"
        code += f"    obj->{method_name}("
        if len(method["parameters"]) > 0:
            code += f"{', '.join([param_name for param_name in method['parameters']])}"
        code += ");\n"
        code += "}\n\n"
        pass

    # properties
    for property_name, property in tree["properties"].items():
        # public only
        if property["access_specifier"] != "PUBLIC":
            continue
        # EXPORT void ShiftFlamework_ExampleClass_Property_Setter(void* self, const int value) {
        #     auto obj = (ShiftFlamework::ExampleClass*)self;
        #     obj->property = value;
        # }
        code += f"EXPORT void {perfect_class_name}_{property_name}_Setter(void* self, {property['return_type']} value) {{\n"
        code += f"    auto obj = ({cpp_perfect_class_name}*)self;\n"
        code += f"    obj->{property_name} = value;\n"
        code += "}\n\n"

        # EXPORT int ShiftFlamework_ExampleClass_Property_Getter(void* self) {
        #     auto obj = (ShiftFlamework::ExampleClass*)self;
        #     return obj->property;
        # }
        code += f"EXPORT {property['return_type']} {perfect_class_name}_{property_name}_Getter(void* self) {{\n"
        code += f"    auto obj = ({cpp_perfect_class_name}*)self;\n"
        code += f"    return obj->{property_name};\n"
        code += "}\n\n"

    # print(code)
    return code

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