import os
import shutil

current_dir = os.getcwd()

sdk_folder_path = os.path.join(current_dir, "sdk")

if os.path.exists(sdk_folder_path):
    shutil.rmtree(sdk_folder_path)

os.makedirs(sdk_folder_path)

include_folder_path = os.path.join(current_dir, "include")
lib_folder_path = os.path.join(current_dir, "lib")


shutil.copytree(include_folder_path, os.path.join(sdk_folder_path, "include"))
shutil.copytree(lib_folder_path, os.path.join(sdk_folder_path, "lib"))
