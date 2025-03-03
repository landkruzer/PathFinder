// GetDestination.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
//#include "stdafx.h"
#include "PathFinder.h"
#include "GetDestination.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetDestination dialog
extern CStringList g_strMRUDestNames;


CGetDestination::CGetDestination(CWnd* pParent /*=NULL*/)
	: CDialog(CGetDestination::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetDestination)
	m_strDestination = _T("");
	//}}AFX_DATA_INIT
}


void CGetDestination::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetDestination)
	DDX_CBString(pDX, IDC_COMBO1, m_strDestination);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetDestination, CDialog)
	//{{AFX_MSG_MAP(CGetDestination)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetDestination message handlers

BOOL CGetDestination::OnInitDialog() 
{
POSITION ListPos;

	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	CComboBox *p = (CComboBox *) GetDlgItem(IDC_COMBO1);

	for(ListPos = g_strMRUDestNames.GetHeadPosition(); ListPos != NULL;)
	{		
		p->AddString(g_strMRUDestNames.GetAt(ListPos));
		g_strMRUDestNames.GetNext(ListPos);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
