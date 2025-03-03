
// PathFinderView.cpp : implementation of the CPathFinderView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "PathFinder.h"
#endif

#include "PathFinderDoc.h"
#include "PathFinderView.h"
#include "GetDestination.h"
#include "PFConfig.h"
#include "traceroute.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAXPRIVATEIP 3

CStringList g_strMRUDestNames;
BOOL g_bCanTrace, g_bCanStopTrace, g_bCanReTrace, g_bCanSave, g_bStopTraceReqested;
extern int g_nMaxHops, g_nICMPTimeout;
extern char g_szDbPath[256];
extern char g_szAppDataPath[512];
extern const char* g_szPFRegistry;
extern struct IPNETWORK g_ipPrivateIPs[3];


// CPathFinderView

IMPLEMENT_DYNCREATE(CPathFinderView, CListView)

BEGIN_MESSAGE_MAP(CPathFinderView, CListView)
	ON_COMMAND(ID_TRACEROUTE_STARTTRACE, &CPathFinderView::OnStarttrace)
	ON_UPDATE_COMMAND_UI(ID_TRACEROUTE_STARTTRACE, &CPathFinderView::OnUpdateStarttrace)
	ON_COMMAND(ID_TRACEROUTE_STOPTRACE, &CPathFinderView::OnStoptrace)
	ON_UPDATE_COMMAND_UI(ID_TRACEROUTE_STOPTRACE, &CPathFinderView::OnUpdateStoptrace)
	ON_COMMAND(ID_TRACEROUTE_RETRACE, &CPathFinderView::OnRetrace)
	ON_UPDATE_COMMAND_UI(ID_TRACEROUTE_RETRACE, &CPathFinderView::OnUpdateRetrace)
	ON_COMMAND(ID_FILE_SAVE_AS, &CPathFinderView::OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, &CPathFinderView::OnUpdateFileSaveAs)
	ON_COMMAND(ID_FILE_CONFIG, &CPathFinderView::OnFileConfig)

	ON_MESSAGE(UM_INSERT_NORMALROW, CPathFinderView::InsertNormalRow)
	ON_MESSAGE(UM_INSERT_TOUTROW, CPathFinderView::InserTimeoutRow)
	ON_MESSAGE(UM_ADD_NAME_LOC_ISP, CPathFinderView::AddRowInfo)
	ON_MESSAGE(UM_ADD_NAME, CPathFinderView::AddNodeName)
END_MESSAGE_MAP()

// CPathFinderView construction/destruction

CPathFinderView::CPathFinderView() noexcept
{
	// TODO: add construction code here

}

CPathFinderView::~CPathFinderView()
{
}

BOOL CPathFinderView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CListView::PreCreateWindow(cs);
}

void CPathFinderView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();


	g_bCanTrace = TRUE;
	g_bCanStopTrace = FALSE;
	g_bCanReTrace = FALSE;
	g_bCanSave = FALSE;


	CListCtrl& ListCtrl = GetListCtrl();

	ListCtrl.ModifyStyle(0, LVS_REPORT);

	ListView_SetExtendedListViewStyle
	(ListCtrl.m_hWnd, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);


	LV_COLUMN iColumn;

	iColumn.mask = LVCF_TEXT | LVCF_WIDTH;
	iColumn.fmt = 0;
	iColumn.cchTextMax = 0;
	iColumn.iSubItem = 0;

	iColumn.cx = 35;
	iColumn.pszText = "Hop";
	ListCtrl.InsertColumn(0, &iColumn);

	iColumn.cx = 100;
	iColumn.pszText = "IP Address";
	ListCtrl.InsertColumn(1, &iColumn);

	iColumn.cx = 300;
	iColumn.pszText = "RDNS Name";
	ListCtrl.InsertColumn(2, &iColumn);

	iColumn.cx = 60;
	iColumn.pszText = "RTT";
	ListCtrl.InsertColumn(3, &iColumn);

	iColumn.cx = 200;
	iColumn.pszText = "ISP/Organization";
	ListCtrl.InsertColumn(4, &iColumn);

	iColumn.cx = 100;
	iColumn.pszText = "Country";
	ListCtrl.InsertColumn(5, &iColumn);

	iColumn.cx = 100;
	iColumn.pszText = "Area";
	ListCtrl.InsertColumn(6, &iColumn);


	// TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to GetListCtrl().
}


// CPathFinderView diagnostics

#ifdef _DEBUG
void CPathFinderView::AssertValid() const
{
	CListView::AssertValid();
}

void CPathFinderView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CPathFinderDoc* CPathFinderView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CPathFinderDoc)));
	return (CPathFinderDoc*)m_pDocument;
}
#endif //_DEBUG


// CPathFinderView message handlers

void CPathFinderView::OnStarttrace()
{
	// TODO: Add your command handler code here

	char* AddBuf, szTempBuf[256];
	int nStrLen, nSlashLoc;
	CString szTitleBuf;
	POSITION ListPos;
	DWORD IPaddress;

	CPathFinderApp* pApp;

	CGetDestination* pGetIPAddress;

	pGetIPAddress = new CGetDestination;

	pApp = (CPathFinderApp*)AfxGetApp();


#define TRINPUT pGetIPAddress->m_strDestination


	if (pGetIPAddress->DoModal() == IDOK)
	{
		TRINPUT.TrimLeft();
		TRINPUT.TrimRight();
		nStrLen = TRINPUT.GetLength();
		nSlashLoc = 0;
		if (TRINPUT.Find("http://") != -1)
		{
			nSlashLoc = 1;
			TRINPUT = TRINPUT.Right(nStrLen - 7);
		}
		if (TRINPUT.Find("https://") != -1)
		{
			nSlashLoc = 1;
			TRINPUT = TRINPUT.Right(nStrLen - 8);
		}
		if (nSlashLoc)
		{
			nSlashLoc = TRINPUT.Find("/");
			if (nSlashLoc != -1)
			{
				TRINPUT = TRINPUT.Left(nSlashLoc);
			}
		}
		nSlashLoc = TRINPUT.Find(":");
		if (nSlashLoc != -1)
			TRINPUT = TRINPUT.Left(nSlashLoc);


		nStrLen = TRINPUT.GetLength();

		AddBuf = TRINPUT.GetBuffer(nStrLen);

		// Add it to the rcently used list
		//int nTotalTraceNames, nTNStartPoint;

		ListPos = g_strMRUDestNames.Find(TRINPUT);
		if (ListPos != NULL)  // Move it to the top
		{
			g_strMRUDestNames.RemoveAt(ListPos);
		}
		else
		{
			if (g_strMRUDestNames.GetCount() == 10)
			{
				g_strMRUDestNames.RemoveTail();
			}
		}
		g_strMRUDestNames.AddHead(TRINPUT);

		szTitleBuf = "PathFinder - Tracing ";
		szTitleBuf += AddBuf;

		CListCtrl& ListCtrl = GetListCtrl();
		ListCtrl.DeleteAllItems();

		pApp->m_pMainWnd->SetWindowText("PathFinder");

		nStrLen = StartTracing(AddBuf, m_hWnd, &IPaddress);

		if (nStrLen > 0)
		{
			if (nStrLen == 1)
			{
				szTitleBuf += "(";
				NTOA(IPaddress, szTempBuf);
				szTitleBuf += szTempBuf;
				szTitleBuf += ")";
			}

			pApp->m_pMainWnd->SetWindowText(szTitleBuf);

		}

		TRINPUT.ReleaseBuffer();
	}
	delete pGetIPAddress;



}

void CPathFinderView::OnUpdateStarttrace(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(g_bCanTrace);
}

void CPathFinderView::OnStoptrace()
{
	// TODO: Add your command handler code here
	g_bStopTraceReqested = TRUE;
	g_bCanStopTrace = FALSE;
}

void CPathFinderView::OnUpdateStoptrace(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(g_bCanStopTrace);
}

void CPathFinderView::OnRetrace()
{
	// TODO: Add your command handler code here
	int nResult;
	CString strTraceItem, strTitle;
	char* pszAddBuf, szTempBuf[256];
	IPAddr ipDest;
	CPathFinderApp* pApp;
	// TODO: Add your command handler code here

	strTraceItem = g_strMRUDestNames.GetHead();
	pszAddBuf = strTraceItem.GetBuffer(strTraceItem.GetLength());
	pApp = (CPathFinderApp*)AfxGetApp();

	strTitle = "PathFinder - Tracing ";
	strTitle += strTraceItem;

	CListCtrl& ListCtrl = GetListCtrl();
	ListCtrl.DeleteAllItems();

	pApp->m_pMainWnd->SetWindowText("PathFinder");

	nResult = StartTracing(pszAddBuf, m_hWnd, &ipDest);

	strTraceItem.ReleaseBuffer();

	if (nResult > 0)
	{
		if (nResult == 1)
		{
			strTitle += "(";
			NTOA(ipDest, szTempBuf);
			strTitle += szTempBuf;
			strTitle += ")";
		}

		pApp->m_pMainWnd->SetWindowText(strTitle);

	}
}

void CPathFinderView::OnUpdateRetrace(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(g_bCanReTrace);
}

/*
void CPathFinderView::OnFileSaveAs()
{
	// TODO: Add your command handler code here
}
*/

void CPathFinderView::OnUpdateFileSaveAs(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(g_bCanSave);
}


void CPathFinderView::OnFileConfig()
{
	// TODO: Add your command handler code here
	HKEY hRegKey;
	CPFConfig* pConfigDlg;
	char* pTempBuf;
	int nBufLen;



	pConfigDlg = new CPFConfig;


	pConfigDlg->m_strHomeDir = g_szDbPath;
	pConfigDlg->m_nMaxHops = g_nMaxHops;
	pConfigDlg->m_nTimeout = g_nICMPTimeout;


	if (pConfigDlg->DoModal() == IDOK)
	{
		nBufLen = pConfigDlg->m_strHomeDir.GetLength() + 1;
		pTempBuf = pConfigDlg->m_strHomeDir.GetBuffer(nBufLen);


		if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, g_szPFRegistry, 0, KEY_WRITE, &hRegKey))
		{
			if (pConfigDlg->m_strHomeDir.Compare(g_szDbPath) != 0)
			{
				RegSetValueEx(hRegKey, "DBPath", 0, REG_SZ, (BYTE*)pTempBuf, nBufLen);
				AfxMessageBox("Please restart the PathFinder");
			}
			if (pConfigDlg->m_nMaxHops != g_nMaxHops)
				RegSetValueEx(hRegKey, "MaxHops", 0, REG_DWORD, (BYTE*)&pConfigDlg->m_nMaxHops, 4);
			if (pConfigDlg->m_nTimeout != g_nICMPTimeout)
				RegSetValueEx(hRegKey, "ICMPTimeOut", 0, REG_DWORD, (BYTE*)&pConfigDlg->m_nTimeout, 4);

			RegCloseKey(hRegKey);
		}
		else AfxMessageBox("Sorry, Cannot save the changes");

		lstrcpy(g_szDbPath, pTempBuf);

		pConfigDlg->m_strHomeDir.ReleaseBuffer();

		g_nMaxHops = pConfigDlg->m_nMaxHops;
		g_nICMPTimeout = pConfigDlg->m_nTimeout;
	}

	delete pConfigDlg;

}

LRESULT CPathFinderView::InsertNormalRow(WPARAM wParam, LPARAM lParam)
{
	struct PathData* pRowData;
	char szBuf[256];
	CString strTemp;
	BOOL bPrivateIP;
	int i;


	pRowData = (struct PathData*)wParam;


	CListCtrl& ListCtrl = GetListCtrl();

	if (pRowData->nRTT < 0) strTemp = "-";
	else strTemp.Format("%d", pRowData->nIndex);

	ListCtrl.InsertItem(pRowData->nIndex, strTemp);

	NTOA(pRowData->Address, szBuf);
	ListCtrl.SetItemText(pRowData->nIndex, 1, szBuf);

	if (pRowData->nRTT < 0)
	{
		ListCtrl.SetItemText(pRowData->nIndex, 3, "Timeout");
	}
	else if (pRowData->nCode == IP_DEST_HOST_UNREACHABLE)
	{
		ListCtrl.SetItemText(pRowData->nIndex, 3, "Unreachable");
	}
	else
	{
		strTemp.Format("%d", pRowData->nRTT);
		ListCtrl.SetItemText(pRowData->nIndex, 3, strTemp);
	}


	bPrivateIP = FALSE;
	for (i = 0; i < MAXPRIVATEIP; i++)
	{
		if (IPCompare(g_ipPrivateIPs[i].ipNetwork, g_ipPrivateIPs[i].nPrefix, pRowData->Address) == 0)
		{
			bPrivateIP = TRUE;
			break;
		}
	}

	if (bPrivateIP)
	{
		ListCtrl.SetItemText(pRowData->nIndex, 4, "Private IP");
	}


	HeapFree(GetProcessHeap(), 0, pRowData);

	return 10;
}

LRESULT CPathFinderView::InserTimeoutRow(WPARAM Hop, LPARAM lParam)
{
	CString strTemp;

	strTemp.Format("%d", Hop);

	CListCtrl& ListCtrl = GetListCtrl();

	ListCtrl.InsertItem(Hop, strTemp);
	ListCtrl.SetItemText(Hop, 1, "Time out");

	return 0;
}

LRESULT CPathFinderView::AddRowInfo(WPARAM wParam, LPARAM lParam)
{
	struct NODEINFO* pNodeInfo;


	pNodeInfo = (struct NODEINFO*)wParam;

	CListCtrl& ListCtrl = GetListCtrl();

	ListCtrl.SetItemText(pNodeInfo->Index, 2, pNodeInfo->szNodeName);
	ListCtrl.SetItemText(pNodeInfo->Index, 5, pNodeInfo->szCountryName);
	ListCtrl.SetItemText(pNodeInfo->Index, 6, pNodeInfo->szLocation);
	ListCtrl.SetItemText(pNodeInfo->Index, 4, pNodeInfo->szISPName);

	HeapFree(GetProcessHeap(), 0, pNodeInfo);

	return 0;
}

LRESULT CPathFinderView::AddNodeName(WPARAM wParam, LPARAM lParam)
{
	char* pszNodeName;

	pszNodeName = (char*)wParam;

	CListCtrl& ListCtrl = GetListCtrl();
	ListCtrl.SetItemText(lParam, 2, pszNodeName);

	HeapFree(GetProcessHeap(), 0, pszNodeName);

	return 0;
}
