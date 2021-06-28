# `nttiny v0.1`

OpenGL-based interactive plotting library.

```shell
git clone --recursive git@github.com:haykh/nttiny.git
```

## Dependencies

The dependencies need to be set up just once for each system (or each time you update the submodules). Their source codes are stored in `extern/` directory, while the libraries are stored in either `include/` or `lib/` directories. To update the dependency source codes run the following:

```shell
git submodule update --remote
```

### `glfw`

In case you already have the `glfw` library installed on your system -- change the `LIBRARIES` variable in `Makefile` from `glfw3` to `glfw` and skip this. Otherwise here's an instruction on how to compile it and put as a static library.

```shell
# for convenience define the path to source code as a variable
export NTTINY_PATH=...

cd $NTTINY_PATH/extern/glfw
# configure cmake
cmake -B build
cd build
# compile
make -j <NCORES>

# move the static library to `lib/`
mv $NTTINY_PATH/extern/glfw/build/src/libglfw3.a $NTTINY_PATH/lib/

# (optional) also move updated header files
# cp -r $NTTINY_PATH/extern/glfw/include/GLFW $NTTINY_PATH/include/

unset NTTINY_PATH
```

### `fmt`

```shell
# for convenience define the path to source code as a variable
export NTTINY_PATH=...

cd $NTTINY_PATH/extern/fmt
# configure cmake
cmake -B build -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE
cd build
# compile
make -j <NCORES>

# move the static library to `lib/`
mv $NTTINY_PATH/extern/fmt/build/libfmt.a $NTTINY_PATH/lib/

# (optional) also move updated header files
# cp -r $NTTINY_PATH/extern/fmt/include/fmt $NTTINY_PATH/include/

unset NTTINY_PATH
```

### `imgui`, `implot`

This library is compiled with the rest of the project, so need to just copy the proper files to `lib/imgui/`.

```shell
# for convenience define the path to source code as a variable
export NTTINY_PATH=...

mkdir -p $nttiny_path/lib/imgui/
cp $nttiny_path/extern/imgui/*.cpp $nttiny_path/lib/imgui/
cp $nttiny_path/extern/imgui/*.h $nttiny_path/lib/imgui/
mkdir -p $NTTINY_PATH/lib/imgui/backends/
cp $NTTINY_PATH/extern/imgui/backends/*_glfw.* $NTTINY_PATH/lib/imgui/backends/
cp $NTTINY_PATH/extern/imgui/backends/*_opengl3.* $NTTINY_PATH/lib/imgui/backends/

mkdir -p $nttiny_path/lib/implot/
cp $nttiny_path/extern/implot/*.cpp $nttiny_path/lib/implot/
cp $nttiny_path/extern/implot/*.h $nttiny_path/lib/implot/

unset NTTINY_PATH
```

### `glad`

1. Obtain `glad` headers and `glad.c` from [this online server](https://glad.dav1d.de/), for your specific version of OpenGL (use "Profile: Core" and mark the "Generate a loader" tick). 

> To find out your OpenGL version run `glxinfo | grep "OpenGL version"`.

2. Download the generated `glad.zip` archive, and unzip it (`unzip glad.zip`). If you do this from the source code directory (`<NTTINY_PATH>`) the headers will be properly placed into `include/glad` and `include/KHR` directories (otherwise, do that manually). 

3. Move the `glad.c` file to a more appropriate place (and change `.c` to `.cpp`): `mv glad.c lib/glad.cpp`.

### `plog`, `rapidcsv`

These are all header-only libraries. So it's only necessary to copy the proper header files.

```shell
# for convenience define the path to source code as a variable
export NTTINY_PATH=...

cp -r $NTTINY_PATH/extern/plog/include/plog $NTTINY_PATH/include
cp -r $NTTINY_PATH/extern/rapidcsv/src $NTTINY_PATH/include/rapidcsv

unset NTTINY_PATH
```

## Compilation

Compile the code with `make all [OPTIONS]` flag. To see the help menu with all the options type `make`.
