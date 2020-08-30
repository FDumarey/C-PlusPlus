
// WinHook.h : main header file for the WinHook application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CWinHookApp:
// See WinHook.cpp for the implementation of this class
//

class CWinHookApp : public CWinApp
{
public:
	CWinHookApp();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	UINT  m_nAppLook;
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CWinHookApp theApp;
