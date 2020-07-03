bin_PROGRAMS = pipewalker

pipewalker_SOURCES = \
	main.cpp \
	cell.cpp \
	cell.hpp \
	level.cpp \
	level.hpp \
	level_ui.cpp \
	level_ui.hpp \
	mtrandom.cpp \
	mtrandom.hpp \
	render.cpp \
	render.hpp \
	settings.hpp \
	settings.cpp \
	sound.hpp \
	sound.cpp \
	ui_console.cpp \
	ui_console.hpp \
	ui_sdl.cpp \
	ui_sdl.hpp \
	view.hpp \
	view_level.cpp \
	view_settings.cpp

if DEBUG
AM_CPPFLAGS = -DGAME_DATA_DIR='"$(top_srcdir)/res"'
else
AM_CPPFLAGS = -DGAME_DATA_DIR='"$(datadir)/$(PACKAGE)"'
endif

#if WIN32
#AM_CPPFLAGS = -DPW_GAMEDATADIR='"data\\\\"'
#else
#AM_CPPFLAGS = -DPW_GAMEDATADIR='"$(datadir)/$(PACKAGE)/"'
#endif
#
#WIN32_RESOURCES_SRC_IN = win32_resources.rc.in
#EXTRA_DIST = $(WIN32_RESOURCES_IN)
#
#if WIN32
#WIN32_RESOURCES_SRC = $(abs_builddir)/win32_resources.rc
#WIN32_RESOURCES_OBJ = $(abs_builddir)/win32_resources.o
#pipewalker_DEPENDENCIES = $(WIN32_RESOURCES_OBJ)
#pipewalker_LDADD = $(WIN32_RESOURCES_OBJ)
#
#$(WIN32_RESOURCES_SRC): $(WIN32_RESOURCES_SRC_IN)
#	$(SED) -e "s/\@ver_str\@/$(VERSION)/g" \
#	       -e "s/\@ver_comma\@/$$(echo "$(VERSION).0" | $(SED) 's/\./,/g')/g" $< > $@
#
#$(WIN32_RESOURCES_OBJ): $(WIN32_RESOURCES_SRC)
#	windres $< -o $@
#
## Clean generated files
#clean-local: clean-local-tmp
#.PHONY: clean-local-tmp
#clean-local-tmp:
#	-rm -f $(WIN32_RESOURCES_SRC) $(WIN32_RESOURCES_OBJ)
#endif
