# OpenGL ES C++ examples
A comprehensive collection of open source C++ examples for OpenGL ES 3.2 which is from Khronos.

## Development Environment

<img src="./images/windowslogo.png" alt="" height="22px" valign="bottom">[Windows 10](https://www.microsoft.com/en-us/software-download/windows10), <img src="./images/vs2019logo.png" alt="" height="26px" valign="bottom">[Visual Studio 2019](https://visualstudio.microsoft.com/zh-hant/vs/?rr=https%3A%2F%2Fwww.baidu.com%2Flink%3Furl%3DS64iLGalmjZ-FW3YG98CKR-p-qlL5e44wYM1QX0YOcUTbyVG9RW-OuEKWDJvhpo5eA_XrGeAjalso5T-f03iaK%26wd%3D%26eqid%3Db664e1c30015a0e9000000065d07a930)

## Build Tool

<img src="./images/cmakelogo.png" alt="" height="30px" valign="bottom">[CMake](https://cmake.org/) 3.2 or higher

## Building

The repository contains everything required to compile and build the examples on Windows,  using the Visual Studio compiler that supports C++17. All required dependencies are included.

- ###### compile option : Debug

- ###### target platform : Win32

<img src="./images/windowslogo.png" alt="" height="30px"> **Windows**

Use the provided CMakeLists.txt with CMake to generate a build configuration, e.g.:

```
cmake -G "Visual Studio 16 2019" -A Win32
```

