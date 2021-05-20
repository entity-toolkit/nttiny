# # # # # Directories # # # # # # # # # # 
#
ROOT_DIR := $(realpath ${CURDIR})/
# directory for the building
BUILD_DIR := build
# directory for the executable
BIN_DIR := bin

TARGET := main
# static libraries 
LIBRARIES := glfw3

SRC_DIR := src

# external header-only libraries
INC_DIR := include 
# for source codes of external libraries (either static or dynamic)
EXT_DIR := extern
# for static libraries
LIB_DIR := lib 

SHADER_DIR := shaders

OS := $(shell uname -s | tr A-Z a-z)
ifeq (${OS}, darwin)
	FRAMEWORKS := Cocoa OpenGL IOKit
else ifeq (${OS}, linux)
	LIBRARIES := $(LIBRARIES) GL X11 pthread Xrandr Xi dl 
else
	$(error Unrecognized operating system. Unable to set frameworks.)
endif

# appending path
# `__` means absolute path will be used ...
# ... these variables are dummy
__BUILD_DIR := ${ROOT_DIR}${BUILD_DIR}
__BIN_DIR := ${ROOT_DIR}${BIN_DIR}
__SRC_DIR := ${ROOT_DIR}${SRC_DIR}
__SHADER_DIR := ${ROOT_DIR}${SHADER_DIR}
__TARGET := ${__BIN_DIR}/${TARGET}
__SHADERS:= $(shell find ${__SHADER_DIR} -name *.vert -or -name *.frag) 
# # # # # Settings # # # # # # # # # # # # 
# 
VERBOSE ?= n
DEBUG ?= y
COMPILER ?= gcc

DEFINITIONS := 

ifeq ($(strip ${VERBOSE}), y)
	HIDE = 
else
	HIDE = @
endif

# 3-rd party library configurations
# ...

# # # # # Compiler and flags # # # # # # #  
# 
ifeq ($(strip ${COMPILER}), gcc)
	CC := gcc
	CXX := g++
else
	CC := clang
	CXX := clang++
endif
LINK := ${CXX}
CXXSTANDARD := -std=c++17
CXX := ${CXX} ${CXXSTANDARD}
ifeq ($(strip ${DEBUG}), y)
	CONFFLAGS := -O0 -g
	PREPFLAGS := -DDEBUG
else
	CONFFLAGS := -O3 -Ofast
	PREPFLAGS := -DNDEBUG
endif

# warning flags
WARNFLAGS := -Wall -Wextra -pedantic

# custom preprocessor flags
# ... 

CPPFLAGS := $(WARNFLAGS) $(PREPFLAGS) 
CXXFLAGS := $(CXXFLAGS) $(CONFFLAGS)

# # # # # File collection # # # # # # # # # # # 
# 
SRCS_CXX := $(shell find ${__SRC_DIR} -name *.cpp) 
OBJS_CXX := $(subst ${__SRC_DIR},${__BUILD_DIR},$(SRCS_CXX:%=%.o))
DEPS_CXX := $(OBJS_CXX:.o=.d)

SRCS_CC := $(shell find ${__SRC_DIR} -name *.c)
OBJS_CC := $(subst ${__SRC_DIR},${__BUILD_DIR},$(SRCS_CC:%=%.o))
DEPS_CC := $(OBJS_CC:.o=.d)

INC_DIRS := $(shell find ${__SRC_DIR} -type d) ${EXT_DIR} ${INC_DIR} ${LIB_DIRS}
INCFLAGS := $(addprefix -I,${INC_DIRS})

LDFLAGS := $(LDFALGS) $(addprefix -L, $(LIB_DIR)) $(addprefix -l, $(LIBRARIES)) $(addprefix -framework , $(FRAMEWORKS))

SHADER_COPIES = $(subst ${__SHADER_DIR}, ${__BIN_DIR}, $(__SHADERS))
# # # # # Targets # # # # # # # # # # # # # # 
# 
.PHONY: all help default clean 

default : help

help:
	@echo "OS identified as \`${OS}\`"
	@echo 
	@echo "usage: \`make all [OPTIONS]\`"
	@echo 
	@echo "options:"
	@echo "   DEBUG={y|n}      : enable/disable debug mode [default: y]"
	@echo "   VERBOSE={y|n}    : enable/disable verbose compilation mode [default: n]"
	@echo
	@echo "cleanup: \`make clean\`"

# linking the main app
all : ${__TARGET} $(SHADER_COPIES)

# copy all the shader files to bin dir
define copyShaders
$(1): $(2)
	@echo [C]opying $(subst ${ROOT_DIR},,$(2))
	$(HIDE)cp -f $(2) $(1)
endef
$(foreach sh, $(__SHADERS), $(eval $(call copyShaders, $(subst ${__SHADER_DIR}, ${__BIN_DIR}, ${sh}), ${sh})))

# main executable
${__TARGET} : $(OBJS_CXX) $(OBJS_CC)
	@echo [L]inking $(subst ${ROOT_DIR},,$@) \<: $(subst ${ROOT_DIR},,$^)
	$(HIDE)${LINK} $(OBJS_CXX) $(OBJS_CC) -o $@ $(LDFLAGS) 

# generate compilation rules for all `.o` files
define generateRules
$(1): $(2)
	@echo [C]ompiling $(subst ${ROOT_DIR},,$(2))
	@mkdir -p ${BIN_DIR}
	@mkdir -p $(dir $(1))
	$(HIDE)$(3) $(INCFLAGS) $(DEFINITIONS) $(CPPFLAGS) $(CXXFLAGS) -c $(2) -o $(1)
endef
$(foreach obj, $(OBJS_CXX), $(eval $(call generateRules, ${obj}, $(subst ${BUILD_DIR},${SRC_DIR},$(subst .o,,$(obj))), ${CXX})))
$(foreach obj, $(OBJS_CC), $(eval $(call generateRules, ${obj}, $(subst ${BUILD_DIR},${SRC_DIR},$(subst .o,,$(obj))), ${CC})))

clean:
	rm -rf ${BUILD_DIR} ${BIN_DIR}

-include $(DEPS_CXX) $(DEPS_CC)
