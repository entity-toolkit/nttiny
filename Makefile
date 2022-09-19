# # # # # Directories # # # # # # # # # #
#
NTTINY_ROOT_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
# directory for the building
NTTINY_BUILD_DIR := build
# directory for the executable
NTTINY_BIN_DIR := bin

NTTINY_TARGET := nttiny.example
NTTINY_STATIC := libnttiny.a
# static libraries
COMPILE_GLFW ?= y
GLFW_TARGET :=
ifeq (${COMPILE_GLFW}, y)
	GLFW_TARGET := glfw3
	NTTINY_LIBRARIES := glfw3
else
	GLFW_TARGET :=
	NTTINY_LIBRARIES := glfw
endif

COMPILE_FREETYPE ?= y
ifeq (${COMPILE_FREETYPE}, y)
	FREETYPE_TARGET := freetype
	NTTINY_LIBRARIES += freetype
	IMGUI_FREETYPE_FLAG := -DIMGUI_ENABLE_FREETYPE 
else
	FREETYPE_TARGET := 
endif

NTTINY_SRC_DIR := nttiny

# for external libraries
NTTINY_EXTERN_DIR := extern

# appending path
# `__` means absolute path will be used ...
# ... these variables are dummy
__BUILD_DIR := ${NTTINY_ROOT_DIR}${NTTINY_BUILD_DIR}
__BIN_DIR := ${NTTINY_ROOT_DIR}${NTTINY_BIN_DIR}
__SRC_DIR := ${NTTINY_ROOT_DIR}${NTTINY_SRC_DIR}
__EXTERN_DIR := ${NTTINY_ROOT_DIR}${NTTINY_EXTERN_DIR}
__TARGET := ${__BIN_DIR}/${NTTINY_TARGET}
__STATIC := ${__BUILD_DIR}/${NTTINY_STATIC}

# # # # # Settings # # # # # # # # # # # #
#
NTTINY_OS := $(shell uname -s | tr A-Z a-z)
ifeq (${NTTINY_OS}, darwin)
	NTTINY_FRAMEWORKS := Cocoa OpenGL IOKit
else ifeq (${NTTINY_OS}, linux)
	NTTINY_LIBRARIES += GL X11 pthread Xrandr Xi dl
else
	COMPILE_GLFW := n
	COMPILE_FREETYPE := n
	NTTINY_OS := windows
	NTTINY_LIBRARIES := glfw3 gdi32 opengl32 imm32
	NTTINY_CFLAGS := `pkg-config --cflags glfw3`
	IMGUI_FREETYPE_FLAG :=
	GLFW_TARGET :=
	FREETYPE_TARGET :=
endif

VERBOSE ?= n
DEBUG ?= n
COMPILER ?= g++

DEFINITIONS := ${IMGUI_FREETYPE_FLAG}

ifeq ($(strip ${VERBOSE}), y)
	HIDE =
	NTTINY_PREPFLAGS = -DVERBOSE
else
	HIDE = @
endif

# # # # # Compiler and flags # # # # # # #
#
NTTINY_CXX := ${COMPILER}
NTTINY_LINK := ${NTTINY_CXX}
NTTINY_CXXSTANDARD := -std=c++17
NTTINY_CXX := ${NTTINY_CXX} ${NTTINY_CXXSTANDARD}
ifeq ($(strip ${DEBUG}), y)
	NTTINY_CFLAGS += $(NTTINY_PREPFLAGS) -O0 -g -DDEBUG -fPIE
else
	NTTINY_CFLAGS += $(NTTINY_PREPFLAGS) -O3 -Ofast -DNDEBUG -fPIE
endif
NTTINY_WARNFLAGS := -Wall -Wextra
NTTINY_CFLAGS := $(NTTINY_WARNFLAGS) $(NTTINY_CFLAGS)

NTTINY_EXTERNAL_INCLUDES := glfw/include implot imgui imgui/backends plog/include KHR toml11 freetype/include
NTTINY_INC_DIRS := ${NTTINY_ROOT_DIR} $(filter %/, $(wildcard ${__SRC_DIR}/**/)) ${__EXTERN_DIR} $(addprefix ${__EXTERN_DIR}/,${NTTINY_EXTERNAL_INCLUDES})
NTTINY_INCFLAGS := $(addprefix -I,$(NTTINY_INC_DIRS))

NTTINY_LDFLAGS := $(addprefix -L, $(__BUILD_DIR)/lib)
NTTINY_LDFLAGS := $(NTTINY_LDFLAGS) $(addprefix -l, $(NTTINY_LIBRARIES))
NTTINY_LDFLAGS := $(NTTINY_LDFLAGS) $(addprefix -framework , $(NTTINY_FRAMEWORKS))

# # # # # File collection # # # # # # # # # # #
#
NTTINY_SRCS_CXX := $(wildcard ${__SRC_DIR}/*.cpp) $(wildcard ${__SRC_DIR}/**/*.cpp)
ifeq (${COMPILE_FREETYPE},n)
	NTTINY_SRCS_CXX := $(filter-out ${__SRC_DIR}/assets/imgui_freetype.cpp, ${NTTINY_SRCS_CXX})
endif
NTTINY_OBJS_CXX := $(subst ${__SRC_DIR},${__BUILD_DIR},$(NTTINY_SRCS_CXX:%=%.o))
NTTINY_DEPS_CXX := $(NTTINY_OBJS_CXX:.o=.d)

NTTINY_EXTERNAL_LIBS := glad/glad.cpp imgui/backends/imgui_impl_opengl3.cpp imgui/backends/imgui_impl_glfw.cpp
NTTINY_SLIBS_CXX := $(addprefix ${__EXTERN_DIR}/, $(NTTINY_EXTERNAL_LIBS)) $(wildcard ${__EXTERN_DIR}/imgui/*.cpp)
NTTINY_SLIBS_CXX := $(NTTINY_SLIBS_CXX) $(wildcard ${__EXTERN_DIR}/implot/*.cpp)
NTTINY_OLIBS_CXX := $(subst ${__EXTERN_DIR},${__BUILD_DIR}/lib,$(NTTINY_SLIBS_CXX:%=%.o))
NTTINY_DLIBS_CXX := $(NTTINY_OLIBS_CXX:.o=.d)

NTTINY_OBJECTS := $(NTTINY_OLIBS_CXX) $(NTTINY_OBJS_CXX)

# # # # # Targets # # # # # # # # # # # # # #
#
nttiny_help:
	@echo "OS identified as \`${NTTINY_OS}\`"
	@echo
	@echo "usage: \`make nttiny [OPTIONS]\`"
	@echo
	@echo "options:"
	@echo "   DEBUG={y|n}             : enable/disable debug mode [default: n]"
	@echo "   VERBOSE={y|n}           : enable/disable verbose compilation/run mode [default: n]"
	@echo "   COMPILER={g++|clang++}  : choose the compiler [default: g++]"
	@echo "   COMPILE_GLFW={y|n}      : compile glfw3 or use system default [default: y]"
	@echo "   COMPILE_FREETYPE={y|n}  : use freetype for font rasterization [default: y]"
	@echo
	@echo "note: windows compilation disables COMPILE_GLFW and COMPILE_FREETYPE"
	@echo
	@echo "cleanup: \`make nttiny_cleanall\`"
	@echo "   also: \`make nttiny_clean\` to clean just the \`nttiny\`"
	@echo "         \`make nttiny_cleanlib\` to clean just the compiled libraries"
	@echo
	@echo "to build a static library:"
	@echo "   \`make nttiny_static\`"
	@echo 
	@echo "exported variables to use when including nttiny:"
	@echo '    $${NTTINY_INCFLAGS}'
	@echo '    $${NTTINY_LINKFLAGS}'
	@echo '    $${NTTINY_LIBS}'

nttiny : ${FREETYPE_TARGET} ${GLFW_TARGET} ${__TARGET}

nttiny_static : ${FREETYPE_TARGET} ${GLFW_TARGET} ${__STATIC}

${__STATIC} : $(filter-out %/main.cpp.o, $(NTTINY_OBJECTS))
	@echo [A]rchiving $(subst ${NTTINY_ROOT_DIR},,$@) \<: $(subst ${NTTINY_ROOT_DIR},,$^)
	$(HIDE)ar -rcs $@ $^

${__TARGET} : $(NTTINY_OBJECTS)
	@echo [L]inking $(subst ${NTTINY_ROOT_DIR},,$@) \<: $(subst ${NTTINY_ROOT_DIR},,$^)
	$(HIDE)${NTTINY_LINK} $(NTTINY_OBJECTS) -o $@ $(NTTINY_LDFLAGS)

${__BUILD_DIR}/%.o : ${__SRC_DIR}/%
	@echo [C]ompiling $(subst ${ROOT_DIR},,$@)
	@mkdir -p ${__BIN_DIR}
	@mkdir -p $(dir $@)
	$(HIDE)${NTTINY_CXX} $(NTTINY_INCFLAGS) $(DEFINITIONS) $(NTTINY_CFLAGS) -MMD -c $< -o $@

${__BUILD_DIR}/lib/%.o : ${__EXTERN_DIR}/%
	@echo [C]ompiling $(subst ${ROOT_DIR},,$@)
	@mkdir -p ${__BIN_DIR}
	@mkdir -p $(dir $@)
	$(HIDE)${NTTINY_CXX} $(NTTINY_INCFLAGS) $(DEFINITIONS) $(NTTINY_CFLAGS) -MMD -c $< -o $@

glfw3 : ${__BUILD_DIR}/lib/libglfw3.a

${__BUILD_DIR}/lib/libglfw3.a : ${__EXTERN_DIR}/glfw/build/src/libglfw3.a
	@mkdir -p $(dir $@)
	$(HIDE)cp $< $@

${__EXTERN_DIR}/glfw/build/src/libglfw3.a :
	@echo [B]uilding GLFW
	$(HIDE)cd ${NTTINY_ROOT_DIR}/extern/glfw && cmake -B build && cd build && $(MAKE) -j `ncores`

freetype : ${__BUILD_DIR}/lib/libfreetype.a

${__BUILD_DIR}/lib/libfreetype.a : ${__EXTERN_DIR}/freetype/build/libfreetype.a
	@mkdir -p $(dir $@)
	$(HIDE)cp $< $@

FREETYPE_SETTINGS := -D FT_DISABLE_BROTLI=TRUE -D FT_DISABLE_HARFBUZZ=TRUE -D FT_DISABLE_ZLIB=TRUE -D FT_DISABLE_BZIP2=TRUE -D FT_DISABLE_PNG=TRUE

${__EXTERN_DIR}/freetype/build/libfreetype.a :
	@echo [B]uilding freetype
	$(HIDE)cd ${NTTINY_ROOT_DIR}/extern/freetype && cmake -B build $(FREETYPE_SETTINGS) && cd build && $(MAKE) -j `ncores`

nttiny_clean:
	rm -rf ${__BUILD_DIR}/*.o ${__BUILD_DIR}/*.d $(filter-out ${__BUILD_DIR}/lib/, $(wildcard ${__BUILD_DIR}/**/))
	rm -rf ${__BIN_DIR}

nttiny_cleanlib:
	rm -rf ${__EXTERN_DIR}/glfw/build
	rm -rf ${__BUILD_DIR}/lib
	rm -rf ${__EXTERN_DIR}/freetype/build

nttiny_cleanall : nttiny_clean nttiny_cleanlib

-include $(NTTINY_DEPS_CXX) $(NTTINY_DLIBS_CXX)

NTTINY_LINKFLAGS := $(NTTINY_LDFLAGS) $(addprefix -L, ${__BUILD_DIR}) $(addprefix -l, nttiny)
NTTINY_LIBS := ${__BUILD_DIR}/libnttiny.a
ifeq (${COMPILE_GLFW}, y)
	NTTINY_LIBS := ${NTTINY_LIBS} ${__BUILD_DIR}/lib/libglfw3.a
endif

ifeq (${COMPILE_FREETYPE}, y)
	NTTINY_LIBS += ${__BUILD_DIR}/lib/libfreetype.a
endif

.PHONY: nttiny nttiny_static nttiny_clean nttiny_cleanall nttiny_cleanlib glfw3 freetype

# exported variables to use in the upstream:
# . . . ${NTTINY_INCFLAGS}
# . . . ${NTTINY_LINKFLAGS}
# . . . ${NTTINY_LIBS}
