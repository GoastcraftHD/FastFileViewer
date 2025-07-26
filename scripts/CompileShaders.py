# Partially from https://github.com/SaschaWillems/Vulkan/blob/master/shaders/slang/compileshaders.py
import fileinput
import os
import subprocess
import sys


def getShaderStages(filename):
    stages = []
    with open(filename) as f:
        filecontent = f.read()
        if '[shader("vertex")]' in filecontent:
            stages.append("vertex")
        if '[shader("fragment")]' in filecontent:
            stages.append("fragment")
        if '[shader("raygeneration")]' in filecontent:
            stages.append("raygeneration")
        if '[shader("miss")]' in filecontent:
            stages.append("miss")
        if '[shader("closesthit")]' in filecontent:
            stages.append("closesthit")
        if '[shader("callable")]' in filecontent:
            stages.append("callable")
        if '[shader("intersection")]' in filecontent:
            stages.append("intersection")
        if '[shader("anyhit")]' in filecontent:
            stages.append("anyhit")
        if '[shader("compute")]' in filecontent:
            stages.append("compute")
        if '[shader("amplification")]' in filecontent:
            stages.append("amplification")
        if '[shader("mesh")]' in filecontent:
            stages.append("mesh")
        if '[shader("geometry")]' in filecontent:
            stages.append("geometry")
        if '[shader("hull")]' in filecontent:
            stages.append("hull")
        if '[shader("domain")]' in filecontent:
            stages.append("domain")
        f.close()
    return stages


for root, dirs, files in os.walk("assets/shaders"):
    for file in files:
        if not file.endswith(".slang"):
            continue

        input_file = os.path.join(root, file)
        # Slang can store multiple shader stages in a single file, we need to split into separate SPIR-V files for the sample framework
        stages = getShaderStages(input_file)
        print("Compiling %s" % input_file)
        output_base_file_name = "bin/" + input_file
        output_dir = "bin/" + root
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)

        for stage in stages:
            entry_point = stage + "Main"
            output_ext = ""
            match stage:
                case "vertex":
                    output_ext = ".vert"
                case "fragment":
                    output_ext = ".frag"
                case "raygeneration":
                    output_ext = ".rgen"
                case "miss":
                    output_ext = ".rmiss"
                case "closesthit":
                    output_ext = ".rchit"
                case "callable":
                    output_ext = ".rcall"
                case "intersection":
                    output_ext = ".rint"
                case "anyhit":
                    output_ext = ".rahit"
                case "compute":
                    output_ext = ".comp"
                case "mesh":
                    output_ext = ".mesh"
                case "amplification":
                    output_ext = ".task"
                case "geometry":
                    output_ext = ".geom"
                case "hull":
                    output_ext = ".tesc"
                case "domain":
                    output_ext = ".tese"
            output_file = output_base_file_name + output_ext + ".spv"
            output_file = output_file.replace(".slang", "")

            if os.environ.get("VULKAN_SDK") == None:
                print("Could not find VULKAN_SDK in PATH")
                sys.exit(1)

            compiler_path = os.environ.get("VULKAN_SDK") + "/bin/slangc"
            if os.name == "nt":
                compiler_path += ".exe"

            res = subprocess.call(
                "%s %s -profile spirv_1_6 -target spirv -o %s -entry %s -stage %s"
                % (compiler_path, input_file, output_file, entry_point, stage),
                shell=True,
            )
            if res != 0:
                sys.exit(res)
