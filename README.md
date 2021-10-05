# `nttiny v0.4`

OpenGL-based interactive plotting library.

```shell
git clone --recursive git@github.com:haykh/nttiny.git
# to update submodules
git submodule update --remote
```

## Compilation

Compile an example code as a standalone application with the `make nttiny [OPTIONS]` command. To see the help menu with all the options type `make`.

To build a static library simply run `make nttiny_static [OPTIONS]`.

## Usage

File `src/main.cpp` contains an example usage of the `nttiny` as a standalone app. In most of the scenarios, however, you would need to build and use `nttiny` as a static library. To do that simply include `nttiny/Makefile` in your project's Makefile, add a dependency of your desired target on `nttiny_static`, and use `NTTINY_LINKFLAGS` and `NTTINY_INCFLAGS` for linking/compilation. A typical Makefile would look something like this:

```Makefile
all: nttiny_static myapp

myapp : $(OBJS)
	${CXX} $< -o $@ $(NTTINY_LINKFLAGS)

%.cpp.o : %.cpp
	${CXX} $(NTTINY_INCFLAGS) -c $^ -o $@

# assuming `nttiny` is in `extern` directory
include extern/nttiny/Makefile
```

---

### Note on `glad`

`GLAD` and `KHR` for `GL=4.4` is included in the repo. If for some reason you might have other version of `openGL`, use the following instruction to obtain the proper `.cpp` and `.h` files.

1. Obtain `glad` headers and `glad.c` from [this online server](https://glad.dav1d.de/), for your specific version of OpenGL (use "Profile: Core" and mark the "Generate a loader" tick).

> To find out your OpenGL version run `glxinfo | grep "OpenGL version"`.

2. Download the generated `glad.zip` archive, and unzip it (`unzip glad.zip`). If you do this from the source code directory (`<NTTINY_PATH>`) the headers will be properly placed into `extern/glad` and `extern/KHR` directories (otherwise, do that manually).

3. Move the `glad.c` file to a more appropriate place (and change `.c` to `.cpp`): `mv glad.c lib/glad.cpp`.

### Note on `glfw`

Your system might already have a globally preinstalled `glfw` (to test that run `g++ -lglfw`). In this case you can avoid compiling `glfw3` with the code. To fix the Makefile for that simply set `NTTINY_LIBRARIES := glfw` (instead of `glfw3`), and remove the `glfw3` dependency from the following targets: `nttiny : glfw3 ${__TARGET}`, `nttiny_static : glfw3 ${__STATIC}`.
