# OpenGL ES C++ examples
A comprehensive collection of open source C++ examples for OpenGL ES 3.1 which is from Khronos.

## Development Environment

<img src="./images/windowslogo.png" alt="" height="22px" valign="bottom">[Windows 10](https://www.microsoft.com/en-us/software-download/windows10), <img src="./images/vs2019logo.png" alt="" height="26px" valign="bottom">[Visual Studio 2019](https://visualstudio.microsoft.com/zh-hant/vs/?rr=https%3A%2F%2Fwww.baidu.com%2Flink%3Furl%3DS64iLGalmjZ-FW3YG98CKR-p-qlL5e44wYM1QX0YOcUTbyVG9RW-OuEKWDJvhpo5eA_XrGeAjalso5T-f03iaK%26wd%3D%26eqid%3Db664e1c30015a0e9000000065d07a930)

## Build Tool

<img src="./images/cmakelogo.png" alt="" height="32px" valign="bottom">[CMake](https://cmake.org/) 3.2 or higher

## Building

The repository contains everything required to compile and build the examples on Windows,  using the Visual Studio compiler that supports C++14 at least. All required dependencies are included.

- ##### compile option : Debug, MinSizeRel, Release, RelWithDebInfo

- ##### target platform : Win32, Win64

<img src="./images/windowslogo.png" alt="" height="30px"> **Windows**

Use the provided CMakeLists.txt with CMake to generate a build configuration, e.g.:

```
cmake -G "Visual Studio 16 2019"
```

## Examples

#### [01 - hello_window](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/01.hello_window>)



#### [02 - hello_window_color_clear](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/02.hello_window_color_clear>)



#### [03 - triangle](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/03.triangle>)



#### [04 - quadrangle_indices](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/04.quadrangle_indices>)



#### [05 - triangle_restart_index](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/05.triangle_restart_index>)



#### [06 - triangle_instancing](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/06.triangle_instancing>)



#### [07 - quad_with_texture](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/07.quad_with_texture>)



#### [08 - cube_with_camera](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/08.cube_with_camera>)



#### [09 - transformations](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/09.transformations>)



#### [10 - model_loading](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/10.model_loading>)



#### [11 - cull_face](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/11.cull_face>)



#### [12 - stencil_buffer](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/12.stencil_buffer>)



#### [13 - blinn_phong_lighting](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/13.blinn_phong_lighting>)



#### [14 - blending](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/14.blending>)



#### [15 - offscreen_rendering](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/15.offscreen_rendering>)



#### [16 - normal_mapping](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/16.normal_mapping>)



#### [17 - dear_imgui](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/17.dear_imgui>)



#### [18 - uniform_buffer_object](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/18.uniform_buffer_object>)



#### [19 - skybox_reflect](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/19.skybox_reflect>)



#### [20 - parallax_mapping](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/20.parallax_mapping>)



#### [21 - shadow_mapping_directional_light](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/21.shadow_mapping_directional_light>)



#### [22 - shadow_mapping_point_light](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/22.shadow_mapping_point_light>)



#### [23 - shadow_mapping_spot_light](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/23.shadow_mapping_spot_light>)



#### [24 - cascade_shadow_mapping](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/24.cascade_shadow_mapping>)



#### [25 - variance_shadow_mapping](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/25.variance_shadow_mapping>)



#### [26 - bloom_with_hdr](<https://github.com/rinkowei/OpenGLES_Examples/tree/master/src/26.bloom_with_hdr>)