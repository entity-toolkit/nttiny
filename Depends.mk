depends: ${FREETYPE_TARGET} ${GLFW_TARGET} KHR glad stb imgui implot plog toml11

CMAKE_INSTALL_PREFIX := ${__BUILD_DIR}
CMAKE_BUILD_TYPE := Release
cmake_command := cmake -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}

# ---------------------------------------------------------------------------- #
#                                     glfw3                                    #
# ---------------------------------------------------------------------------- #
glfw3 : ${__BUILD_DIR}/lib/libglfw3.a ${__BUILD_DIR}/include/GLFW/glfw3.h

${__BUILD_DIR}/lib/libglfw3.a ${__BUILD_DIR}/include/GLFW/glfw3.h:
	@echo [B]uilding GLFW
	@mkdir -p $(dir $@)
	@mkdir -p ${__BUILD_DIR}/glfw3_build
	$(HIDE)cd ${__BUILD_DIR}/glfw3_build && ${cmake_command} ${NTTINY_ROOT_DIR}/extern/glfw && $(MAKE) && $(MAKE) install

# ---------------------------------------------------------------------------- #
#                                   freetype                                   #
# ---------------------------------------------------------------------------- #
freetype : ${__BUILD_DIR}/lib/libfreetype.a

FREETYPE_SETTINGS := -D FT_DISABLE_BROTLI=TRUE -D FT_DISABLE_HARFBUZZ=TRUE -D FT_DISABLE_ZLIB=TRUE -D FT_DISABLE_BZIP2=TRUE -D FT_DISABLE_PNG=TRUE

${__BUILD_DIR}/lib/libfreetype.a ${__BUILD_DIR}/include/freetype2/ft2build.h:
	@echo [B]uilding freetype
	@mkdir -p $(dir $@)
	@mkdir -p ${__BUILD_DIR}/freetype_build
	$(HIDE)cd ${__BUILD_DIR}/freetype_build && ${cmake_command} $(FREETYPE_SETTINGS) ${NTTINY_ROOT_DIR}/extern/freetype && $(MAKE) && $(MAKE) install

# ---------------------------------------------------------------------------- #
#                                     glad                                     #
# ---------------------------------------------------------------------------- #
GLAD_OBJECTS := ${__BUILD_DIR}/glad_build/glad.cpp.o

glad : ${__BUILD_DIR}/include/glad/glad.h $(GLAD_OBJECTS)

${__BUILD_DIR}/include/glad/glad.h : ${__EXTERN_DIR}/glad/glad.h
	@echo [C]opying $@
	@mkdir -p $(dir $@)
	$(HIDE)cp $< $@

${__BUILD_DIR}/glad_build/glad.cpp.o : ${__EXTERN_DIR}/glad/glad.cpp
	@echo [C]ompiling $@
	@mkdir -p $(dir $@)
	$(HIDE)${NTTINY_CXX} $(NTTINY_INCFLAGS) $(DEFINITIONS) $(NTTINY_CFLAGS) -MMD -c $< -o $@
	${NTTINY_OBJECTS} += $@

# ---------------------------------------------------------------------------- #
#                                      KHR                                     #
# ---------------------------------------------------------------------------- #
KHR : ${__BUILD_DIR}/include/KHR/khrplatform.h

${__BUILD_DIR}/include/KHR/khrplatform.h : ${__EXTERN_DIR}/KHR/khrplatform.h
	@echo [C]opying $@
	@mkdir -p $(dir $@)
	$(HIDE)cp $< $@

# ---------------------------------------------------------------------------- #
#                                      STB                                     #
# ---------------------------------------------------------------------------- #
stb : ${__BUILD_DIR}/include/stb/stb_image_write.h

${__BUILD_DIR}/include/stb/stb_image_write.h : ${__EXTERN_DIR}/stb/stb_image_write.h
	@echo [C]opying $@
	@mkdir -p $(dir $@)
	$(HIDE)cp $< $@


# ---------------------------------------------------------------------------- #
#                                     ImGui                                    #
# ---------------------------------------------------------------------------- #
IMGUI_HEADERS := $(subst ${__EXTERN_DIR}, ${__BUILD_DIR}/include, $(wildcard ${__EXTERN_DIR}/imgui/*.h)) 
IMGUI_HEADERS += ${__BUILD_DIR}/include/imgui/imgui_impl_glfw.h ${__BUILD_DIR}/include/imgui/imgui_impl_opengl3.h 
IMGUI_SOURCES := $(subst ${__EXTERN_DIR}/imgui, ${__BUILD_DIR}/imgui_build, $(wildcard ${__EXTERN_DIR}/imgui/*.cpp))
IMGUI_SOURCES += ${__BUILD_DIR}/imgui_build/backends/imgui_impl_glfw.cpp ${__BUILD_DIR}/imgui_build/backends/imgui_impl_opengl3.cpp
IMGUI_OBJECTS := $(IMGUI_SOURCES:%.cpp=%.cpp.o)

imgui : ${IMGUI_HEADERS} ${IMGUI_OBJECTS}

${__BUILD_DIR}/include/imgui/imgui_impl_%.h : ${__EXTERN_DIR}/imgui/backends/imgui_impl_%.h
	@echo [C]opying $@
	@mkdir -p $(dir $@)
	$(HIDE)cp $< $@

${__BUILD_DIR}/include/imgui/%.h : ${__EXTERN_DIR}/imgui/%.h
	@echo [C]opying $@
	@mkdir -p $(dir $@)
	$(HIDE)cp $< $@

${__BUILD_DIR}/imgui_build/%.o : ${__EXTERN_DIR}/imgui/%
	@echo [C]ompiling $@
	@mkdir -p $(dir $@)
	$(HIDE)${NTTINY_CXX} $(NTTINY_INCFLAGS) -I${__BUILD_DIR}/include/imgui $(DEFINITIONS) $(NTTINY_CFLAGS) -MMD -c $< -o $@

# ---------------------------------------------------------------------------- #
#                                    ImPlot                                    #
# ---------------------------------------------------------------------------- #
IMPLOT_HEADERS := $(subst ${__EXTERN_DIR}, ${__BUILD_DIR}/include, $(wildcard ${__EXTERN_DIR}/implot/*.h))
IMPLOT_SOURCES := $(subst ${__EXTERN_DIR}/implot, ${__BUILD_DIR}/implot_build, $(wildcard ${__EXTERN_DIR}/implot/*.cpp))
IMPLOT_OBJECTS := $(IMPLOT_SOURCES:%.cpp=%.cpp.o)

implot : ${IMPLOT_HEADERS} ${IMPLOT_OBJECTS}

${__BUILD_DIR}/include/implot/%.h : ${__EXTERN_DIR}/implot/%.h
	@echo [C]opying $@
	@mkdir -p $(dir $@)
	$(HIDE)cp $< $@

${__BUILD_DIR}/implot_build/%.o : ${__EXTERN_DIR}/implot/%
	@echo [C]ompiling $@
	@mkdir -p $(dir $@)
	$(HIDE)${NTTINY_CXX} $(NTTINY_INCFLAGS) -I${__BUILD_DIR}/include/implot -I${__BUILD_DIR}/include/imgui $(DEFINITIONS) $(NTTINY_CFLAGS) -MMD -c $< -o $@

# ---------------------------------------------------------------------------- #
#                                     plog                                     #
# ---------------------------------------------------------------------------- #
plog : ${__BUILD_DIR}/include/plog

${__BUILD_DIR}/include/plog : ${__EXTERN_DIR}/plog/include/plog
	@echo [C]opying plog
	@mkdir -p $(dir $@)
	$(HIDE)cp -r $< $@

# ---------------------------------------------------------------------------- #
#                                    toml11                                    #
# ---------------------------------------------------------------------------- #
toml11 : ${__BUILD_DIR}/include/toml11/

${__BUILD_DIR}/include/toml11/ : ${__EXTERN_DIR}/toml11/toml ${__EXTERN_DIR}/toml11/toml.hpp
	@echo [C]opying $@
	@mkdir -p $(dir $@)
	$(HIDE)cp -r ${__EXTERN_DIR}/toml11/toml ${__BUILD_DIR}/include/toml11/
	$(HIDE)cp ${__EXTERN_DIR}/toml11/toml.hpp ${__BUILD_DIR}/include/toml11/

NTTINY_OBJECTS += $(IMGUI_OBJECTS) $(IMPLOT_OBJECTS) $(GLAD_OBJECTS)
NTTINY_INCFLAGS += -I${__BUILD_DIR}/include/imgui -I${__BUILD_DIR}/include/toml11

ifeq (${COMPILE_GLFW}, y)
	NTTINY_LIBS += ${NTTINY_LIBS} ${__BUILD_DIR}/lib/libglfw3.a
endif

ifeq (${COMPILE_FREETYPE}, y)
	NTTINY_LIBS += ${__BUILD_DIR}/lib/libfreetype.a
	NTTINY_INCFLAGS += -I${__BUILD_DIR}/include/freetype2
endif