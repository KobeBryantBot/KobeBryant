import os
import shutil

current_dir = os.getcwd()

if os.path.exists("../output/sdk"):
    shutil.rmtree("../output/sdk")

os.makedirs("../output/sdk")

shutil.copytree("../include", "../output/sdk/include")
shutil.copytree("../lib", "../output/sdk/lib")

if os.path.exists("../build/MinSizeRel/KobeBryant.lib"):
    shutil.copy("../build/MinSizeRel/KobeBryant.lib", "../output/sdk/lib/KobeBryant.lib")
elif os.path.exists("../build/Release/KobeBryant.lib"):
    shutil.copy("../build/Release/KobeBryant.lib", "../output/sdk/lib/KobeBryant.lib")
