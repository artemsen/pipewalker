; PipeWalker game (http://pipewalker.sourceforge.net)
; Installer script
;----------------------------------------------------

;--------------------------------
;Include Modern UI
  !include "MUI2.nsh"


;--------------------------------
;General
  Name       "PipeWalker ${VERSION}"
  OutFile    "pipewalker-${VERSION}-install.exe"
  InstallDir $PROGRAMFILES\PipeWalker

  !define MUI_STARTMENUPAGE_DEFAULTFOLDER PipeWalker

  RequestExecutionLevel admin


;--------------------------------
;Version Information

  VIProductVersion ${VERSION}.0
  VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "PipeWalker"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "PipeWalker game installer"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "Copyright (C) 2007-2010 Artem A. Senichev"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" ${VERSION}
  VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" ${VERSION}


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
  !insertmacro MUI_LANGUAGE "Portuguese"
  !insertmacro MUI_LANGUAGE "Italian"
  !insertmacro MUI_LANGUAGE "French"
  !insertmacro MUI_LANGUAGE "German"
  !insertmacro MUI_LANGUAGE "Czech"


;--------------------------------
;Installer Sections

Section "Dummy Section" SecDummy

  SetOutPath "$INSTDIR"
  File PipeWalker.exe
  File ChangeLog
  File README
  SetOutPath "$INSTDIR\Data"
  File data\settings.ini
  File data\clatz.wav
  File data\complete.wav
  File data\font.tga
  File data\network1.tga
  File data\network2.tga
  File data\plumbing.tga
  File data\hellsfire.tga
  File data\newyear.tga
  File data\simple.tga
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  ;Create shortcuts
  SetOutPath "$INSTDIR"
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortCut  "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    CreateShortCut  "$SMPROGRAMS\$StartMenuFolder\PipeWalker.lnk" "$INSTDIR\PipeWalker.exe"
  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  RMDir /r "$INSTDIR"

  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\PipeWalker.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"

SectionEnd
