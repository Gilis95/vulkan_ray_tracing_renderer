![](wunder_logo.png "logo")

## How to build project

Project is using CMake as a build tool. It's recommended to generate platform dependent files and store them in new project root child directory, e.g. `build`.
To build the project by mentioned above method, following secuence of commands should be used:

 ```
 cd ${WUNDER_PROJECT_ROOT_DIR}
 mkdir build
 cd build
 cmake ../
 ```
## Requirements

### Operating System

#### Linux
That's the system of choice and the one it's being developed under. Tests are run only under it.

#### Windows
It should be working out of the box under windows, but it's not tested.

#### MacOS
This system has no native Vulkan support, so I don't expect to be runnable there.

### Hardware
Only restriction is the GPU, since we're using rtx extension \
_GPU:_ [list of supported ones](https://vulkan.gpuinfo.org/listdevicescoverage.php?extension=VK_KHR_ray_tracing_pipeline&platform=windows) 
