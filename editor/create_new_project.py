import sys
import os
import shutil

path_to_new_proj = sys.argv[1]
proj_name = sys.argv[2]
new_proj_path = os.path.join(path_to_new_proj, proj_name)
editor_path = os.path.dirname(__file__)
editor_resource_path = os.path.join(editor_path, "new_project")

# cmake
os.makedirs(new_proj_path, exist_ok=True)
os.makedirs(os.path.join(new_proj_path,"include"), exist_ok=True)
os.makedirs(os.path.join(new_proj_path, "source"), exist_ok=True)
with open(os.path.join(new_proj_path, "CmakeLists.txt"), mode="x", encoding="utf-8") as cmake:
    cmake.writelines(["cmake_minimum_required(VERSION 3.26)\n",
                      "project(" + proj_name +")\n",
                      "set(CMAKE_CXX_STANDARD 20)\n",
                      "file(GLOB includes include/*.hpp)\n",
                      "file(GLOB sources source/*.cpp)\n",
                      "add_library(" + proj_name + " SHARED ${sources} ${includes})\n",
                      "target_compile_features(" + proj_name + " PRIVATE cxx_std_20)\n",
                      "target_include_directories(" + proj_name + " PUBLIC include include/ShiftFlamework)\n"])
    pass

# gitignore
shutil.copyfile(os.path.join(editor_resource_path, ".gitignore"), os.path.join(new_proj_path, ".gitignore"))

# headers
shutil.copytree(os.path.dirname(os.path.dirname(__file__)) + "/include", path_to_new_proj + "/" + proj_name + "/include/ShiftFlamework")