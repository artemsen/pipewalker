DEFS = @DEFS@ -DPW_SYSTEM_NIX=1 -DNDEBUG=1 -DPW_GAMEDATA_DIR=\""$(datadir)/games/$(PACKAGE)"\"
bin_PROGRAMS = pipewalker
pipewalker_SOURCES = \
    base.h common.h \
    main.cpp \
    game.h game.cpp \
    cell.h cell.cpp \
    opengl.h opengl.cpp \
    globjects.h globjects.cpp \
    tgaloader.h tgaloader.cpp \
    wss_x11.h wss_x11.cpp \
    wss_glut.h wss_glut.cpp \
    PipeWalker.xpm

#MS Windows specific files
EXTRA_DIST = \
    wss_winnt.h wss_winnt.cpp \
    PipeWalker.rc PipeWalkerRes.h
