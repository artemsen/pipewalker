DEFS = @DEFS@ -DNDEBUG=1 -DDIR_GAMEDATA=\""$(datadir)/games/$(PACKAGE)/"\"
bin_PROGRAMS = pipewalker
pipewalker_SOURCES = \
	common.h exception.h \
	main.cpp \
	game.h game.cpp \
	map.h map.cpp \
	cell.h cell.cpp \
	serializer.h serializer.cpp \
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
