# Microsoft Developer Studio Project File - Name="gimage" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=gimage - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gimage.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gimage.mak" CFG="gimage - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gimage - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "gimage - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gimage - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x415 /d "NDEBUG"
# ADD RSC /l 0x415 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Lib\cgimage.lib"

!ELSEIF  "$(CFG)" == "gimage - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x415 /d "_DEBUG"
# ADD RSC /l 0x415 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"Lib\cgimaged.lib"

!ENDIF 

# Begin Target

# Name "gimage - Win32 Release"
# Name "gimage - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "zlib source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\zlib\adler32.c
# End Source File
# Begin Source File

SOURCE=..\zlib\compress.c
# End Source File
# Begin Source File

SOURCE=..\zlib\crc32.c
# End Source File
# Begin Source File

SOURCE=..\zlib\deflate.c
# End Source File
# Begin Source File

SOURCE=..\zlib\gzio.c
# End Source File
# Begin Source File

SOURCE=..\zlib\infblock.c
# End Source File
# Begin Source File

SOURCE=..\zlib\infcodes.c
# End Source File
# Begin Source File

SOURCE=..\zlib\inffast.c
# End Source File
# Begin Source File

SOURCE=..\zlib\inflate.c
# End Source File
# Begin Source File

SOURCE=..\zlib\inftrees.c
# End Source File
# Begin Source File

SOURCE=..\zlib\infutil.c
# End Source File
# Begin Source File

SOURCE=..\zlib\minigzip.c
# End Source File
# Begin Source File

SOURCE=..\zlib\trees.c
# End Source File
# Begin Source File

SOURCE=..\zlib\uncompr.c
# End Source File
# Begin Source File

SOURCE=..\zlib\zutil.c
# End Source File
# End Group
# Begin Group "png source"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\png\png.c
# End Source File
# Begin Source File

SOURCE=..\png\pngerror.c
# End Source File
# Begin Source File

SOURCE=..\png\pnggccrd.c
# End Source File
# Begin Source File

SOURCE=..\png\pngget.c
# End Source File
# Begin Source File

SOURCE=..\png\pngmem.c
# End Source File
# Begin Source File

SOURCE=..\png\pngpread.c
# End Source File
# Begin Source File

SOURCE=..\png\pngread.c
# End Source File
# Begin Source File

SOURCE=..\png\pngrio.c
# End Source File
# Begin Source File

SOURCE=..\png\pngrtran.c
# End Source File
# Begin Source File

SOURCE=..\png\pngrutil.c
# End Source File
# Begin Source File

SOURCE=..\png\pngset.c
# End Source File
# Begin Source File

SOURCE=..\png\pngtrans.c
# End Source File
# Begin Source File

SOURCE=..\png\pngvcrd.c
# End Source File
# Begin Source File

SOURCE=..\png\pngwio.c
# End Source File
# Begin Source File

SOURCE=..\png\pngwrite.c
# End Source File
# Begin Source File

SOURCE=..\png\pngwtran.c
# End Source File
# Begin Source File

SOURCE=..\png\pngwutil.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\cgimage.cpp
# End Source File
# Begin Source File

SOURCE=.\pngimage.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "zlib header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\zlib\deflate.h
# End Source File
# Begin Source File

SOURCE=..\zlib\infblock.h
# End Source File
# Begin Source File

SOURCE=..\zlib\infcodes.h
# End Source File
# Begin Source File

SOURCE=..\zlib\inffast.h
# End Source File
# Begin Source File

SOURCE=..\zlib\inffixed.h
# End Source File
# Begin Source File

SOURCE=..\zlib\inftrees.h
# End Source File
# Begin Source File

SOURCE=..\zlib\infutil.h
# End Source File
# Begin Source File

SOURCE=..\zlib\trees.h
# End Source File
# Begin Source File

SOURCE=..\zlib\zconf.h
# End Source File
# Begin Source File

SOURCE=..\zlib\zlib.h
# End Source File
# Begin Source File

SOURCE=..\zlib\zutil.h
# End Source File
# End Group
# Begin Group "png header"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\png\png.h
# End Source File
# Begin Source File

SOURCE=..\png\pngasmrd.h
# End Source File
# Begin Source File

SOURCE=..\png\pngconf.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\cgimage.h
# End Source File
# Begin Source File

SOURCE=.\pngimage.h
# End Source File
# End Group
# End Target
# End Project
