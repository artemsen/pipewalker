:: PipeWalker game (http://pipewalker.sourceforge.net)
:: Make dist package for MS Windows

set VERSION=0.9.2
set ZIPTOOL="C:\Program Files\7-Zip\7z.exe" a -tzip -r
set NSITOOL="C:\soft_dev\nsis\makensis.exe"

:: Make portable package
if exist PipeWalker rmdir /S /Q PipeWalker
mkdir PipeWalker
echo # This is the user settings file of the PipeWalker game > PipeWalker\.pipewalker
copy ChangeLog PipeWalker\ChangeLog
copy README PipeWalker\README
copy PipeWalker.exe PipeWalker\PipeWalker.exe
copy SDL.dll PipeWalker\SDL.dll
mkdir PipeWalker\data
copy data\*.wav PipeWalker\data
copy data\*.png PipeWalker\data
call %ZIPTOOL% PipeWalker-%VERSION%-win32bin.zip PipeWalker\*
rmdir /S /Q PipeWalker

:: Make installer
call %NSITOOL% /DVERSION=%VERSION% PipeWalker.nsi
