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

.PHONY: clang-all clang-tidy-naming clang-format-fix clang-format clang-tidy clang-tidy-bugprone

SOURCES := $(subst ${ROOT_DIR},,$(SRCS_CC) $(SRCS_CXX))
ALLCODE := $(subst ${ROOT_DIR},,$(SRCS_CC) $(SRCS_CXX) $(shell find ${__SRC_DIR} -name \*.h))

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
		    ]}" "$$src" -extra-arg=${CXXSTANDARD} -- $(INCFLAGS);\
	done
	@echo "clang-tidy-naming -- done"

clang-format:
	@for src in $(ALLCODE) ; do \
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
	@for src in $(ALLCODE) ; do \
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
			readability-*,performance-*,openmp-*,mpi-*,-performance-no-int-to-ptr" \
			-header-filter="${SRC_DIR}/.*" \
			"$$src" -extra-arg=${CXXSTANDARD} -- $(INCFLAGS); \
	done
	@echo "clang-tidy -- done"

clang-tidy-bugprone:
	@for src in $(SOURCES) ; do \
		echo "tidying $$src:" ; \
		clang-tidy -quiet -checks="-*,bugprone-*",\
			-header-filter="src/.*" \
			"$$src" -extra-arg=${CXXSTANDARD} -- $(INCFLAGS); \
	done
	@echo "clang-tidy-bugprone -- done"
