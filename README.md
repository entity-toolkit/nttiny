# `nttiny`

OpenGL-based interactive plotting library for scientific computing.

```shell
git clone --recursive git@github.com:haykh/nttiny.git
# to update submodules
git submodule update --remote
```

## Compilation

Compile an example code as a standalone application with the `make nttiny -j [OPTIONS]` command. To see the help menu with all the options type `make`.

To build a static library simply run `make nttiny_static [OPTIONS]`.

## Usage

File `src/main.cpp` contains the most comprehensive examples on how to address the `nttiny`. To compile the examples simply run `make nttiny -j`. Provided that all the dependencies are satisfied, the executable `nttiny.example` will be created in the `bin` directory.

To use `nttiny` as a plotting tool for your code you would need to build and use `nttiny` as a static library. To do that simply include `nttiny/Makefile` in your project's Makefile, add a dependency of your desired target on `nttiny_static`, and use `NTTINY_LINKFLAGS` and `NTTINY_INCFLAGS` for linking/compilation. A typical Makefile would look something like this:

```Makefile
all: nttiny_static myapp

myapp : $(OBJS)
	${CXX} $< -o $@ $(NTTINY_LINKFLAGS)

%.cpp.o : %.cpp
	${CXX} $(NTTINY_INCFLAGS) -c $^ -o $@

# assuming `nttiny` is in `extern` directory
include extern/nttiny/Makefile
```

> Alternatively you could simply compile `nttiny` as a static library (`make nttiny_static -j`) and link the generated file to your project.

## Releases
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

## Compile commands

Type `nttiny_help` to see the list of available commands.

```shell
usage: `make nttiny [OPTIONS]`

options:
   DEBUG={y|n}             : enable/disable debug mode [default: n]
   VERBOSE={y|n}           : enable/disable verbose compilation/run mode [default: n]
   COMPILER={g++|clang++}  : choose the compiler [default: g++]
   COMPILE_GLFW={y|n}      : compile glfw3 or use system default [default: y]
   COMPILE_FREETYPE={y|n}  : use freetype for font rasterization [default: y]

cleanup: `make nttiny_cleanall`
   also: `make nttiny_clean` to clean just the `nttiny`
         `make nttiny_cleanlib` to clean just the compiled libraries

to build a static library:
   `make nttiny_static`

exported variables to use when including nttiny:
    ${NTTINY_INCFLAGS}
    ${NTTINY_LINKFLAGS}
    ${NTTINY_LIBS}
```

---

### Note on `glad`

`GLAD` and `KHR` for `GL=4.4` is included in the repo. If for some reason you might have other version of `openGL`, use the following instruction to obtain the proper `.cpp` and `.h` files.

1. Obtain `glad` headers and `glad.c` from [this online server](https://glad.dav1d.de/), for your specific version of OpenGL (use "Profile: Core" and mark the "Generate a loader" tick).

> To find out your OpenGL version run `glxinfo | grep "OpenGL version"`.

2. Download the generated `glad.zip` archive, and unzip it (`unzip glad.zip`). If you do this from the source code directory (`<NTTINY_PATH>`) the headers will be properly placed into `extern/glad` and `extern/KHR` directories (otherwise, do that manually).

3. Move the `glad.c` file to a more appropriate place (and change `.c` to `.cpp`): `mv glad.c lib/glad.cpp`.

### Note on `glfw`

Your system might already have a globally preinstalled `glfw` (to test that run `g++ -lglfw`). In this case you can avoid compiling `glfw3` with the code: simply compile with the `COMPILE_GLFW=n` flag.

### Note on `freetype`

On some of the hidpi monitors using freetype for font rasterization may improve the text quality. To enable/disable this behavior set `COMPILE_FREETYPE=y/n` during compilation (set to `n` by default).