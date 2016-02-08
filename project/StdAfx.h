// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__6AF13066_791C_4FD4_B0ED_DA0030274715__INCLUDED_)
#define AFX_STDAFX_H__6AF13066_791C_4FD4_B0ED_DA0030274715__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers


#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#include <conio.h>
#include <math.h>
#include <cgimage.h>
#include <carchive.h>
#include <cfile.h>


#include <d3dx8.h>//
#include <mmsystem.h>//
#include <ddraw.h>
#define D3D_OVERLOADS
#include <d3d.h>


#include <streams.h>
#include <atlbase.h>
#include <atlconv.cpp>
#include <mmreg.h>
#include <commctrl.h>
#include <initguid.h>

#include <stdarg.h>
#include <stdio.h>

#include <afxadv.h>	//class CRecentFileList

#include <mmsystem.h> //multimedia timers

#include <mapi.h> //e-mail functions


#define RECTW(rc) (rc.right-rc.left)
#define RECTH(rc) (rc.bottom-rc.top)

#ifndef __INITDDSTRUCT_DEFINED
#define __INITDDSTRUCT_DEFINED
template <typename T>
__inline void INITDDSTRUCT(T& dd)
{
    ZeroMemory(&dd, sizeof(dd));
    dd.dwSize = sizeof(dd);
}
#endif

#ifndef __RELEASE_DEFINED
#define __RELEASE_DEFINED
template<typename T>
__inline void RELEASE( T* &p )
{
    if( p ) {
        p->Release();
        p = NULL;
    }
}
#endif

#ifndef CHECK_HR
    #define CHECK_HR(expr) do { if (FAILED(expr)) __leave; } while(0);
#endif




#endif // _AFX_NO_AFXCMN_SUPPORT


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__6AF13066_791C_4FD4_B0ED_DA0030274715__INCLUDED_)
