import os
import shutil

current_dir = os.getcwd()

sdk_folder_path = os.path.join(current_dir, "sdk")

if os.path.exists(sdk_folder_path):
    shutil.rmtree(sdk_folder_path)

os.makedirs(sdk_folder_path)
os.makedirs(os.path.join(sdk_folder_path, "lib"))

lib_origin = os.path.join(current_dir, "lib")
fmt_lib_origin = os.path.join(lib_origin, "fmt")
lib_path = os.path.join(sdk_folder_path, "lib")
leveldb_lib_origin = os.path.join(lib_origin, "leveldb")

source_file_path = "bin/MinSizeRel/KobeBryant.lib"
destination_folder_path = "sdk/lib/"

include_folder_path = os.path.join(current_dir, "include")


shutil.copytree(include_folder_path, os.path.join(sdk_folder_path, "include"))
shutil.copytree(fmt_lib_origin, os.path.join(lib_path, "fmt"))
shutil.copytree(leveldb_lib_origin, os.path.join(lib_path, "leveldb"))

destination_file_path = os.path.join(
    destination_folder_path, os.path.basename(source_file_path)
)

shutil.copy(source_file_path, destination_file_path)
