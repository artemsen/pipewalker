DEFS = @DEFS@ -DNDEBUG=1 -DPW_USE_SDL=1 -DDIR_GAMEDATA=\""$(datadir)/games/$(PACKAGE)/"\"
bin_PROGRAMS = pipewalker
pipewalker_SOURCES = \
	buffer.cpp buffer.h \
	button.cpp button.h \
	cell.cpp cell.h	\
	common.h \
	exception.h \
	explosion.cpp explosion.h \
	game.cpp game.h \
	image.cpp image.h \
	main.cpp \
	map.cpp map.h \
	modepuzzle.cpp modepuzzle.h \
	modesettings.cpp modesettings.h \
	rendertext.cpp rendertext.h \
	settings.cpp settings.h \
	sound.cpp sound.h \
	synchro.cpp synchro.h \
	texture.cpp texture.h \
	winmgr.cpp winmgr.h \
	winmgr_sdl.cpp winmgr_sdl.h

#MS Windows specific files
EXTRA_DIST = \
	winmgr_win.cpp winmgr_win.h \
	PipeWalker.rc PipeWalkerRes.h
