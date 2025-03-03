
// PathFinder.h : main header file for the PathFinder application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CPathFinderApp:
// See PathFinder.cpp for the implementation of this class
//

class CPathFinderApp : public CWinApp
{
public:
	CPathFinderApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CPathFinderApp theApp;
