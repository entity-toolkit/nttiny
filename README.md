# `nttiny`

OpenGL-based interactive plotting library for scientific computing.

```shell
git clone --recursive git@github.com:haykh/nttiny.git
# to update submodules
git submodule update --remote
```

## Compilation

Compile `nttiny` as a static library with the example code as a standalone application:

```shell
cmake -B build -D CMAKE_INSTALL_PREFIX=<install_dir> -D BUILD_EXAMPLES=ON
cd build
make -j
# install to the specified directory
make install
```

## Usage

File `examples/examples.cpp` contains the most comprehensive examples on how to use `nttiny` for your own code.

To use `nttiny` as a plotting tool for your code you would need to build and use `nttiny` as a static library. To do that simply add `nttiny` as a subdirectory and link to your target; can be done by adding the following to your `CmakeLists.txt`: 

```cmake
add_subdirectory(${PATH_TO_NTTINY} nttiny)
target_link_libraries(${YOUR_TARGET_NAME} PRIVATE nttiny)
target_include_directories(${YOUR_TARGET_NAME} PRIVATE ${PATH_TO_NTTINY}/)
```

If using GNU Makefiles you can do the same by adding the following to your `Makefile`:

```makefile
NTTINY_DIR = ${PATH_TO_NTTINY}
NTTINY_LIB = ${BUILD_DIR}/libnttiny.a

${BUILD_DIR}/libnttiny.a:
  cmake -B ${BUILD_DIR} -D CMAKE_INSTALL_PREFIX=${BUILD_DIR} -S ${NTTINY_DIR} -D BUILD_EXAMPLES=OFF && cd ${BUILD_DIR} && make -j && make install

LIBS += ${NTTINY_LIB}
INCLUDES += -I${NTTINY_DIR}

# then use ${LIBS} and ${INCLUDES} when compiling/linking your target
```

## Releases
* `v0.7.0` [Oct 2022]:
  - full cmake support (readme updated)
  - option for symmetric colorbar
  - unnnecessary freetype dependency removed
  - minor bugs fixed
* `v0.6.4` [Sep 2022]:
  - notifications
  - movie recording (frames)
  - more advanced file saving (directory creation)
  - ignoring warnings for some external depends
* `v0.6.3` [Sep 2022]:
  - timeplot added
  - collapsible controls
  - better organizing state saving/loading
  - minor bug fixes
  - minor UI improvements
* `v0.6.2` [Sep 2022]:
  - better menu ui
  - better legends (cmap selector) + plot control
  - global ui settings
  - optimized data loading/updating
  - minor bug fixes with rescaling
* `v0.6.1` [Sep 2022]:
  - windows support
  - hidpi support using scales (see `main.cpp` for usage)
  - minor bug fixes in Makefile
* `v0.6.0` [Sep 2022]:
  - better plot layouts
  - linked axes
  - easier API access (see `main.cpp` for usage examples)
  - ghost cell support
  - icons + custom font support
  - now using implot main branch
  - bump implot version to v0.14
  - minor bugs + code restructure
* `v0.5.2` [Apr 2022]:
  - timestep jumpover to skip rendering certain number of steps (faster)
* `v0.5.1` [Mar 2022]:
  - saving and loading of plotter states
  - "log"-scale: uses `sign(x) * abs(x)^0.25`
  - freetype font rendering

## To-do

- [ ] particle selection
- [ ] custom colormaps
- [x] user-specific outlining
- [x] save/load states
- [x] timestep jumpover
- [x] hidpi support
- [x] subplots & linked axes
- [x] resizing of plots

---

### Note on `glad`

`GLAD` and `KHR` for `GL=4.4` is included in the repo. If for some reason you might have other version of `openGL`, use the following instruction to obtain the proper `.cpp` and `.h` files.

1. Obtain `glad` headers and `glad.c` from [this online server](https://glad.dav1d.de/), for your specific version of OpenGL (use "Profile: Core" and mark the "Generate a loader" tick).

> To find out your OpenGL version run `glxinfo | grep "OpenGL version"`.

2. Download the generated `glad.zip` archive, and unzip it (`unzip glad.zip`). If you do this from the source code directory (`<NTTINY_PATH>`) the headers will be properly placed into `extern/glad` and `extern/KHR` directories (otherwise, do that manually).

3. Move the `glad.c` file to a more appropriate place (and change `.c` to `.cpp`): `mv glad.c lib/glad.cpp`.