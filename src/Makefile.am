bin_PROGRAMS = pipewalker
pipewalker_SOURCES = \
	button.cpp button.h \
	cell.cpp cell.h \
	common.h \
	explosion.cpp explosion.h \
	game.cpp game.h \
	image.cpp image.h \
	level.cpp level.h \
	loading_xpm.h \
	main.cpp \
	mode.h \
	mode_puzzle.cpp mode_puzzle.h \
	mode_settings.cpp mode_settings.h \
	mtrandom.cpp mtrandom.h \
	render.cpp render.h \
	settings.cpp settings.h \
	sound.cpp sound.h

if WIN32
pipewalker_DEPENDENCIES = pipewalker_win32_res.o
pipewalker_LDADD = pipewalker_win32_res.o
endif

pipewalker_win32_res.rc:
	@echo "#include <windows.h>" > pipewalker_win32_res.rc
	@echo "0 ICON \"..\\\\extra\\\\pipewalker.ico\"" >> pipewalker_win32_res.rc
	@echo "LANGUAGE 0, SUBLANG_NEUTRAL" >> pipewalker_win32_res.rc
	@echo "1 VERSIONINFO" >> pipewalker_win32_res.rc
	@echo "  FILEVERSION    `echo $(VERSION) | $(AWK) 'ORS=""; { subs = gsub(/\./,","); print $0; for (ndx=subs; ndx < 3; ndx++) print ",0"; }'`" >> pipewalker_win32_res.rc
	@echo "  PRODUCTVERSION `echo $(VERSION) | $(AWK) 'ORS=""; { subs = gsub(/\./,","); print $0; for (ndx=subs; ndx < 3; ndx++) print ",0"; }'`" >> pipewalker_win32_res.rc
	@echo "  FILEOS         VOS__WINDOWS32" >> pipewalker_win32_res.rc
	@echo "  FILEFLAGSMASK  VFT2_UNKNOWN" >> pipewalker_win32_res.rc
	@echo "  FILETYPE       VFT_UNKNOWN" >> pipewalker_win32_res.rc
	@echo "BEGIN" >> pipewalker_win32_res.rc
	@echo "  BLOCK \"StringFileInfo\"" >> pipewalker_win32_res.rc
	@echo "  BEGIN" >> pipewalker_win32_res.rc
	@echo "    BLOCK \"000004b0\"" >> pipewalker_win32_res.rc
	@echo "    BEGIN" >> pipewalker_win32_res.rc
	@echo "      VALUE \"FileDescription\", \"PipeWalker game\"" >> pipewalker_win32_res.rc
	@echo "      VALUE \"FileVersion\", \"$(VERSION)\"" >> pipewalker_win32_res.rc
	@echo "      VALUE \"InternalName\", \"PipeWalker\"" >> pipewalker_win32_res.rc
	@echo "      VALUE \"LegalCopyright\", \"Copyright (C) 2007-2012 Artem Senichev\"" >> pipewalker_win32_res.rc
	@echo "      VALUE \"OriginalFilename\", \"PipeWalker.exe\"" >> pipewalker_win32_res.rc
	@echo "      VALUE \"ProductName\", \"PipeWalker game\"" >> pipewalker_win32_res.rc
	@echo "      VALUE \"ProductVersion\", \"$(VERSION)\"" >> pipewalker_win32_res.rc
	@echo "    END" >> pipewalker_win32_res.rc
	@echo "  END" >> pipewalker_win32_res.rc
	@echo "  BLOCK \"VarFileInfo\"" >> pipewalker_win32_res.rc
	@echo "  BEGIN" >> pipewalker_win32_res.rc
	@echo "    VALUE \"Translation\", 0x0, 1200" >> pipewalker_win32_res.rc
	@echo "  END" >> pipewalker_win32_res.rc
	@echo "END" >> pipewalker_win32_res.rc

pipewalker_win32_res.o: pipewalker_win32_res.rc
	@windres $^ -o $@
