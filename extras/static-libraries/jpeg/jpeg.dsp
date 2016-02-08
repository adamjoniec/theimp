# Microsoft Developer Studio Project File - Name="jpeg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=jpeg - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "jpeg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jpeg.mak" CFG="jpeg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jpeg - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "jpeg - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe

!IF  "$(CFG)" == "jpeg - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "jpeg - Win32 Release"
# Name "jpeg - Win32 Debug"
# Begin Group "sources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\files\Jbsmooth.c
# End Source File
# Begin Source File

SOURCE=.\files\Jcarith.c
# End Source File
# Begin Source File

SOURCE=.\files\Jccolor.c
# End Source File
# Begin Source File

SOURCE=.\files\Jcdeflts.c
# End Source File
# Begin Source File

SOURCE=.\files\Jcexpand.c
# End Source File
# Begin Source File

SOURCE=.\files\Jchuff.c
# End Source File
# Begin Source File

SOURCE=.\files\Jcmaster.c
# End Source File
# Begin Source File

SOURCE=.\files\Jcmcu.c
# End Source File
# Begin Source File

SOURCE=.\files\Jcpipe.c
# End Source File
# Begin Source File

SOURCE=.\files\Jcsample.c
# End Source File
# Begin Source File

SOURCE=.\files\Jdarith.c
# End Source File
# Begin Source File

SOURCE=.\files\Jdcolor.c
# End Source File
# Begin Source File

SOURCE=.\files\Jddeflts.c
# End Source File
# Begin Source File

SOURCE=.\files\Jdhuff.c
# End Source File
# Begin Source File

SOURCE=.\files\Jdmaster.c
# End Source File
# Begin Source File

SOURCE=.\files\Jdmcu.c
# End Source File
# Begin Source File

SOURCE=.\files\Jdpipe.c
# End Source File
# Begin Source File

SOURCE=.\files\Jdsample.c
# End Source File
# Begin Source File

SOURCE=.\files\Jerror.c
# End Source File
# Begin Source File

SOURCE=.\files\Jfwddct.c
# End Source File
# Begin Source File

SOURCE=.\files\Jmemmgr.c
# End Source File
# Begin Source File

SOURCE=.\files\Jmemsys.c
# End Source File
# Begin Source File

SOURCE=.\files\Jquant1.c
# End Source File
# Begin Source File

SOURCE=.\files\Jquant2.c
# End Source File
# Begin Source File

SOURCE=.\files\Jrdgif.c
# End Source File
# Begin Source File

SOURCE=.\files\Jrdjfif.c
# End Source File
# Begin Source File

SOURCE=.\files\Jrdppm.c
# End Source File
# Begin Source File

SOURCE=.\files\Jrdrle.c
# End Source File
# Begin Source File

SOURCE=.\files\Jrdtarga.c
# End Source File
# Begin Source File

SOURCE=.\files\Jrevdct.c
# End Source File
# Begin Source File

SOURCE=.\files\jrwmem.c
# End Source File
# Begin Source File

SOURCE=.\files\Jutils.c
# End Source File
# Begin Source File

SOURCE=.\files\Jwrgif.c
# End Source File
# Begin Source File

SOURCE=.\files\Jwrjfif.c
# End Source File
# Begin Source File

SOURCE=.\files\Jwrppm.c
# End Source File
# Begin Source File

SOURCE=.\files\Jwrrle.c
# End Source File
# Begin Source File

SOURCE=.\files\Jwrtarga.c
# End Source File
# End Group
# Begin Group "headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\files\Jconfig.h
# End Source File
# Begin Source File

SOURCE=.\files\Jinclude.h
# End Source File
# Begin Source File

SOURCE=.\files\Jmemsys.h
# End Source File
# Begin Source File

SOURCE=.\files\Jpegdata.h
# End Source File
# End Group
# End Target
# End Project
