# Microsoft Developer Studio Project File - Name="PipeWalker" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PipeWalker - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "PipeWalker.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "PipeWalker.mak" CFG="PipeWalker - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "PipeWalker - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "PipeWalker - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PipeWalker - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "PWTARGET_WINNT" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "NDEBUG"
# ADD RSC /l 0x419 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "PipeWalker - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "PWTARGET_WINNT" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x419 /d "_DEBUG"
# ADD RSC /l 0x419 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "PipeWalker - Win32 Release"
# Name "PipeWalker - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=src\cell.cpp
# End Source File
# Begin Source File

SOURCE=src\game.cpp
# End Source File
# Begin Source File

SOURCE=src\main.cpp
# End Source File
# Begin Source File

SOURCE=src\mswindows.cpp
# End Source File
# Begin Source File

SOURCE=src\opengl.cpp
# End Source File
# Begin Source File

SOURCE=src\texload.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=src\base.h
# End Source File
# Begin Source File

SOURCE=src\cell.h
# End Source File
# Begin Source File

SOURCE=src\common.h
# End Source File
# Begin Source File

SOURCE=src\game.h
# End Source File
# Begin Source File

SOURCE=src\mswindows.h
# End Source File
# Begin Source File

SOURCE=src\opengl.h
# End Source File
# Begin Source File

SOURCE=src\texload.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Group "Textures"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\textures\bkgr.tga
# End Source File
# Begin Source File

SOURCE=.\textures\btn_new.tga
# End Source File
# Begin Source File

SOURCE=.\textures\btn_reset.tga
# End Source File
# Begin Source File

SOURCE=.\textures\cellbkg.tga
# End Source File
# Begin Source File

SOURCE=.\textures\congr.tga
# End Source File
# Begin Source File

SOURCE=.\textures\display_act.tga
# End Source File
# Begin Source File

SOURCE=.\textures\display_psv.tga
# End Source File
# Begin Source File

SOURCE=.\textures\recv.tga
# End Source File
# Begin Source File

SOURCE=.\textures\sender.tga
# End Source File
# Begin Source File

SOURCE=.\textures\title.tga
# End Source File
# Begin Source File

SOURCE=.\textures\tube_act.tga
# End Source File
# Begin Source File

SOURCE=.\textures\tube_psv.tga
# End Source File
# Begin Source File

SOURCE=.\textures\weblink.tga
# End Source File
# End Group
# Begin Source File

SOURCE=.\PipeWalker.ico
# End Source File
# Begin Source File

SOURCE=src\PipeWalker.rc
# End Source File
# Begin Source File

SOURCE=src\PipeWalkerRes.h
# End Source File
# End Group
# End Target
# End Project
