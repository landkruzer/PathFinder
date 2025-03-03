#if !defined(AFX_GETDESTINATION_H__EF02CF9A_7A8D_47DB_9FFC_3A6D22856C36__INCLUDED_)
#define AFX_GETDESTINATION_H__EF02CF9A_7A8D_47DB_9FFC_3A6D22856C36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GetDestination.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGetDestination dialog

class CGetDestination : public CDialog
{
// Construction
public:
	CGetDestination(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGetDestination)
	enum { IDD = IDD_IPADDRESS };
	CString	m_strDestination;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetDestination)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGetDestination)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETDESTINATION_H__EF02CF9A_7A8D_47DB_9FFC_3A6D22856C36__INCLUDED_)
