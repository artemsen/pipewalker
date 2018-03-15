DEFS = @DEFS@ -DNDEBUG=1 -DPW_USE_SDL=1 -DDIR_GAMEDATA=\""$(datadir)/games/$(PACKAGE)/"\"
bin_PROGRAMS = pipewalker
pipewalker_SOURCES = \
	buffer.h buffer.cpp \
	button.h button.cpp \
	cell.h cell.cpp \
	common.h \
	exception.h \
	explosion.h explosion.cpp \
	game.h game.cpp \
	image.h image.cpp \
	main.cpp \
	map.h map.cpp \
	modepuzzle.h modepuzzle.cpp \
	modesettings.h modesettings.cpp \
	mtrandom.h mtrandom.cpp \
	rendertext.h rendertext.cpp \
	settings.h settings.cpp \
	sound.h sound.cpp \
	synchro.h synchro.cpp \
	texture.h texture.cpp \
	winmgr.h winmgr.cpp \
	winmgr_sdl.h winmgr_sdl.cpp

#MS Windows specific files
EXTRA_DIST = \
	winmgr_win.h winmgr_win.cpp \
	PipeWalkerRes.h PipeWalker.rc
