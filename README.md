# OpenGL Programming Guide 9th

WIP ( Work in Progress )  

과거에 대한 기록 차원에서 변화된 개발환경에 맞게 수정 중 입니다.  


## 책 관련 링크  

<img src="https://image.aladin.co.kr/product/7678/46/cover500/0134495497_2.jpg" alt="" height="256px" align="right">

- [OpenGL Programming Guide 9th [ 원서 ]](http://www.opengl-redbook.com/)  

- [알라딘 서점](https://www.aladin.co.kr/shop/wproduct.aspx?ItemId=76784697) ( 출판일 2016-07-08 )  


## 개발 및 테스트 환경

- 시스템 ( Computer System )  

  - AMD Ryzen 9 5900X 12-Core Processor
  - NVIDIA Geforce RTX 3080 10GB
  - Windows 11 64bit Korean

- 컴파일러 및 링커 ( Compiler , Linker )  

  - [Visual Studio Community 2022](https://visualstudio.microsoft.com/ko/free-developer-offers/)  
    - C++를 사용한 데스크톱 개발  
    - C++를 사용한 게임 개발  

  - [~~MingGW-w64 via MSYS2~~](https://code.visualstudio.com/docs/cpp/config-mingw)
    - 당분간 테스트 없음  
    - [설치파일](https://github.com/msys2/msys2-installer/releases/download/2024-12-08/msys2-x86_64-20241208.exe)
      ```
      $ pacman -S --needed base-devel mingw-w64-ucrt-x86_64-toolchain
      ```
      ```
      // 시스템 환경 변수 'PATH' 에 추가 등록  
      C:\msys64\ucrt64\bin
      ```
      ```
      $ gcc --version
      $ g++ --version
      $ gdb --version
      ```

- 에디터 ( Editor, Intergrated Development Environment )  

  - [VS Code](https://visualstudio.microsoft.com/ko/free-developer-offers/)  
    - C/C++ Extension for VS Code
      - c_cpp_properties.json ( compiler path and IntelliSense settings )  
      - tasks.json ( build instructions )  
      - launch.json ( debugger settings )  
      - Chapter01 동작 테스트 후 CMake 도입함.  

- 빌드 매니저 ( Build Manager )
  - [CMake](https://cmake.org/)  
    - [설치](https://cmake.org/download/)  
    - ...
    ```
    $ cmake --version
    ```
  - CMakeLists.txt
  - CMakePresets.json
  - CMakeUserPresets.json

- 패키지 매니저 ( Package Manager )
  - [vcpkg](https://vcpkg.io/en/)  
    - [설치](https://github.com/microsoft/vcpkg)  
    - ...
    ```
    $ vcpkg --version
    $ vcpkg new --application
    ```
  - vcpkg.json
  - vcpkg-configuration.json


## 사용된 패키지 목록

- GLAD
  - [vcpkg](https://vcpkg.io/en/package/glad)
    ```
    $ vcpkg add port glad[gl-api-43]
    ```
  - [GLAD](https://glad.dav1d.de/)  
  - C/C++ OpenGL Core 4.3 API

- GLFW
  - [vcpkg](https://vcpkg.io/en/package/glfw3)
    ```
    $ vcpkg add port glfw3
    ```
  - [GLFW](https://www.glfw.org/download.html)
  - [Windows Pre-Compiled Binaries](https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip)
  - glfw 3.4 win64

- GLM
  - [vcpkg](https://vcpkg.io/en/package/glm)  
    ```
    $ vcpkg add port glm
    ```
  - [glm](https://github.com/g-truc/glm)
  - OpenGL Mathematics  

- STB
  - [vcpkg](https://vcpkg.io/en/package/stb)  
  ```
  $ vcpkg add port stb
  ```
  - [stb](https://github.com/nothings/stb)
  - Image Loader

- IMGUI
  - [vcpkg](https://vcpkg.io/en/package/imgui)  
  ```
  $ vcpkg add port imgui[glfw-binding,opengl3-binding,freetype]
  ```
  - [imgui](https://github.com/ocornut/imgui)
  - Immediate Mode Graphical User Interface

- ...
  - [vcpkg]()  
  ```
  $ vcpkg add port ...
  ```
  - [...]()
  - ...

## ...

---
---
---


OpenGL Red Book Example Code
============================

This is the example code for the OpenGL Programming Guide (Red Book), 9th Edition.
It is mostly an import of code from previous editions with minor updates
to match some of the descriptions in the latest edition of the book.
We will be adding more samples and updating existing ones over time.
Treat this as a live repository. Pull requests and issues are welcome.
Go forth and fork!

Building
--------

We are using CMake as our build system. To build the samples, enter
the "build" subdirectory and enter "cmake -G "{your generator here}" .."
Project files or makefiles will be produced in the build subdirectory.
We have tested Unix makefiles ("Unix Makefiles") and Visual Studio project files.
Visual Studio 2013 ("Visual Studio 12") was used to develop most of the samples.
An update to add support for 2015 will land shortly. Code should compile, but the
templates for the project files are not in the repository at this time.
Other compilers and IDEs should work, but we haven't tested them.

Running
-------

After building, there will be a large collection of executables in the bin directory.
These are the example applications, which you can directly execute.

The media is now included with the git repository, and
the samples should compile and run out-of-the box. Of course, make sure your
OpenGL drivers are up to date and that your hardware is capable of running OpenGL 4.5.
That's what this book is about. Some systems don't support OpenGL 4.5 and will therefore
not run some of the samples. Trying to run such samples on a machine that doesn't have
OpenGL 4.5 support will fail. For example, if a platform were limited to, say OpenGL 4.1,
then the samples wouldn't work on that platform. Please don't file bugs about that either.
Error checking in these applications is minimal. If you don't have media files or if
your OpenGL drivers are out of date, they'll probably fail spectacularly.