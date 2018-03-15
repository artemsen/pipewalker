SUBDIRS = src data extra

pkgdocdatadir = $(datadir)/doc/$(PACKAGE)
pkgdocdata_DATA = README

EXTRA_DIST = $(pkgdocdata_DATA) \
	PipeWalker.sln PipeWalker.vcxproj \
	PipeWalker.nsi

if WIN32
BUILT_SOURCES = win32_make_dist.cmd
endif

win32_make_dist.cmd:
	@echo "set ZIPTOOL=\"C:\Program Files\7-Zip\7z.exe\" a -tzip -r" > win32_make_dist.cmd
	@echo "set NSITOOL=\"C:\soft_dev\nsis\makensis.exe\"" >> win32_make_dist.cmd
	@echo "if exist PipeWalker rmdir /S /Q PipeWalker" >> win32_make_dist.cmd
	@echo "mkdir PipeWalker" >> win32_make_dist.cmd
	@echo "echo # This is the user settings file of the PipeWalker game > PipeWalker\.pipewalker" >> win32_make_dist.cmd
	@echo "copy ChangeLog PipeWalker\ChangeLog" >> win32_make_dist.cmd
	@echo "copy README PipeWalker\ReadMe.txt" >> win32_make_dist.cmd
	@echo "copy src\pipewalker.exe PipeWalker\PipeWalker.exe" >> win32_make_dist.cmd
	@echo "copy SDL.dll PipeWalker\SDL.dll" >> win32_make_dist.cmd
	@echo "mkdir PipeWalker\data" >> win32_make_dist.cmd
	@echo "copy data\*.wav PipeWalker\data" >> win32_make_dist.cmd
	@echo "copy data\*.png PipeWalker\data" >> win32_make_dist.cmd
	@echo "call %ZIPTOOL% pipewalker-$(VERSION)-win32-portable.zip PipeWalker\*" >> win32_make_dist.cmd
	@echo "rmdir /S /Q PipeWalker" >> win32_make_dist.cmd
	@echo "call %NSITOOL% /DVERSION=$(VERSION) PipeWalker.nsi" >> win32_make_dist.cmd
