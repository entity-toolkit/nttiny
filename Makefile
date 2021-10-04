# # # # # Directories # # # # # # # # # #
#
NTTINY_ROOT_DIR := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
# directory for the building
NTTINY_BUILD_DIR := build
# directory for the executable
NTTINY_BIN_DIR := bin

NTTINY_TARGET := nttiny.example
# static libraries
NTTINY_DEP_LIBRARIES := glfw3

NTTINY_SRC_DIR := nttiny

# for external libraries
# NTTINY_LIB_DIR := lib
NTTINY_EXTERN_DIR := extern

# appending path
# `__` means absolute path will be used ...
# ... these variables are dummy
__BUILD_DIR := ${NTTINY_ROOT_DIR}${NTTINY_BUILD_DIR}
__BIN_DIR := ${NTTINY_ROOT_DIR}${NTTINY_BIN_DIR}
__SRC_DIR := ${NTTINY_ROOT_DIR}${NTTINY_SRC_DIR}
# __LIB_DIR := ${NTTINY_ROOT_DIR}${NTTINY_LIB_DIR}
# __LIBBUILD_DIR := ${NTTINY_ROOT_DIR}${NTTINY_LIB_DIR}/${NTTINY_BUILD_DIR}
__TARGET := ${__BIN_DIR}/${NTTINY_TARGET}

# # external header-only libraries
# IMGUI_INCDIR := $(shell find ${ROOT_DIR}${LIB_DIR}/imgui -type d)
# IMPLOT_INCDIR := $(shell find ${ROOT_DIR}${LIB_DIR}/implot -type d)
# INC_DIR := include include/nttiny $(subst ${ROOT_DIR},,$(IMGUI_INCDIR) $(IMPLOT_INCDIR))

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

NTTINY_EXTERNAL_INCLUDES := glfw/include implot imgui imgui/backends plog/include
NTTINY_INC_DIRS := ${NTTINY_ROOT_DIR} $(shell find ${__SRC_DIR} -type d) ${NTTINY_EXTERN_DIR} $(addprefix ${NTTINY_EXTERN_DIR}/,${NTTINY_EXTERNAL_INCLUDES})
NTTINY_INCFLAGS := $(addprefix -I,$(NTTINY_INC_DIRS))

# # # # # File collection # # # # # # # # # # #
#
NTTINY_SRCS_CXX := $(shell find ${__SRC_DIR} -name \*.cpp -or -name \*.c)
NTTINY_OBJS_CXX := $(subst ${__SRC_DIR},${__BUILD_DIR},$(NTTINY_SRCS_CXX:%=%.o))
NTTINY_DEPS_CXX := $(NTTINY_OBJS_CXX:.o=.d)

# NTTINY_SLIBS_CXX := $(shell find ${__LIB_DIR} -name \*.cpp -or -name \*.c)
# NTTINY_OLIBS_CXX := $(subst ${__LIB_DIR},${__LIBBUILD_DIR},$(NTTINY_SLIBS_CXX:%=%.o))
# NTTINY_DLIBS_CXX := $(NTTINY_OLIBS_CXX:.o=.d)

NTTINY_OBJECTS := $(NTTINY_OBJS_CXX) $(NTTINY_OLIBS_CXX)

# # # # # Targets # # # # # # # # # # # # # #
#

nttiny : ${__TARGET}

${__TARGET} : $(NTTINY_OBJECTS)
	@echo [L]inking $(subst ${NTTINY_ROOT_DIR},,$@) \<: $(subst ${NTTINY_ROOT_DIR},,$^)
	$(HIDE)${NTTINY_LINK} $(NTTINY_OBJECTS) -o $@ $(LDFLAGS)

${__BUILD_DIR}/%.o : ${__SRC_DIR}/%
	@echo [C]ompiling $(subst ${ROOT_DIR},,$@)
	@mkdir -p ${__BIN_DIR}
	@mkdir -p $(dir $@)
	$(HIDE)${NTTINY_CXX} $(NTTINY_INCFLAGS) $(DEFINITIONS) $(NTTINY_CFLAGS) -c $^ -o $@

# define generateRules
# $(1): $(2)
# 	@echo [C]ompiling $(subst ${ROOT_DIR},,$(2))
# 	@mkdir -p ${__BIN_DIR}
# 	@mkdir -p $(dir $(1))
# 	$(HIDE)$(3) $(INCFLAGS) $(DEFINITIONS) $(CPPFLAGS) $(CXXFLAGS) -c $(2) -o $(1)
# endef
# $(foreach obj, $(OBJS_CXX), $(eval $(call generateRules, ${obj}, $(subst ${BUILD_DIR},${SRC_DIR},$(subst .o,,$(obj))), ${CXX})))
# $(foreach obj, $(OBJS_CC), $(eval $(call generateRules, ${obj}, $(subst ${BUILD_DIR},${SRC_DIR},$(subst .o,,$(obj))), ${CC})))
# $(foreach obj, $(OLIBS_CXX), $(eval $(call generateRules, ${obj}, $(subst ${__LIBBUILD_DIR},${__LIB_DIR},$(subst .o,,$(obj))), ${CXX})))


nttiny_help:
	@echo "OS identified as \`${NTTINY_OS}\`"
	@echo
	@echo "usage: \`make nttiny [OPTIONS]\`"
	@echo
	@echo "options:"
	@echo "   DEBUG={y|n}             : enable/disable debug mode [default: ${_DEFAULT_DEBUG}]"
	@echo "   VERBOSE={y|n}           : enable/disable verbose compilation/run mode [default: ${_DEFAULT_VERBOSE}]"
	@echo "   COMPILER={gcc|clang}    : choose the compiler [default: ${_DEFAULT_COMPILER}]"
	@echo
	@echo "cleanup: \`make nttiny_clean\` or \`make nttiny_cleanlib\`"
	@echo
	@echo "to build a static library:"
	@echo "   \`make nttiny_static\`"
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
