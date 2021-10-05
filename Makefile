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
NTTINY_LIBRARIES := glfw3

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
	NTTINY_LIBRARIES := $(NTTINY_LIBRARIES) GL X11 pthread Xrandr Xi dl
else
	$(error Unrecognized operating system. Unable to set frameworks.)
endif

VERBOSE ?= n
DEBUG ?= n
COMPILER ?= g++

DEFINITIONS :=

ifeq ($(strip ${VERBOSE}), y)
	HIDE =
	NTTINY_PREPFLAGS = -DVERBOSE
else
	HIDE = @
endif

# # # # # Compiler and flags # # # # # # #
#
ifeq ($(strip ${COMPILER}), gcc)
	NTTINY_CXX := g++
else
	NTTINY_CXX := clang++
endif
NTTINY_LINK := ${NTTINY_CXX}
NTTINY_CXXSTANDARD := -std=c++17
NTTINY_CXX := ${NTTINY_CXX} ${NTTINY_CXXSTANDARD}
ifeq ($(strip ${DEBUG}), y)
	NTTINY_CFLAGS := $(NTTINY_PREPFLAGS) -O0 -g -DDEBUG
else
	NTTINY_CFLAGS := $(NTTINY_PREPFLAGS) -O3 -Ofast -DNDEBUG
endif
NTTINY_WARNFLAGS := -Wall -Wextra -pedantic
NTTINY_CFLAGS := $(NTTINY_WARNFLAGS) $(NTTINY_CFLAGS)

NTTINY_EXTERNAL_INCLUDES := glfw/include implot imgui imgui/backends plog/include KHR
NTTINY_INC_DIRS := ${NTTINY_ROOT_DIR} $(shell find ${__SRC_DIR} -type d) ${__EXTERN_DIR} $(addprefix ${__EXTERN_DIR}/,${NTTINY_EXTERNAL_INCLUDES})
NTTINY_INCFLAGS := $(addprefix -I,$(NTTINY_INC_DIRS))

NTTINY_LDFLAGS := $(addprefix -L, $(__BUILD_DIR)/lib)
NTTINY_LDFLAGS := $(NTTINY_LDFLAGS) $(addprefix -l, $(NTTINY_LIBRARIES))
NTTINY_LDFLAGS := $(NTTINY_LDFLAGS) $(addprefix -framework , $(NTTINY_FRAMEWORKS))

# # # # # File collection # # # # # # # # # # #
#
NTTINY_SRCS_CXX := $(shell find ${__SRC_DIR} -name \*.cpp -or -name \*.c)
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
	@echo
	@echo "cleanup: \`make nttiny_clean\` or \`make nttiny_cleanlib\`"
	@echo
	@echo "to build a static library:"
	@echo "   \`make nttiny_static\`"

nttiny : glfw3 ${__TARGET}

nttiny_static : glfw3 ${__STATIC}

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
	$(HIDE)${NTTINY_CXX} $(NTTINY_INCFLAGS) $(DEFINITIONS) $(NTTINY_CFLAGS) -MMD -c $^ -o $@

${__BUILD_DIR}/lib/%.o : ${__EXTERN_DIR}/%
	@echo [C]ompiling $(subst ${ROOT_DIR},,$@)
	@mkdir -p ${__BIN_DIR}
	@mkdir -p $(dir $@)
	$(HIDE)${NTTINY_CXX} $(NTTINY_INCFLAGS) $(DEFINITIONS) $(NTTINY_CFLAGS) -MMD -c $^ -o $@

glfw3 : ${__BUILD_DIR}/lib/libglfw3.a

${__BUILD_DIR}/lib/libglfw3.a : ${__EXTERN_DIR}/glfw/build/src/libglfw3.a
	@echo [B]uilding GLFW
	$(HIDE)cd ${NTTINY_ROOT_DIR}/extern/glfw && cmake -B build && cd build && $(MAKE) -j `ncores`
	@mkdir -p $(dir $@)
	$(HIDE)cp $< $@

nttiny_clean:
	find ${__BUILD_DIR} -mindepth 1 -name lib -prune -o -exec rm -rf {} +
	rm -rf ${__BIN_DIR}

nttiny_cleanlib:
	rm -rf ${__BUILD_DIR}/lib

-include $(NTTINY_DEPS_CXX) $(NTTINY_DLIBS_CXX)

NTTINY_LINKFLAGS := $(NTTINY_LDFLAGS) $(addprefix -L, ${__BUILD_DIR}) $(addprefix -l, nttiny)

# exported variables to use in the upstream:
# . . . ${NTTINY_INCFLAGS}
# . . . ${NTTINY_LINKFLAGS}

# @echo
# @echo "---------------"
# @echo "for developers:"
# @echo
# @echo "use \`make [CLANG_COMMAND]\` to check the code matches with best practices & consistent stylistics"
# @echo
# @echo "list of all \`[CLANG_COMMAND]\`-s:"
# @echo "   clang-tidy-naming       : test if the naming of variables/functionts/etc is consistent"
# @echo "   clang-format            : test if the code formatting is consistent"
# @echo "   clang-format-fix        : same as \`clang-format\` except now fix the issues"
# @echo "   clang-tidy              : check if the code contains any bad practices or other deprecated features"
# @echo "   clang-tidy-bugprone     : check if the code contains any bug-prone features"
# @echo "   clang-all               : run \`clang-tidy-naming\`, \`clang-format\` and \`clang-tidy\`"
# @echo
