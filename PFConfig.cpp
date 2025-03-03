// PFConfig.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "pathfinder.h"
#include "PFConfig.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPFConfig dialog


CPFConfig::CPFConfig(CWnd* pParent /*=NULL*/)
	: CDialog(CPFConfig::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPFConfig)
	m_strHomeDir = _T("");
	m_nMaxHops = 0;
	m_nTimeout = 0;
	//}}AFX_DATA_INIT
}


void CPFConfig::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPFConfig)
	//DDX_Text(pDX, IDC_CFG_HOMEDIR, m_strHomeDir);
	DDX_Text(pDX, IDC_CFG_MAXHOPS, m_nMaxHops);
	DDV_MinMaxInt(pDX, m_nMaxHops, 10, 40);
	DDX_Text(pDX, IDC_CFG_TIMEOUT, m_nTimeout);
	DDV_MinMaxInt(pDX, m_nTimeout, 10, 10000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPFConfig, CDialog)
	//{{AFX_MSG_MAP(CPFConfig)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPFConfig message handlers

void CPFConfig::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}
