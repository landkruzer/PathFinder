
// PathFinder.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "PathFinder.h"
#include "MainFrm.h"

#include "PathFinderDoc.h"
#include "PathFinderView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define NIC_DB 1
#define IPORG_DB 2
#define IPCTY_DB 3
#define IPLOC_DB 4

// CPathFinderApp


void LoadHelperDLLs();
void LoadAllTables();
void GetDBfromClound(bool bWaitTillCompletion);

const char* g_szPFRegistry = "SOFTWARE\\MSOFT\\PATHFINDER";
extern CStringList g_strMRUDestNames;
extern char g_szDbPath[256];
extern char g_szAppDataPath[512];
extern int g_nMaxHops, g_nICMPTimeout;
extern int g_nNICDBVersion, g_nIP2ORGVersion, g_nIP2CNTRYVersion, g_nIP2LocVersion;
extern int g_nLogWhoIsData;


BEGIN_MESSAGE_MAP(CPathFinderApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CPathFinderApp::OnAppAbout)
	// Standard file based document commands
	//ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CPathFinderApp construction

CPathFinderApp::CPathFinderApp() noexcept
{

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("PathFinder.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CPathFinderApp object

CPathFinderApp theApp;


// CPathFinderApp initialization

BOOL CPathFinderApp::InitInstance()
{
	HANDLE hMutex;
	HKEY hRegKey;
	DWORD dwSize;
	int i, nLen;
	char szTempBuf[256], szURL[256];


	CWinApp::InitInstance();


	EnableTaskbarInteraction(FALSE);

	hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, "PATHFINDER");
	if (hMutex == NULL)
		hMutex = CreateMutex(NULL, TRUE, "PATHFINDER");
	else
	{
		AfxMessageBox("PathFinder running already");
		return FALSE;
	}


	/*
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
	*/

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
	wVersionRequested = MAKEWORD(2, 2);

	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		/* Tell the user that we could not find a usable */
		/* Winsock DLL.                                  */
		printf("WSAStartup failed with error: %d\n", err);
		return 1;
	}


	// AfxInitRichEdit2() is required to use RichEdit control
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	//SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	//LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)

	LoadHelperDLLs();

	char* appData = getenv("LOCALAPPDATA");
	if (NULL == appData) appData = getenv("TEMP");

	if (NULL == appData) {
		AfxMessageBox("Cant create temp files");
		return FALSE;

	}
	//lstrcpy(g_szDbPath, "d:\\mohan\\PathFinder13\\");
	lstrcpy(g_szAppDataPath, appData);
	lstrcat(g_szAppDataPath, "\\PathFinder");
	CreateDirectory(g_szAppDataPath, NULL);
	lstrcat(g_szAppDataPath, "\\15");
	CreateDirectory(g_szAppDataPath, NULL);
	lstrcpy(g_szDbPath, g_szAppDataPath);
	lstrcat(g_szDbPath, "\\");
	lstrcat(g_szAppDataPath, "\\cache");
	CreateDirectory(g_szAppDataPath, NULL);
	lstrcat(g_szAppDataPath, "\\");





	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, g_szPFRegistry, 0, KEY_READ, &hRegKey))
	{
		dwSize = 255;

		/*
		g_szDbPath[0] = 0;

		RegQueryValueEx(hRegKey, "DBPath", 0, NULL, (unsigned char *)g_szDbPath, &dwSize);


		dwSize = lstrlen(g_szDbPath);
		if(dwSize > 1)
		{
			if(g_szDbPath[dwSize-1] != '\\') lstrcat(g_szDbPath,"\\");
		}
		*/
		g_nLogWhoIsData = 0;

		dwSize = 4;
		RegQueryValueEx(hRegKey, "MaxHops", 0, NULL, (BYTE*)&g_nMaxHops, &dwSize);

		dwSize = 4;
		RegQueryValueEx(hRegKey, "ICMPTimeOut", 0, NULL, (BYTE*)&g_nICMPTimeout, &dwSize);

		dwSize = 4;
		RegQueryValueEx(hRegKey, "NICDBVersion", 0, NULL, (BYTE*)&g_nNICDBVersion, &dwSize);
		dwSize = 4;
		RegQueryValueEx(hRegKey, "IP2ORGVersion", 0, NULL, (BYTE*)&g_nIP2ORGVersion, &dwSize);
		dwSize = 4;
		RegQueryValueEx(hRegKey, "IP2CNTRYVersion", 0, NULL, (BYTE*)&g_nIP2CNTRYVersion, &dwSize);
		dwSize = 4;
		RegQueryValueEx(hRegKey, "IP2LocVersion", 0, NULL, (BYTE*)&g_nIP2LocVersion, &dwSize);
		//g_nLogWhoIsData WhoisLogging

		dwSize = 4;
		RegQueryValueEx(hRegKey, "WhoisLogging", 0, NULL, (BYTE*)&g_nLogWhoIsData, &dwSize);

		for (i = 0; i < 10; i++)
		{
			sprintf(szTempBuf, "MRUName%02d", i);
			dwSize = 255;
			if (ERROR_SUCCESS != RegQueryValueEx(hRegKey, szTempBuf, 0, NULL, (unsigned char*)szURL, &dwSize)) break;
			if (g_strMRUDestNames.Find(szURL) == NULL)
				g_strMRUDestNames.AddTail(szURL);
		}
		RegCloseKey(hRegKey);

		LoadAllTables();

	}
	else
	{
		//g_szDbPath[0] =0;
		g_nMaxHops = 30;
		g_nICMPTimeout = 1500;
		g_nNICDBVersion = 0;
		g_nIP2ORGVersion = 0;
		g_nIP2CNTRYVersion = 0;
		g_nIP2LocVersion = 0;

		if (ERROR_SUCCESS == RegCreateKeyEx(HKEY_CURRENT_USER, g_szPFRegistry, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hRegKey, NULL))
		{

			//GetCurrentDirectory(256, g_szDbPath);
			nLen = lstrlen(g_szDbPath);
			//if(g_szDbPath[nLen-1] != '\\') lstrcat(g_szDbPath,"\\");
			//AfxMessageBox(g_szDbPath);


			RegSetValueEx(hRegKey, "DBPath", 0, REG_SZ, (BYTE*)g_szDbPath, nLen + 1);
			RegSetValueEx(hRegKey, "MaxHops", 0, REG_DWORD, (BYTE*)&g_nMaxHops, 4);
			RegSetValueEx(hRegKey, "ICMPTimeOut", 0, REG_DWORD, (BYTE*)&g_nICMPTimeout, 4);

			RegSetValueEx(hRegKey, "NICDBVersion", 0, REG_DWORD, (BYTE*)&g_nNICDBVersion, 4);
			RegSetValueEx(hRegKey, "IP2ORGVersion", 0, REG_DWORD, (BYTE*)&g_nIP2ORGVersion, 4);
			RegSetValueEx(hRegKey, "IP2CNTRYVersion", 0, REG_DWORD, (BYTE*)&g_nIP2CNTRYVersion, 4);
			RegSetValueEx(hRegKey, "IP2LocVersion", 0, REG_DWORD, (BYTE*)&g_nIP2LocVersion, 4);

			RegSetValueEx(hRegKey, "WhoisLogging", 0, REG_DWORD, (BYTE*)&g_nIP2LocVersion, 4);


			RegCloseKey(hRegKey);

			//LoadAllTables();

		}

	}

	if (0 == g_nNICDBVersion) {
		GetDBfromClound(true);
		LoadAllTables();
	}
	else GetDBfromClound(false);


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CPathFinderDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CPathFinderView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);



	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	return TRUE;
}

// CPathFinderApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChangeEdit1();
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	//ON_EN_CHANGE(IDC_EDIT1, &CAboutDlg::OnEnChangeEdit1)
END_MESSAGE_MAP()

// App command to run the dialog
void CPathFinderApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CPathFinderApp message handlers

int CPathFinderApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class

	int i, Size;
	HKEY hRegKey;
	POSITION ListPos;
	char Buf[256], * URL;
	CString strURL;

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, g_szPFRegistry, 0, KEY_WRITE, &hRegKey))
	{
		i = 0;
		for (ListPos = g_strMRUDestNames.GetHeadPosition(); ListPos != NULL;)
		{
			strURL = g_strMRUDestNames.GetAt(ListPos);
			sprintf(Buf, "MRUName%02d", i);
			Size = strURL.GetLength();
			URL = strURL.GetBuffer(Size + 1);
			if (ERROR_SUCCESS != RegSetValueEx(hRegKey, Buf, 0, REG_SZ, (unsigned char*)URL, Size + 1)) break;
			strURL.ReleaseBuffer();
			g_strMRUDestNames.GetNext(ListPos);
			i++;
		}

		RegCloseKey(hRegKey);
	}

	return CWinApp::ExitInstance();
}

void UpdateDBVersionNumber(int DBType, int DBVersion)
{
	HKEY hRegKey;



	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, g_szPFRegistry, 0, KEY_WRITE, &hRegKey))
	{
		if (NIC_DB == DBType)
			RegSetValueEx(hRegKey, "NICDBVersion", 0, REG_DWORD, (BYTE*)&DBVersion, 4);
		else if (IPORG_DB == DBType)
			RegSetValueEx(hRegKey, "IP2ORGVersion", 0, REG_DWORD, (BYTE*)&DBVersion, 4);
		else if (IPCTY_DB == DBType)
			RegSetValueEx(hRegKey, "IP2CNTRYVersion", 0, REG_DWORD, (BYTE*)&DBVersion, 4);
		else if (IPLOC_DB == DBType)
			RegSetValueEx(hRegKey, "IP2LocVersion", 0, REG_DWORD, (BYTE*)&DBVersion, 4);

		RegCloseKey(hRegKey);
	}


}

void CAboutDlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
