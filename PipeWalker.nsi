; PipeWalker game (http://pipewalker.sourceforge.net)
; MS Windows NSIS Installer script
;
; Use
;   makensis.exe /DVERSION=[version] PipeWalker.nsi
;
;----------------------------------------------------

;--------------------------------
;Include Modern UI
  !include "MUI2.nsh"

;--------------------------------
;General
  Name       "PipeWalker ${VERSION}"
  OutFile    "pipewalker-${VERSION}-win32-install.exe"
  InstallDir $PROGRAMFILES\PipeWalker
  !define MUI_STARTMENUPAGE_DEFAULTFOLDER PipeWalker
  RequestExecutionLevel admin

;--------------------------------
;Version Information
  VIProductVersion ${VERSION}.0
  VIAddVersionKey "ProductName" "PipeWalker"
  VIAddVersionKey "FileDescription" "PipeWalker game installer"
  VIAddVersionKey "LegalCopyright" "Copyright (C) 2007-2012 Artem Senichev"
  VIAddVersionKey "FileVersion" ${VERSION}
  VIAddVersionKey "ProductVersion" ${VERSION}

;--------------------------------
;Variables
  Var StartMenuFolder

;--------------------------------
;Interface Settings
  !define MUI_ABORTWARNING
  !define MUI_ICON extra\pipewalker.ico

;--------------------------------
;Pages
  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Languages
  !insertmacro MUI_LANGUAGE "English" ;first language is the default language
  !insertmacro MUI_LANGUAGE "Russian"
  !insertmacro MUI_LANGUAGE "Spanish"
  !insertmacro MUI_LANGUAGE "German"
  !insertmacro MUI_LANGUAGE "Italian"
  !insertmacro MUI_LANGUAGE "French"

;--------------------------------
;Installer Sections
Section "Install"
  SetOutPath "$INSTDIR"
  File /oname=PipeWalker.exe src\pipewalker.exe
  File SDL.dll
  File ChangeLog
  File /oname=ReadMe.txt README
  SetOutPath "$INSTDIR\data"
  File data\clatz.wav
  File data\complete.wav
  File data\Hellsfire.png
  File data\Linux.png
  File data\Network.png
  File data\Plumbing.png
  File data\Simple.png

  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ;Create shortcuts
  SetOutPath "$INSTDIR"
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
  CreateShortCut  "$SMPROGRAMS\$StartMenuFolder\PipeWalker.lnk" "$INSTDIR\PipeWalker.exe"
  CreateShortCut  "$SMPROGRAMS\$StartMenuFolder\Read me.lnk" "$INSTDIR\ReadMe.txt"
  CreateShortCut  "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;--------------------------------
;Uninstaller Section
Section "Uninstall"
  RMDir /r "$INSTDIR"
  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
  Delete "$SMPROGRAMS\$StartMenuFolder\Read me.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\PipeWalker.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"
SectionEnd
