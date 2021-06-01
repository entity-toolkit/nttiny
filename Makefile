# Dependencies
#
# 1. glfw
# 2. glad
# 3. plog
# 4. fmt
# 5. imgui
# 
# # # # # Directories # # # # # # # # # # 
#
ROOT_DIR := $(realpath ${CURDIR})/
# directory for the building
BUILD_DIR := build
# directory for the executable
BIN_DIR := bin

TARGET := main
# static libraries 
LIBRARIES := glfw3 fmt

SRC_DIR := src

# for external libraries
LIB_DIR := lib
# external header-only libraries
INC_DIR := include $(shell find ${LIB_DIR}/imgui/ -type d)

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
__LIB_DIR := ${ROOT_DIR}${LIB_DIR}
__LIBBUILD_DIR := ${ROOT_DIR}${LIB_DIR}/${BUILD_DIR}
__SHADER_DIR := ${ROOT_DIR}${SHADER_DIR}
__TARGET := ${__BIN_DIR}/${TARGET}
__SHADERS:= $(shell find ${__SHADER_DIR} -name *.vert -or -name *.frag) 
# # # # # Settings # # # # # # # # # # # # 
# 
_DEFAULT_VERBOSE := n
_DEFAULT_DEBUG := n
_DEFAULT_COMPILER := gcc

VERBOSE ?= ${_DEFAULT_VERBOSE}
DEBUG ?= ${_DEFAULT_DEBUG}
COMPILER ?= ${_DEFAULT_COMPILER}

DEFINITIONS := 

ifeq ($(strip ${VERBOSE}), y)
	HIDE = 
	PREPFLAGS = -DVERBOSE
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
	PREPFLAGS := $(PREPFLAGS) -DDEBUG
else
	CONFFLAGS := -O3 -Ofast
	PREPFLAGS := $(PREPFLAGS) -DNDEBUG
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

SLIBS_CXX := $(shell find ${__LIB_DIR} -name *.cpp) 
OLIBS_CXX := $(subst ${__LIB_DIR},${__LIBBUILD_DIR},$(SLIBS_CXX:%=%.o))
DLIBS_CXX := $(OLIBS_CXX:.o=.d)

INC_DIRS := $(shell find ${__SRC_DIR} -type d) ${INC_DIR} ${LIB_DIR}
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
	@echo "   DEBUG={y|n}             : enable/disable debug mode [default: ${_DEFAULT_DEBUG}]"
	@echo "   VERBOSE={y|n}           : enable/disable verbose compilation/run mode [default: ${_DEFAULT_VERBOSE}]"
	@echo "   COMPILER={gcc|clang}    : choose the compiler [default: ${_DEFAULT_COMPILER}]"
	@echo
	@echo "cleanup: \`make clean\` or \`make cleanlib\`"
	@echo
	@echo "use \`make [CLANG_COMMAND]\` to check the code matches with best practices & consistent stylistics"
	@echo 
	@echo "   make clang-tidy-naming  : test if the naming of variables/functionts/etc is consistent"
	@echo "   make clang-format       : test if the code formatting is consistent"
	@echo "   make clang-format-fix   : same as \`clang-format\` except now fix the issues"
	@echo "   make clang-tidy         : check if the code contains any bad practices or other deprecated features"
	@echo "   make clang-tidy-bugprone: check if the code contains any bug-prone features"
	@echo "   make clang-all          : run \`clang-tidy-naming\`, \`clang-format\` and \`clang-tidy\`"
	@echo 

# linking the main app
all : ${__TARGET} $(SHADER_COPIES)

# copy all the shader files to bin dir
define copyShaders
$(1): $(2)
	@echo [C]opying $(subst ${ROOT_DIR},,$(2))
	$(HIDE)cp -f $(2) $(1)
endef
$(foreach sh, $(__SHADERS), $(eval $(call copyShaders, $(subst ${__SHADER_DIR}, ${__BIN_DIR}, ${sh}), ${sh})))

ALL_OBJECTS := $(OLIBS_CXX) $(OBJS_CXX) $(OBJS_CC)

# main executable
${__TARGET} : $(ALL_OBJECTS)
	@echo [L]inking $(subst ${ROOT_DIR},,$@) \<: $(subst ${ROOT_DIR},,$^)
	$(HIDE)${LINK} $(ALL_OBJECTS) -o $@ $(LDFLAGS) 

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
$(foreach obj, $(OLIBS_CXX), $(eval $(call generateRules, ${obj}, $(subst ${__LIBBUILD_DIR},${__LIB_DIR},$(subst .o,,$(obj))), ${CXX})))

clean:
	rm -rf ${BUILD_DIR} ${BIN_DIR}

cleanlib:
	rm -rf ${LIB_DIR}/${BUILD_DIR}

-include $(DEPS_CXX) $(DEPS_CC) $(DLIBS_CXX)

.PHONY: clang-all, clang-format-fix, clang-format, clang-tidy, clang-tidy-bugprone

SOURCES := $(subst ${ROOT_DIR},,$(SRCS_CC) $(SRCS_CXX))

clang-all : clang-tidy-naming clang-format clang-tidy

clang-tidy-naming:
	@for src in $(SOURCES) ; do \
		echo "checking namings in $$src:" ;\
		clang-tidy -quiet -checks='-*,readability-identifier-naming' \
		    -config="{CheckOptions: [ \
		    { key: readability-identifier-naming.NamespaceCase, value: lower_case },\
		    { key: readability-identifier-naming.ClassCase, value: CamelCase  },\
		    { key: readability-identifier-naming.StructCase, value: CamelCase  },\
		    { key: readability-identifier-naming.FunctionCase, value: camelBack },\
		    { key: readability-identifier-naming.VariableCase, value: lower_case },\
		    { key: readability-identifier-naming.GlobalConstantCase, value: UPPER_CASE }\
		    ]}" "$$src" -- $(INCFLAGS);\
	done
	@echo "clang-tidy-naming -- done"

clang-format:
	@for src in $(SOURCES) ; do \
		var=`clang-format $$src | diff $$src - | wc -l` ; \
		if [ $$var -ne 0 ] ; then \
			diff=`clang-format $$src | diff $$src -` ; \
			echo "$$src:" ; \
			echo "$$diff" ; \
			echo ; \
		fi ; \
	done
	@echo "clang-format -- done"

clang-format-fix:
	@for src in $(SOURCES) ; do \
		var=`clang-format $$src | diff $$src - | wc -l` ; \
		if [ $$var -ne 0 ] ; then \
			echo "formatting $$src:" ;\
			diff=`clang-format $$src | diff $$src -` ; \
			clang-format -i "$$src" ; \
			echo "$$diff" ; \
			echo ; \
		fi ; \
	done
	@echo "clang-format-fix -- done"


clang-tidy:
	@for src in $(SOURCES) ; do \
		echo "tidying $$src:" ; \
		clang-tidy -quiet -checks="-*,\
			clang-diagnostic-*,clang-analyzer-*,modernize-*,-modernize-avoid-c-arrays*,\
			readability-*,performance-*,openmp-*,mpi-*" \
			-header-filter="${SRC_DIR}/.*" \
			"$$src" -- $(INCFLAGS); \
	done
	@echo "clang-tidy -- done"

clang-tidy-bugprone:
	@for src in $(SOURCES) ; do \
		echo "tidying $$src:" ; \
		clang-tidy -quiet -checks="-*,bugprone-*",\
			-header-filter="src/.*" \
			"$$src" -- $(INCFLAGS); \
	done
	@echo "clang-tidy-bugprone -- done"
