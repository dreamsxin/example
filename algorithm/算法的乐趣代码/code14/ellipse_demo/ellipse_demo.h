
// ellipse_demo.h : main header file for the ellipse_demo application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// Cellipse_demoApp:
// See circle_demo.cpp for the implementation of this class
//

class Cellipse_demoApp : public CWinAppEx
{
public:
	Cellipse_demoApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

public:
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern Cellipse_demoApp theApp;
