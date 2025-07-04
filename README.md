# FastFileViewer

Just a 3D object viewer, intended as a project to learn Vulkan.

## Planned features
- Ray Tracing
- Simple File Browser
- PBR Material Viewer

## How to build
You need to have the Vulkan Sdk installed and your GPU needs to support Vulkan 1.3 or higher.
### Linux
```bash
git clone --recursive https://github.com/GoastcraftHD/FastFileViewer.git
cd FastFileViewer
./Generate-Linux.sh
make
```

The Generate-Linux.sh script will automatically create a make file and compile the shaders.

### Windows
```batch
git clone --recursive https://github.com/GoastcraftHD/FastFileViewer.git
```
Then just execute the Generate-Windows.bat script to create a Visual Studio 22 Solution and to compile the shaders.
