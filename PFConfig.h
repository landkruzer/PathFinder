#if !defined(AFX_PFCONFIG_H__B5EC63E0_6ABB_4F19_B2A1_A1F65C828432__INCLUDED_)
#define AFX_PFCONFIG_H__B5EC63E0_6ABB_4F19_B2A1_A1F65C828432__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PFConfig.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPFConfig dialog

class CPFConfig : public CDialog
{
// Construction
public:
	CPFConfig(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPFConfig)
	enum { IDD = IDD_DIALOG_CFG };
	CString	m_strHomeDir;
	int		m_nMaxHops;
	int		m_nTimeout;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPFConfig)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPFConfig)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PFCONFIG_H__B5EC63E0_6ABB_4F19_B2A1_A1F65C828432__INCLUDED_)
