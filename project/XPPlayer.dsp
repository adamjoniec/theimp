# Microsoft Developer Studio Project File - Name="XPPlayer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=XPPlayer - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "XPPlayer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "XPPlayer.mak" CFG="XPPlayer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "XPPlayer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "XPPlayer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "XPPlayer - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D WINVER=0x501 /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x415 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x415 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 cgimage.lib carchive.lib strmbase.lib strmiids.lib quartz.lib ddraw.lib dxguid.lib winmm.lib d3dx8.lib d3d8.lib msimg32.lib /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib libcmt.lib" /out:"Release\The Imp.exe"

!ELSEIF  "$(CFG)" == "XPPlayer - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /Gi /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "DEBUG" /D WINVER=0x0501 /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x415 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x415 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 cgimaged.lib carchived.lib strmbasd.lib strmiids.lib quartz.lib ddraw.lib dxguid.lib winmm.lib d3dx8.lib d3d8.lib msimg32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug\The Imp.exe" /pdbtype:sept
# SUBTRACT LINK32 /profile /map /nodefaultlib

!ENDIF 

# Begin Target

# Name "XPPlayer - Win32 Release"
# Name "XPPlayer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Accelerators.cpp
# End Source File
# Begin Source File

SOURCE=.\Config.cpp
# End Source File
# Begin Source File

SOURCE=.\FilterProp.cpp
# End Source File
# Begin Source File

SOURCE=.\FullscreenWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Language.cpp
# End Source File
# Begin Source File

SOURCE=.\MainWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\MListBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Playlist.cpp
# End Source File
# Begin Source File

SOURCE=.\Skin.cpp
# End Source File
# Begin Source File

SOURCE=.\SplashScreen.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\Subtitles.cpp
# End Source File
# Begin Source File

SOURCE=.\SubtitlesFs.cpp
# End Source File
# Begin Source File

SOURCE=.\TimeOut.cpp
# End Source File
# Begin Source File

SOURCE=.\Tooltip.cpp
# End Source File
# Begin Source File

SOURCE=.\vcdplyer.cpp
# End Source File
# Begin Source File

SOURCE=.\vcdplyerap.cpp
# End Source File
# Begin Source File

SOURCE=.\XMLParser.cpp
# End Source File
# Begin Source File

SOURCE=.\XPPlayer.cpp
# End Source File
# End Group
# Begin Group "Skin"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Debug\Skins\bestplayer\player.xml
# End Source File
# Begin Source File

SOURCE=.\Debug\Skins\divxplayer\player.xml
# End Source File
# End Group
# Begin Group "Options"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Options.cpp
# End Source File
# Begin Source File

SOURCE=.\Options.h
# End Source File
# Begin Source File

SOURCE=.\OptionsAbout.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsAbout.h
# End Source File
# Begin Source File

SOURCE=.\OptionsFiletypes.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsFiletypes.h
# End Source File
# Begin Source File

SOURCE=.\OptionsOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsOptions.h
# End Source File
# Begin Source File

SOURCE=.\OptionsPlay.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsPlay.h
# End Source File
# Begin Source File

SOURCE=.\OptionsSetup.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsSetup.h
# End Source File
# Begin Source File

SOURCE=.\OptionsSkinBrowser.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsSkinBrowser.h
# End Source File
# Begin Source File

SOURCE=.\OptionsStartup.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsStartup.h
# End Source File
# Begin Source File

SOURCE=.\OptionsSubtitles.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsSubtitles.h
# End Source File
# Begin Source File

SOURCE=.\OptionsSubtitlesGen.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsSubtitlesGen.h
# End Source File
# Begin Source File

SOURCE=.\OptionsSubtitlesTime.cpp
# End Source File
# Begin Source File

SOURCE=.\OptionsSubtitlesTime.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\Imp.ico
# End Source File
# Begin Source File

SOURCE=.\res\Movie01.ICO
# End Source File
# Begin Source File

SOURCE=.\res\Movie02.ico
# End Source File
# Begin Source File

SOURCE=.\res\Movie03.ico
# End Source File
# Begin Source File

SOURCE=.\res\Movie04.ico
# End Source File
# Begin Source File

SOURCE=.\res\Movie05.ico
# End Source File
# Begin Source File

SOURCE=.\res\Pattern.bmp
# End Source File
# Begin Source File

SOURCE=.\res\XPPlayer.ico
# End Source File
# Begin Source File

SOURCE=.\XPPlayer.rc
# End Source File
# Begin Source File

SOURCE=.\res\XPPlayer.rc2
# End Source File
# End Group
# Begin Group "PNG"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\res\About - Mail.png"
# End Source File
# Begin Source File

SOURCE=".\res\About - Register It.png"
# End Source File
# Begin Source File

SOURCE=".\res\About - Scroller.png"
# End Source File
# Begin Source File

SOURCE=".\res\About - www.png"
# End Source File
# Begin Source File

SOURCE=.\res\About.png
# End Source File
# Begin Source File

SOURCE=.\res\Author.png
# End Source File
# Begin Source File

SOURCE=.\res\Setup.png
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Accelerators.h
# End Source File
# Begin Source File

SOURCE=.\Config.h
# End Source File
# Begin Source File

SOURCE=.\FilterProp.h
# End Source File
# Begin Source File

SOURCE=.\FullscreenWnd.h
# End Source File
# Begin Source File

SOURCE=.\Language.h
# End Source File
# Begin Source File

SOURCE=.\MainWnd.h
# End Source File
# Begin Source File

SOURCE=.\MListBox.h
# End Source File
# Begin Source File

SOURCE=.\Playlist.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Skin.h
# End Source File
# Begin Source File

SOURCE=.\SkinDefinition.h
# End Source File
# Begin Source File

SOURCE=.\SplashScreen.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\Subtitles.h
# End Source File
# Begin Source File

SOURCE=.\SubtitlesFs.h
# End Source File
# Begin Source File

SOURCE=.\TimeOut.h
# End Source File
# Begin Source File

SOURCE=.\Tooltip.h
# End Source File
# Begin Source File

SOURCE=.\vcdplyer.h
# End Source File
# Begin Source File

SOURCE=.\XMLParser.h
# End Source File
# Begin Source File

SOURCE=.\XPPlayer.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\bestplayer.zip
# End Source File
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# Begin Source File

SOURCE=.\skin_make.txt
# End Source File
# End Target
# End Project
