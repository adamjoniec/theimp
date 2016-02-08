; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CXPPlayerApp
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "XPPlayer.h"

ClassCount=23
Class1=CXPPlayerApp
Class2=CXPPlayerDlg

ResourceCount=23
Resource1=IDD_XPPLAYER_DIALOG (English (U.S.))
Class3=CMainWnd
Class4=CFullscreenWnd
Class5=CTooltip
Resource2=IDD_OLE_PROPPAGE_LARGE (English (U.S.))
Resource3=IDD_PROPPAGE_MEDIUM (English (U.S.))
Class6=CFilterProp
Resource4=IDD_DIALOG_FILTERSPROP
Class7=CPlaylist
Resource5=IDD_ABOUT (English (U.S.))
Class8=CMListBox
Class9=CSubtitles
Class10=CSubtitlesFs
Resource6=IDD_PP_SUBTITLES (English (U.S.))
Resource7=IDD_PP_FILETYPES (English (U.S.))
Resource8=IDR_MENU1
Resource9=IDD_PP_SKINBROWSER (English (U.S.))
Class11=COptions
Resource10=IDD_PP_PLAY (English (U.S.))
Resource11=IDR_MENU_CALLBACK
Resource12=IDD_DIALOG_OPTIONS
Resource13=IDD_PP_SUBTITLES_TIME (English (U.S.))
Resource14=IDD_PP_OPTIONS (English (U.S.))
Resource15=IDR_MENU_PLS
Resource16=IDD_PP_ABOUT (English (U.S.))
Resource17=IDD_DIALOG_PLAYLIST
Resource18=IDD_PP_STARTUP (English (U.S.))
Class12=COptionsSkinBrowser
Class13=COptionsSubtitles
Resource19=IDD_DIALOG_TIMEOUT
Resource20=IDD_PROPPAGE_LARGE (English (U.S.))
Class14=COptionsSubtitlesGen
Class15=COptionsAbout
Class16=COptionsSubtitlesTime
Class17=COptionsStartup
Class18=COptionsSetup
Class19=COptionsFiletypes
Class20=COptionsOptions
Class21=COptionsPlay
Resource21=IDR_MENU
Class22=CSplashScreen
Resource22=IDD_PP_SETUP (English (U.S.))
Class23=CTimeOut
Resource23=IDD_PP_SUBTITLES_GENERAL (English (U.S.))

[CLS:CXPPlayerApp]
Type=0
HeaderFile=XPPlayer.h
ImplementationFile=XPPlayer.cpp
Filter=N
BaseClass=CWinApp
VirtualFilter=AC
LastObject=CXPPlayerApp

[CLS:CXPPlayerDlg]
Type=0
HeaderFile=XPPlayerDlg.h
ImplementationFile=XPPlayerDlg.cpp
Filter=D
LastObject=IDCANCEL



[DLG:IDD_XPPLAYER_DIALOG (English (U.S.))]
Type=1
Class=COptionsStartup
ControlCount=3
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352

[CLS:CMainWnd]
Type=0
HeaderFile=MainWnd.h
ImplementationFile=MainWnd.cpp
BaseClass=CWnd
Filter=W
LastObject=CMainWnd
VirtualFilter=WC

[CLS:CFullscreenWnd]
Type=0
HeaderFile=FullscreenWnd.h
ImplementationFile=FullscreenWnd.cpp
BaseClass=CWnd
Filter=W
VirtualFilter=WC
LastObject=CFullscreenWnd

[CLS:CTooltip]
Type=0
HeaderFile=Tooltip.h
ImplementationFile=Tooltip.cpp
BaseClass=CWnd
Filter=W
VirtualFilter=WC
LastObject=IDM_NEXTMOVIE

[MNU:IDR_MENU]
Type=1
Class=?
Command1=IDM_PLAYPAUSE
Command2=IDM_STOP
Command3=IDM_REWINDBACK
Command4=IDM_REWINDFORWARD
Command5=IDM_PREVMOVIE
Command6=IDM_NEXTMOVIE
Command7=IDM_PLAYLIST
Command8=IDM_FULLSCREEN
Command9=IDM_PANSCAN
Command10=IDM_PS_AUTO
Command11=IDM_PS_WIDE
Command12=IDM_PS_ZOOM10
Command13=IDM_PS_ZOOM20
Command14=IDM_PS_ZOOM25
Command15=IDM_PS_ZOOM30
Command16=IDM_PS_ZOOM35
Command17=IDM_PS_ZOOM40
Command18=IDM_PS_ZOOM50
Command19=IDM_SHOWSUBTITLES
Command20=IDM_OPENMOVIE
Command21=IDM_OPENSUBTITLES
Command22=IDM_OPTIONS
Command23=IDM_FILTERSPROP
Command24=IDM_SKINBROWSER
Command25=IDM_EXIT
CommandCount=25

[DLG:IDD_DIALOG_FILTERSPROP]
Type=1
Class=CFilterProp
ControlCount=18
Control1=IDOK,button,1342242817
Control2=IDC_LIST_FILTERS,listbox,1352728835
Control3=IDC_TEXTT_TITLESFILTERS,button,1342177287
Control4=IDC_BUTTON_PROP,button,1476460544
Control5=IDCANCEL,button,1342242816
Control6=IDAPPLY,button,1476460544
Control7=IDC_TEXTT_TITLEPICTURE,button,1342177287
Control8=IDC_SLIDER_BRIGHTNESS,msctls_trackbar32,1342242817
Control9=IDC_STATIC_BRIGHTNESS,static,1342308352
Control10=IDC_TEXTT_BRIGHTNESS,static,1342308352
Control11=IDC_SLIDER_CONTRAST,msctls_trackbar32,1342242817
Control12=IDC_STATIC_CONTRAST,static,1342308352
Control13=IDC_TEXTT_CONTRAST,static,1342308352
Control14=IDC_SLIDER_SATURATION,msctls_trackbar32,1342242817
Control15=IDC_STATIC_SATURATION,static,1342308352
Control16=IDC_TEXTT_SATURATION,static,1342308352
Control17=IDC_BUTTON_DEF,button,1342242816
Control18=IDC_TEXTT_WINXPONLY,static,1342308352

[CLS:CFilterProp]
Type=0
HeaderFile=FilterProp.h
ImplementationFile=FilterProp.cpp
BaseClass=CDialog
Filter=W
LastObject=CFilterProp
VirtualFilter=dWC

[DLG:IDD_DIALOG_PLAYLIST]
Type=1
Class=CPlaylist
ControlCount=1
Control1=IDC_LIST_PLAYLIST,listbox,1344341249

[CLS:CPlaylist]
Type=0
HeaderFile=Playlist.h
ImplementationFile=Playlist.cpp
BaseClass=CDialog
Filter=W
LastObject=CPlaylist
VirtualFilter=dWC

[MNU:IDR_MENU_PLS]
Type=1
Class=CPlaylist
Command1=ID_QMPLS_OPEN
Command2=ID_QMPLS_SAVE
Command3=ID_QMPLS_CLEARPLS
Command4=ID_QMPLS_ADDFILES
Command5=ID_QMPLS_REMOVEFILE
Command6=ID_QMPLS_CLOSE
CommandCount=6

[CLS:CMListBox]
Type=0
HeaderFile=MListBox.h
ImplementationFile=MListBox.cpp
BaseClass=CListBox
Filter=C
LastObject=CMListBox
VirtualFilter=bWC

[CLS:CSubtitles]
Type=0
HeaderFile=Subtitles.h
ImplementationFile=Subtitles.cpp
BaseClass=CWnd
Filter=W
LastObject=CSubtitles
VirtualFilter=WC

[CLS:CSubtitlesFs]
Type=0
HeaderFile=SubtitlesFs.h
ImplementationFile=SubtitlesFs.cpp
BaseClass=CWnd
Filter=W
VirtualFilter=WC
LastObject=CSubtitlesFs

[DLG:IDD_PROPPAGE_MEDIUM (English (U.S.))]
Type=1
Class=COptionsSubtitlesTime
ControlCount=1
Control1=IDC_STATIC,static,1342308352

[DLG:IDD_OLE_PROPPAGE_LARGE (English (U.S.))]
Type=1
Class=COptionsSubtitles
ControlCount=1
Control1=IDC_STATIC,static,1342308352

[DLG:IDD_DIALOG_OPTIONS]
Type=1
Class=COptions
ControlCount=2
Control1=IDC_TREE_PAGE,SysTreeView32,1350644230
Control2=IDC_BUTTON_CLOSE,button,1342242816

[CLS:COptions]
Type=0
HeaderFile=Options.h
ImplementationFile=Options.cpp
BaseClass=CDialog
Filter=W
VirtualFilter=dWC
LastObject=COptions

[DLG:IDD_PP_ABOUT (English (U.S.))]
Type=1
Class=COptions
ControlCount=0

[DLG:IDD_PP_SUBTITLES (English (U.S.))]
Type=1
Class=COptionsSubtitles
ControlCount=14
Control1=IDC_TEXTT_TITLE,button,1342177287
Control2=IDC_COMBO_MODESEL,combobox,1344339971
Control3=IDC_CHECK_SHORTBKGRND,button,1342275587
Control4=IDC_TEXTT_MODESEL,static,1342308352
Control5=IDC_COMBO_OUTLINESEL,combobox,1344339971
Control6=IDC_TEXTT_OUTLINESEL,static,1342308352
Control7=IDC_CHECK_TRANSPARENTBKGRND,button,1342275587
Control8=IDC_BUTTON_CHANGEFONT,button,1342242816
Control9=IDC_BUTTON_APPLY,button,1476460545
Control10=IDC_CHECK_RGNPRECISE,button,1342275587
Control11=IDC_TEXT_BKGRNDCOL,static,1342313217
Control12=IDC_TEXT_OUTLINECOL,static,1342313217
Control13=IDC_TEXT_FONTCOL,static,1342313217
Control14=IDC_BUTTON_DEFAULT,button,1342242816

[DLG:IDD_PP_FILETYPES (English (U.S.))]
Type=1
Class=COptions
ControlCount=12
Control1=IDC_TEXTT_TITLE,button,1342177287
Control2=IDC_LIST_EXTENSIONS,listbox,1352728841
Control3=IDC_TEXTT_TITLEASSOCIATED,button,1342177287
Control4=IDC_BUTTON_APPLY,button,1476460545
Control5=IDC_BUTTON_DEFAULT,button,1342242816
Control6=IDC_BUTTON_ALL,button,1342242816
Control7=IDC_BUTTON_NONE,button,1342242816
Control8=IDC_CHECK_REGISTERONSTART,button,1342275587
Control9=IDC_TEXTT_REGISTERONSTART,static,1342374144
Control10=IDC_TEXTT_TITLEICON,button,1342177287
Control11=IDC_STATIC_ICON,static,1342177283
Control12=IDC_SLIDER_ICON,msctls_trackbar32,1342242819

[DLG:IDD_PP_SETUP (English (U.S.))]
Type=1
Class=COptions
ControlCount=6
Control1=IDC_TEXTT_TITLE,button,1342177287
Control2=IDC_TEXTT_TITLECURRLANG,button,1342177287
Control3=IDC_COMBO_CURRLANG,combobox,1344340227
Control4=IDC_BUTTON_APPLY,button,1476460545
Control5=IDC_BUTTON_DEFAULT,button,1342242816
Control6=IDC_TEXTT_TRANSLATION,static,1342308352

[DLG:IDD_PP_OPTIONS (English (U.S.))]
Type=1
Class=COptions
ControlCount=15
Control1=IDC_TEXTT_TITLE,button,1342177287
Control2=IDC_CHECK_HIDEPLSMLEFT,button,1342275587
Control3=IDC_CHECK_MAXOVERTASK,button,1342275587
Control4=IDC_CHECK_ALWAYSONTOP,button,1342275587
Control5=IDC_TEXTT_HIDEAFTER,static,1342308352
Control6=IDC_EDIT_HIDEAFTER,edit,1350639744
Control7=IDC_SPIN_HIDEAFTER,msctls_updown32,1342177462
Control8=IDC_TEXTT2_HIDEAFTER,static,1342308352
Control9=IDC_TEXTT_MAXPLSFILES,static,1342308352
Control10=IDC_EDIT_MAXPLSFILES,edit,1350631552
Control11=IDC_SPIN_MAXPLSFILES,msctls_updown32,1342177334
Control12=IDC_BUTTON_APPLY,button,1476460545
Control13=IDC_BUTTON_DEFAULT,button,1342242816
Control14=IDC_TEXTT_TITLEGENERAL,button,1342177287
Control15=IDC_TEXTT_TITLEPLAYLIST,button,1342177287

[DLG:IDD_PP_PLAY (English (U.S.))]
Type=1
Class=COptions
ControlCount=14
Control1=IDC_TEXTT_TITLE,button,1342177287
Control2=IDC_TEXTT_REWINDBY,static,1342308352
Control3=IDC_EDIT_REWINDBY,edit,1350639744
Control4=IDC_SPIN_REWINDBY,msctls_updown32,1342177334
Control5=IDC_TEXTT2_REWINDBY,static,1342308352
Control6=IDC_CHECK_FASTSEEK,button,1342275587
Control7=IDC_CHECK_AUTOREWIND,button,1342275587
Control8=IDC_CHECK_LOOP,button,1342275587
Control9=IDC_CHECK_AUTOPLAY,button,1342275587
Control10=IDC_TEXTT_FASTSEEK,static,1342308352
Control11=IDC_TEXTT_TITLESEEKING,button,1342177287
Control12=IDC_TEXTT_TITLEGENERAL,button,1342177287
Control13=IDC_BUTTON_APPLY,button,1476460545
Control14=IDC_BUTTON_DEFAULT,button,1342242816

[DLG:IDD_PP_STARTUP (English (U.S.))]
Type=1
Class=COptions
ControlCount=9
Control1=IDC_TEXTT_TITLE,button,1342177287
Control2=IDC_TEXTT_OPENWITHMOVIE,static,1342308352
Control3=IDC_COMBO_OPENWITHMOVIE,combobox,1344339971
Control4=IDC_TEXTT2_OPENWITHMOVIE,static,1342308352
Control5=IDC_TEXTT_OPENONMOVIE,static,1342308352
Control6=IDC_COMBO_OPENNOMOVIE,combobox,1344339971
Control7=IDC_TEXTT2_OPENONMOVIE,static,1342308352
Control8=IDC_BUTTON_APPLY,button,1476460545
Control9=IDC_BUTTON_DEFAULT,button,1342242816

[DLG:IDD_PP_SKINBROWSER (English (U.S.))]
Type=1
Class=COptions
ControlCount=14
Control1=IDC_TEXTT_TITLE,button,1342177287
Control2=IDC_SBLIST_SKINS,listbox,1352728835
Control3=IDC_TEXTT_TITLESKINIFNO,button,1342177287
Control4=IDC_TEXTT_NAME,static,1342308352
Control5=IDC_TEXTT_AUTHOR,static,1342308352
Control6=IDC_TEXTT_COMMENT,static,1342308352
Control7=IDC_STATIC,static,1342177287
Control8=IDC_TEXTT_VERSION,static,1342308352
Control9=IDC_TEXT_VERSION,static,1342308352
Control10=IDC_TEXT_AUTHOR,static,1342308352
Control11=IDC_TEXT_PREVNOTAVAIL,static,1342308865
Control12=IDC_TEXT_COMMENT,static,1342308352
Control13=IDC_TEXT_NAME,static,1342308352
Control14=IDC_BUTTON_SELECTSKIN,button,1476460544

[DLG:IDD_ABOUT (English (U.S.))]
Type=1
ControlCount=1
Control1=IDC_STATIC,button,1342177287
Class=COptionsSkinBrowser

[CLS:COptionsSkinBrowser]
Type=0
HeaderFile=OptionsSkinBrowser.h
ImplementationFile=OptionsSkinBrowser.cpp
BaseClass=CDialog
Filter=D
LastObject=COptionsSkinBrowser
VirtualFilter=dWC

[CLS:COptionsSubtitles]
Type=0
HeaderFile=OptionsSubtitles.h
ImplementationFile=OptionsSubtitles.cpp
BaseClass=CDialog
Filter=D
LastObject=COptionsSubtitles
VirtualFilter=dWC

[DLG:IDD_PROPPAGE_LARGE (English (U.S.))]
Type=1
Class=COptionsAbout
ControlCount=1
Control1=IDC_STATIC,static,1342308352

[DLG:IDD_PP_SUBTITLES_GENERAL (English (U.S.))]
Type=1
Class=COptionsSubtitlesGen
ControlCount=21
Control1=IDC_EDIT_TEXTROWS,edit,1350574208
Control2=IDC_SPIN_TEXTROWS,msctls_updown32,1342242998
Control3=IDC_TEXTT_TITLE,button,1342177287
Control4=IDC_EDIT_DISTANCE,edit,1350574208
Control5=IDC_SPIN_DISTANCE,msctls_updown32,1342242998
Control6=IDC_COMBO_MROWDIR,combobox,1344339971
Control7=IDD_TEXTT_TEXTROWS,static,1342308352
Control8=IDD_TEXTT_ROWDISTANCE,static,1342308352
Control9=IDD_TEXTT_MROWDIR,static,1342308352
Control10=IDC_CHECK_AUTOSIZE,button,1342275587
Control11=IDC_BUTTON_APPLY,button,1476460545
Control12=IDC_BUTTON_DEFAULT,button,1342242816
Control13=IDC_EDIT_TIMETOHIDE,edit,1350574208
Control14=IDC_SPIN_TIMETOHIDE,msctls_updown32,1342242998
Control15=IDD_TEXTT_TIMETOHIDE,static,1342308352
Control16=IDC_SCROLLBAR_POSITION,scrollbar,1342177281
Control17=IDD_TEXTT2_TIMETOHIDE,static,1342308352
Control18=IDD_TEXTT2_ROWDISTANCE,static,1342308352
Control19=IDD_TEXTT_TITLETIMEPROP,button,1342177287
Control20=IDC_TEXTT_TITLEADDITIONAL,button,1342177287
Control21=IDC_TEXTT_TITLEPOSITION,button,1342178055

[CLS:COptionsSubtitlesGen]
Type=0
HeaderFile=OptionsSubtitlesGen.h
ImplementationFile=OptionsSubtitlesGen.cpp
BaseClass=CDialog
Filter=D
LastObject=COptionsSubtitlesGen
VirtualFilter=dWC

[CLS:COptionsAbout]
Type=0
HeaderFile=OptionsAbout.h
ImplementationFile=OptionsAbout.cpp
BaseClass=CDialog
Filter=W
LastObject=COptionsAbout
VirtualFilter=dWC

[CLS:COptionsSubtitlesTime]
Type=0
HeaderFile=OptionsSubtitlesTime.h
ImplementationFile=OptionsSubtitlesTime.cpp
BaseClass=CDialog
Filter=W
VirtualFilter=dWC
LastObject=COptionsSubtitlesTime

[DLG:IDD_PP_SUBTITLES_TIME (English (U.S.))]
Type=1
Class=COptions
ControlCount=11
Control1=IDC_TEXTT_TITLE,button,1342177287
Control2=IDC_DATETIME_TIME,SysDateTimePick32,1342242857
Control3=IDC_TEXTT_TITLEOFFSET,button,1342177287
Control4=IDC_TEXTT_TIME,static,1342308352
Control5=IDC_RADIO_DELAY,button,1342373897
Control6=IDC_RADIO_SPEEDUP,button,1342373897
Control7=IDC_TEXTT2_TIME,static,1342308352
Control8=IDC_CHECK_OFFSET,button,1342275587
Control9=IDC_BUTTON_APPLY,button,1476460545
Control10=IDC_BUTTON_DEFAULT,button,1342242816
Control11=IDC_BUTTON_SAVEAS,button,1342242816

[CLS:COptionsStartup]
Type=0
HeaderFile=OptionsStartup.h
ImplementationFile=OptionsStartup.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=COptionsStartup

[CLS:COptionsSetup]
Type=0
HeaderFile=OptionsSetup.h
ImplementationFile=OptionsSetup.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=COptionsSetup

[CLS:COptionsFiletypes]
Type=0
HeaderFile=OptionsFiletypes.h
ImplementationFile=OptionsFiletypes.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=COptionsFiletypes

[CLS:COptionsOptions]
Type=0
HeaderFile=OptionsOptions.h
ImplementationFile=OptionsOptions.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=IDC_BUTTON_DEFAULT

[CLS:COptionsPlay]
Type=0
HeaderFile=OptionsPlay.h
ImplementationFile=OptionsPlay.cpp
BaseClass=CDialog
Filter=D
VirtualFilter=dWC
LastObject=COptionsPlay

[MNU:IDR_MENU_CALLBACK]
Type=1
Class=?
Command1=IDM_CALLBACK_OPENFROMPLS
Command2=IDM_CALLBACK_CLOSEDPLS
Command3=IDM_CALLBACK_SKINCHANGED
Command4=IDM_CALLBACK_FONTCHANGED
Command5=IDM_CALLBACK_ALWAYSONTOPCHANGED
Command6=IDM_CALLBACK_TRIEDLOADSUBTITLES
Command7=IDM_CALLBACK_OFFSETSUBTITLESCHANGED
Command8=IDM_CALLBACK_SUBTITLESPOSCHANGED
Command9=IDM_CALLBACK_SAVESUBTITLES
Command10=IDM_CALLBACK_LANGUAGECHANGED
Command11=IDM_CALLBACK_OPENFROMMUTEX
CommandCount=11

[MNU:IDR_MENU1]
Type=1
Class=?
Command1=IDM_CALLBACK_OPENFROMPLS
Command2=IDM_CALLBACK_CLOSEDPLS
Command3=IDM_CALLBACK_SKINCHANGED
Command4=IDM_CALLBACK_FONTCHANGED
CommandCount=4

[CLS:CSplashScreen]
Type=0
HeaderFile=SplashScreen.h
ImplementationFile=SplashScreen.cpp
BaseClass=CWnd
Filter=W
VirtualFilter=WC
LastObject=CSplashScreen

[DLG:IDD_DIALOG_TIMEOUT]
Type=1
Class=CTimeOut
ControlCount=4
Control1=IDC_CAPTION,static,1342308352
Control2=IDC_BUTTON_REGISTER,button,1342242816
Control3=IDC_BUTTON_THEIMP,button,1342242816
Control4=IDC_STATIC,static,1342308352

[CLS:CTimeOut]
Type=0
HeaderFile=TimeOut.h
ImplementationFile=TimeOut.cpp
BaseClass=CDialog
Filter=D
LastObject=CTimeOut
VirtualFilter=dWC

