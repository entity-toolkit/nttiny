# nttiny

```shell
git clone --recursive https://github.com/haykh/nttiny.git
```

## Setup

The following setup ensures that all the libraries are compiled with the proper system-specific parameters. These steps need to be done just once on each system. In case you already have the `glfw` library install on your system -- change the `LIBRARIES` variable in `Makefile` from `glfw3` to `glfw` and skip steps 1-2.

1. First we will need to compile a static `glfw` library for your system: 

```shell
cd <NTTINY_PATH>/extern/glfw
# configure cmake
cmake -B build
cd build
# compile
make -j <NCORES>
```

2. Move the compiled static library so the compiler can find it:
```shell
# create a directory for static libraries
cd <NTTINY_PATH>
mkdir -p lib
mv extern/glfw/build/src/libglfw3.a lib
```

3. Obtain `glad` headers and `glad.c` from [this online server](https://glad.dav1d.de/), for your specific version of OpenGL (use "Profile: Core" and mark the "Generate a loader" tick). 

> To find out your OpenGL version run `glxinfo | grep "OpenGL version"`.

4. Download the generated `glad.zip` archive, and unzip it (`unzip glad.zip`). If you do this from the source code directory (`<NTTINY_PATH>`) the headers will be properly placed into `include/glad` and `include/KHR` directories (otherwise, do that manually). 

5. Move the `glad.c` file to a more appropriate place (and change `.c` to `.cpp`): `mv glad.c src/glEngine/glad.cpp`.

## Compilation

Compile the code with `make all [OPTIONS]` flag. To see the help menu with all the options type `make`.
