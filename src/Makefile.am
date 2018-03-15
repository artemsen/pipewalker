DEFS = @DEFS@ -DNDEBUG=1 -DDIR_GAMEDATA=\""$(datadir)/games/$(PACKAGE)/"\"
bin_PROGRAMS = pipewalker
pipewalker_SOURCES = \
	common.h exception.h \
	main.cpp \
	modepuzzle.h modepuzzle.cpp \
	modesettings.h modesettings.cpp \
	game.h game.cpp \
	map.h map.cpp \
	cell.h cell.cpp \
	settings.h settings.cpp \
	explosion.h explosion.cpp \
	buffer.h buffer.cpp \
	button.h button.cpp \
	image.h image.cpp \
	sound.h sound.cpp \
	synchro.h synchro.cpp \
	texture.h texture.cpp

#MS Windows specific files
EXTRA_DIST = \
	PipeWalkerRes.h
