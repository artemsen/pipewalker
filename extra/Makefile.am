DESKTOP_ENTRY = pipewalker.desktop
APPLICATION_ICON = pipewalker.xpm
MENU_ENTRY = pipewalker.menu.in

man_MANS = pipewalker.6

EXTRA_DIST = \
	$(DESKTOP_ENTRY) \
	$(APPLICATION_ICON) \
	$(MENU_ENTRY)
	$(man_MANS) \
	pipewalker.ico \
	scheme.png

if MENU
pkgdesktopdatadir = $(datarootdir)/applications
pkgdesktopdata_DATA = $(DESKTOP_ENTRY)

pkgpixmapdatadir = $(datarootdir)/pixmaps
pkgpixmapdata_DATA = $(APPLICATION_ICON)

MENU_ENTRY_OUT = $(abs_builddir)/pipewalker
$(MENU_ENTRY_OUT): $(MENU_ENTRY)
	$(SED) -e "s|\@datarootdir\@|$(datarootdir)|g" \
	       -e "s|\@bindir\@|$(bindir)|g" $< > $@

pkgmenudatadir = $(datarootdir)/menu
pkgmenudata_DATA = $(MENU_ENTRY_OUT)

# Clean generated files
clean-local: clean-local-tmp
.PHONY: clean-local-tmp
clean-local-tmp:
	-rm -f $(MENU_ENTRY_OUT)
endif
