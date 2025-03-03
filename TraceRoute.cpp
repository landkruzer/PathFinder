#include "pch.h"
#include "framework.h"

#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <stdio.h>
#include <string.h>

//#include "stdafx.h"
//#include <winsock.h>

#include "PathFinder.h"

#include "MainFrm.h"
#include "PathFinderDoc.h"
#include "PathFinderView.h"

//#include "winiphelper.h"
#include "traceroute.h"

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")

//#define MY_DEBUG 1
#define MY_PRINTF(...) {char cad[2048]; sprintf(cad, __VA_ARGS__);  OutputDebugString(cad);}

#define TOTALHOPS g_nMaxHops
#define WAIT_TIME g_nICMPTimeout
#define MAXPRIVATEIP 3
#define MAX_CONSECUTIVE_TIMEOUT 5

const char *g_szICMPDLL = "icmp.dll";
const char *g_szIPHelperDLL = "Iphlpapi.dll";
char g_szDbPath[256];
char g_szAppDataPath[512];
int g_nMaxHops, g_nICMPTimeout,g_nNICDBVersion,g_nIP2ORGVersion,g_nIP2CNTRYVersion,g_nIP2LocVersion;
int g_nLogWhoIsData;

HINSTANCE g_hICMPIns, g_hIPHelper;
HANDLE g_hTraceThread;
DWORD g_dwTraceThreadID;


HANDLE g_hLookupThread[40*3]; 
int g_nLookupThreads;

/*
typedef HANDLE(WINAPI* ICMPCREATEFILE)();
typedef DWORD(WINAPI* ICMPSENDECHO)(HANDLE IcmpHandle,
	IPAddr DestinationAddress,
	LPVOID RequestData,
	WORD RequestSize,
	PIP_OPTION_INFORMATION RequestOptions,
	LPVOID ReplyBuffer,
	DWORD ReplySize,
	DWORD Timeout);

typedef BOOL(WINAPI* ICMPCLOSEHANDLE)(HANDLE IcmpHandle);

typedef DWORD(WINAPI* GETBESTINTERFACE)(
	IPAddr dwDestAddr,     // destination IP address
	PDWORD pdwBestIfIndex  // index of interface with the best route
	);

typedef DWORD(WINAPI* GETIPADDRTABLE) (
	PMIB_IPADDRTABLE pIpAddrTable,
	PULONG pdwSize,
	BOOL bOrder
	);

*/
//ICMPCREATEFILE pfnIcmpCreateFile;
//ICMPSENDECHO pfnIcmpSendEcho;
//ICMPCLOSEHANDLE pfnIcmpCloseHandle;

//GETBESTINTERFACE pfnGetBestInterface;
//GETIPADDRTABLE   pfnGetIpAddrTable;

//struct THREADBUF ThreadData;
IPAddr g_ipAddress2Trace;

struct LOCATIONTBL g_LocationTbl[2048];
struct IP2LOCATION g_IP2Country[2048];
struct RegDB g_RegistryDB[2048];
struct INDIAN_IP g_IP2CompanyTable[1024];
extern struct COUNTRYLIST g_CountryList[];
extern struct STATICIPDATA g_staticNICDB[];
extern struct STATICIPDATA g_staticIPOrgDB[];
extern struct STATICIPDATA g_staticIP2CountryDB[];
extern struct STATICIPDATA g_staticIP2LocDB[];

CRITICAL_SECTION g_csRDNSList, g_csFileAccess[256]; //, g_csRegistryAccess[6];
struct RDNS *g_pRDNSHome=NULL, *g_pRDNSTail=NULL;
int g_RDNSListCount = 0;

char *g_szPrivateIPs[] = {"10.0.0.0/8,D", "172.16.0.0/12,D", "192.168.0.0/16,D"};
struct IPNETWORK g_ipPrivateIPs[3];

int g_nTotalIPLocations, g_nTotalRegDB, g_nTotalIPNetworks,g_nTotalIPCountries;
int g_nTotalCountries=250;

extern BOOL g_bCanTrace, g_bCanStopTrace, g_bCanReTrace, g_bCanSave, g_bStopTraceReqested;

void ReverseAddress(unsigned long *pnAddress);
int GetRegistryFromARIN(IPAddr ipAddress);
int GetRegistryfromWhoisRecord(char *pszWhois);

void LoadHelperDLLs()
{
int i;
/*
	g_hICMPIns = LoadLibrary(g_szICMPDLL);
	if(g_hICMPIns)
	{
		pfnIcmpCreateFile = (ICMPCREATEFILE) GetProcAddress(g_hICMPIns, "IcmpCreateFile");
		pfnIcmpCloseHandle = (ICMPCLOSEHANDLE)GetProcAddress(g_hICMPIns, "IcmpCloseHandle");
		pfnIcmpSendEcho	= (ICMPSENDECHO) GetProcAddress(g_hICMPIns, "IcmpSendEcho");
	}
	else AfxMessageBox("ICMP.DLL not found. Trace route cannot be performed");

	g_hIPHelper = LoadLibrary(g_szIPHelperDLL);
	if(g_hIPHelper)
	{
		pfnGetBestInterface = (GETBESTINTERFACE) GetProcAddress(g_hIPHelper, "GetBestInterface");
		pfnGetIpAddrTable = (GETIPADDRTABLE)  GetProcAddress(g_hIPHelper, "GetIpAddrTable");
	}
	else AfxMessageBox("IPHELPER.DLL not found. Some features will not function");
	*/

	InitializeCriticalSection(&g_csRDNSList);
	for(i=0; i < 256; i++)
		InitializeCriticalSection(&g_csFileAccess[i]);
	

//	for(i=0; i < 6; i++)
//		InitializeCriticalSection(&g_csRegistryAccess[i]);

}

int StartTracing(char *szDestBuf, HWND hViewWnd, DWORD *pIPaddress)
{
int i, nStrLen, nResult;
//IPAddr ipDestAddr;
struct hostent* phe;

	nStrLen = lstrlen(szDestBuf);

	nResult = 0;

	for(i=0;i<nStrLen;i++)
	{
		if((szDestBuf[i] >= 'a' && szDestBuf[i] <=  'z') || (szDestBuf[i] >= 'A' && szDestBuf[i] <=  'Z'))
		{
			phe = gethostbyname(szDestBuf);
			if (phe == NULL) 
			{
				AfxMessageBox("DNS Error, host name could not be resolved");
				//pApp->m_pMainWnd->SetWindowText("");
				//ThreadData.IPAddress = 0;
				return nResult;
			}
			else	
			{
				g_ipAddress2Trace = *((DWORD*)phe->h_addr_list[0]);
				nResult = 1;
				//szTitleBuf += "(";
				//NTOA(ThreadData.IPAddress, szTempBuf);
				//szTitleBuf += szTempBuf;
				//szTitleBuf += ")";
				//pApp->m_pMainWnd->SetWindowText(szTitleBuf);
			}
			break;
		}
	}
	if( i == nStrLen) 
	{			
		g_ipAddress2Trace = inet_addr(szDestBuf);
		if(INADDR_NONE == g_ipAddress2Trace)
		{
				AfxMessageBox("Invalid IP Address entered");
				g_ipAddress2Trace = 0;
				return nResult;
		}
		else 
		{
			nResult = 2;
		}
	}

	*pIPaddress = g_ipAddress2Trace;

	// Start the trace thread
	g_bCanTrace = FALSE;
	g_bCanReTrace = FALSE;
	g_bCanSave = FALSE;

	g_hTraceThread = CreateThread(NULL, 0, 
		(LPTHREAD_START_ROUTINE)TraceThreadRoutine,
		hViewWnd,  0,   &g_dwTraceThreadID);

	return nResult;
}



LONG TraceThreadRoutine(LPVOID lpv)
{
	HWND hViewWindow;
	HANDLE hICMP;
	IP_OPTION_INFORMATION ipInformation;
	char ReqBuffer[32] = "Data Buffer";
	BYTE ReplyBuffer[2048];
	DWORD l_Result, dwViewResult;
	ICMP_ECHO_REPLY* pICBuf;
	int i, j, nFailedHops;
	struct PathData* pPathRow;
	struct THREADBUF* pLUTData;
	IPAddr ipSourceIP;
	BOOL bPrivateIP;


	hViewWindow = (HWND)lpv;

	nFailedHops = 0;
	g_nLookupThreads = 0;

	g_bCanStopTrace = TRUE;


	ipSourceIP = GetSourceIP(g_ipAddress2Trace);
	if (ipSourceIP == 0)
	{
		AfxMessageBox("IP Address not configured", MB_OK | MB_ICONEXCLAMATION);
		g_bCanTrace = TRUE;
		g_bCanStopTrace = FALSE;
		return -1;

	}

	hICMP = IcmpCreateFile();

	if (INVALID_HANDLE_VALUE == hICMP) {
		// Handle error: unable to open ICMP handle
		DWORD errorvalue = GetLastError();
		MY_PRINTF("\nIcmpCreateFile failed %d\n", errorvalue);
		
		return -1;
	}

	ipInformation.Ttl = 1;
	ipInformation.Flags = 0;
	ipInformation.OptionsData = NULL;
	ipInformation.OptionsSize = 0;
	ipInformation.Tos = 0;
	
	DWORD ReplySize = sizeof(ICMP_ECHO_REPLY) + 32;
	
	l_Result = IcmpSendEcho(hICMP,
		ipSourceIP,
		ReqBuffer, 32,
		&ipInformation,
		ReplyBuffer,
		ReplySize,
		WAIT_TIME);

	if (0 != l_Result) {

		pICBuf = (ICMP_ECHO_REPLY*)ReplyBuffer;

		pPathRow = (struct PathData*)HeapAlloc(GetProcessHeap(), 0, sizeof(struct PathData));
		pPathRow->Address = pICBuf->Address;
		pPathRow->nCode = pICBuf->Status;
		pPathRow->nIndex = 0;
		pPathRow->nRTT = pICBuf->RoundTripTime;


		MY_PRINTF("Address %u, Code %d, index %d RTT %d\n", pPathRow->Address, pPathRow->nCode, pPathRow->nIndex, pPathRow->nRTT);
		//char DebugBuf[2048];
		//sprintf(DebugBuf, "Address % u, Code % d, index % d RTT % d\n", pPathRow->Address, pPathRow->nCode, pPathRow->nIndex, pPathRow->nRTT);
		//OutputDebugString(DebugBuf);

		dwViewResult = SendMessage(hViewWindow, UM_INSERT_NORMALROW, (WPARAM)pPathRow, 0);
	}
	else {
		DWORD errorvalue = GetLastError();
		if (ERROR_INVALID_PARAMETER == errorvalue) OutputDebugString("IcmpSendEcho returned ERROR_INVALID_PARAMETER");
		else MY_PRINTF("\nTracer toute failed %d\n", errorvalue);
		return -1;
	}
	


		bPrivateIP = FALSE;
		for(i = 0; i < MAXPRIVATEIP; i++)
		{
			if(IPCompare(g_ipPrivateIPs[i].ipNetwork,g_ipPrivateIPs[i].nPrefix,  pICBuf->Address) == 0)
			{
				bPrivateIP = TRUE;
				break;
			}
		}

	// Start the lookup thread	
		if(bPrivateIP == FALSE) // Is it private IP address
		{
			pLUTData = (struct THREADBUF *) HeapAlloc(GetProcessHeap(), 0, sizeof(struct THREADBUF));
			pLUTData->hViewWnd = hViewWindow;
			pLUTData->ipNode = pICBuf->Address;
			pLUTData->nIndex = 0;

			g_hLookupThread[g_nLookupThreads] = CreateThread(NULL, 0, 
				(LPTHREAD_START_ROUTINE)LookupThreadRoutine,
				pLUTData, 0, NULL);

#ifdef MY_DEBUG
			// Debugging
			WaitForSingleObject(g_hLookupThread[g_nLookupThreads],INFINITE);
#endif
			g_nLookupThreads++;
		}


	for(i=1; i <= TOTALHOPS; i++)
	{

		if(g_bStopTraceReqested) break;	

		ipInformation.Ttl = i;
		ipInformation.Flags = 0;
		ipInformation.OptionsData = NULL;
		ipInformation.OptionsSize = 0;
		ipInformation.Tos = 0;

		l_Result = IcmpSendEcho(hICMP, 
				g_ipAddress2Trace, 
				ReqBuffer, 32, 
				&ipInformation, 
				ReplyBuffer, 
				2048, 
				WAIT_TIME);

		if(l_Result)	
		{
			nFailedHops = 0;
			pICBuf = (ICMP_ECHO_REPLY*) ReplyBuffer;
			
			pPathRow = (struct PathData*) HeapAlloc(GetProcessHeap(), 0, sizeof(struct PathData));
			pPathRow->Address = pICBuf->Address;
			pPathRow->nCode = pICBuf->Status;
			pPathRow->nIndex = i;
			pPathRow->nRTT = pICBuf->RoundTripTime;

			SendMessage(hViewWindow, UM_INSERT_NORMALROW, (WPARAM) pPathRow, 0); // i, pICBuf->Address, pICBuf->RoundTripTime);

			bPrivateIP = FALSE;
			for(j = 0; j < MAXPRIVATEIP; j++)
			{
				if(IPCompare(g_ipPrivateIPs[j].ipNetwork,g_ipPrivateIPs[j].nPrefix,  pICBuf->Address) == 0)
				{
					bPrivateIP = TRUE;
					break;
				}
			}


			// Start the lookup thread

			if(bPrivateIP == FALSE) 
			{
				pLUTData = (struct THREADBUF *) HeapAlloc(GetProcessHeap(), 0, sizeof(struct THREADBUF));
				pLUTData->hViewWnd = hViewWindow;
				pLUTData->ipNode = pICBuf->Address;
				pLUTData->nIndex = i;

				g_hLookupThread[g_nLookupThreads] = CreateThread(NULL, 0, 
					(LPTHREAD_START_ROUTINE)LookupThreadRoutine,
					pLUTData, 0, NULL);

#ifdef MY_DEBUG
				// Debugging
				WaitForSingleObject(g_hLookupThread[g_nLookupThreads],INFINITE);
#endif

				g_nLookupThreads++;
			}

			/*
			
			if(i == TOTALHOPS) 
			{
				pPathRow = (struct PathData*) HeapAlloc(GetProcessHeap(), 0, sizeof(struct PathData));
				pPathRow->Address = g_ipAddress2Trace;
				pPathRow->nCode = IP_REQ_TIMED_OUT;
				pPathRow->nIndex = i+1;
				pPathRow->nRTT = -1;

				SendMessage(hViewWindow, UM_INSERT_NORMALROW, (WPARAM) pPathRow,0);
			}
			*/

			if(pICBuf->Status ==  IP_TTL_EXPIRED_TRANSIT) continue;
			if(pICBuf->Status == IP_DEST_HOST_UNREACHABLE) 
			{
				//Display error
				OutputDebugString("Host unreachable\n");
				break;
			}
			break;
		}
		else
		{
			l_Result = GetLastError();
			if (l_Result != IP_REQ_TIMED_OUT)
			{
				OutputDebugString("Unknown error returned by IcmpSendEcho\n");
			}
			else {
				MY_PRINTF("ECHO TIMEDOUT WAIT TIME %d, hop %d\n", WAIT_TIME, i);
			}
			//IP_DEST_HOST_UNREACHABLE
			nFailedHops++;
			if(nFailedHops > MAX_CONSECUTIVE_TIMEOUT) 
			{
				pPathRow = (struct PathData*) HeapAlloc(GetProcessHeap(), 0, sizeof(struct PathData));
				pPathRow->Address = g_ipAddress2Trace;
				pPathRow->nCode = IP_REQ_TIMED_OUT;
				pPathRow->nIndex = i;
				pPathRow->nRTT = -1;

				SendMessage(hViewWindow, UM_INSERT_NORMALROW, (WPARAM) pPathRow,0);

				//Sleep(100);

				bPrivateIP = FALSE;
				for(j = 0; j < MAXPRIVATEIP; j++)
				{
					if(IPCompare(g_ipPrivateIPs[j].ipNetwork,g_ipPrivateIPs[j].nPrefix,  g_ipAddress2Trace) == 0)
					{
						bPrivateIP = TRUE;
						break;
					}
				}

				if(	bPrivateIP == FALSE)
				{
					pLUTData = (struct THREADBUF *) HeapAlloc(GetProcessHeap(), 0, sizeof(struct THREADBUF));
					pLUTData->hViewWnd = hViewWindow;
					pLUTData->ipNode = g_ipAddress2Trace;
					pLUTData->nIndex = i;

					g_hLookupThread[g_nLookupThreads] = CreateThread(NULL, 0, 
						(LPTHREAD_START_ROUTINE)LookupThreadRoutine,
						pLUTData, 0, NULL);

#ifdef MY_DEBUG
					// Debugging
					WaitForSingleObject(g_hLookupThread[g_nLookupThreads],INFINITE);
#endif

					g_nLookupThreads++;
				}


				break;
			}
			SendMessage(hViewWindow, UM_INSERT_TOUTROW, i,0);
		}
	}	
	IcmpCloseHandle(hICMP);


	// Wait for all the child threads to exit

	g_bCanStopTrace = FALSE;
		
	for(i=0; i < g_nLookupThreads; i++)
	{
		WaitForSingleObject(g_hLookupThread[i], INFINITE);
		CloseHandle(g_hLookupThread[i]);
	}

	g_bCanTrace = TRUE;
	g_bCanReTrace = TRUE;
	g_bCanSave = TRUE;
	g_bStopTraceReqested = FALSE;

	return 0;
}


LONG LookupThreadRoutine(LPVOID lpv)
{
struct THREADBUF *pLUTData;
struct RDNSThreadData *pTData;
//struct hostent* phe;
char szNodeName[256], szCountryName[256], szLocation[256], szISPName[256], *pNodeName;
BOOL bResult;
HANDLE hRDNSThread;
struct NODEINFO *pNodeInfo;

	pLUTData = (struct THREADBUF *) lpv;

	hRDNSThread = NULL;

	//Check out the list before giving a network request
	bResult = GetRDNSfromList(pLUTData->ipNode, szNodeName);

	if(bResult == FALSE)  // Start a new thread which will do the serach
	{
		pTData = (struct RDNSThreadData *) HeapAlloc(GetProcessHeap(), 0, sizeof(struct RDNSThreadData));
		pTData->ipNodeAddress = pLUTData->ipNode;
		pTData->szName = szNodeName;

		hRDNSThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RDNSThreadRoutine, pTData, 0, NULL);
	}

	// Get the ISP Name and Location

	GetLocationandISPName(pLUTData->ipNode, szCountryName, szLocation, szISPName);

	pNodeInfo = (struct NODEINFO *) HeapAlloc(GetProcessHeap(), 0, sizeof(struct NODEINFO));

	pNodeInfo->Index = pLUTData->nIndex;
	if(hRDNSThread)  lstrcpy(pNodeInfo->szNodeName, "");
	else lstrcpy(pNodeInfo->szNodeName, szNodeName);
	lstrcpy(pNodeInfo->szISPName, szISPName);
	lstrcpy(pNodeInfo->szLocation, szLocation);
	lstrcpy(pNodeInfo->szCountryName, szCountryName);
	//lstrcpy(pNodeInfo->szCountryName, "India");

	PostMessage(pLUTData->hViewWnd, UM_ADD_NAME_LOC_ISP, (WPARAM) pNodeInfo, 0);

	if(hRDNSThread)  
	{
		WaitForSingleObject(hRDNSThread, INFINITE);

		pNodeName = (char *) HeapAlloc(GetProcessHeap(), 0, 256);
		lstrcpy(pNodeName, szNodeName);

		PostMessage(pLUTData->hViewWnd, UM_ADD_NAME, (WPARAM)pNodeName, pLUTData->nIndex);
		CloseHandle(hRDNSThread);
	}

	HeapFree(GetProcessHeap(), 0, pLUTData);
	return 0;
}


BOOL GetRDNSfromList(IPAddr ipNodeAddress, char *pszNodeName)
{
BOOL bResult;
//struct RDNS *g_pRDNSHome=NULL, *g_pRDNSTail=NULL;
//int g_RDNSListCount;
struct RDNS *pTempNode; //, *pPrevNode;

	bResult = FALSE	;
	
	EnterCriticalSection(&g_csRDNSList);

	if(g_RDNSListCount > 0) 
	{
		//Search
		pTempNode = g_pRDNSHome;

		while(pTempNode)
		{
			if(pTempNode->ipNodeAddress == ipNodeAddress) 
			{
				lstrcpy(pszNodeName, pTempNode->szName);
				bResult = TRUE;

				//Remove the sorting for time being
				/*
				if(g_RDNSListCount < 3) break;		
				if(pTempNode == g_pRDNSHome) break;

				if(pTempNode == g_pRDNSTail)
				{
					g_pRDNSTail = g_pRDNSTail->pPrev;
					g_pRDNSTail->pNext = NULL;
				}
				else
				{
					// Remove the temp node from the list
					pPrevNode = pTempNode->pPrev;
					pPrevNode->pNext = pTempNode->pNext;
					pTempNode->pNext->pPrev = pPrevNode;
				}
				// Now add the tempnode to the top
				pTempNode->pNext = g_pRDNSHome;
				pTempNode->pPrev = NULL;
				g_pRDNSHome->pPrev = pTempNode;
				g_pRDNSHome = pTempNode;
				*/

				break;
			}
			pTempNode = pTempNode->pNext;
		}
	}

	LeaveCriticalSection(&g_csRDNSList);

	return bResult;
}

void Add2RDNSList(IPAddr ipNodeAddress, char *pszNodeName)
{
struct RDNS *pNode;

	EnterCriticalSection(&g_csRDNSList);
	
	if(g_RDNSListCount == MAX_RDNSLIST)
	{
		// Delete the tail
		pNode = g_pRDNSTail;
		g_pRDNSTail = g_pRDNSTail->pPrev;
		g_pRDNSTail->pNext = NULL;
	}
	else
	{
		pNode = (struct RDNS *) HeapAlloc(GetProcessHeap(), 0, sizeof(struct RDNS));
		g_RDNSListCount++;
	}

	pNode->ipNodeAddress = ipNodeAddress;
	lstrcpy(pNode->szName, pszNodeName);
	pNode->pPrev = NULL;

	if(g_pRDNSHome)
	{
		pNode->pNext = g_pRDNSHome;
		g_pRDNSHome->pPrev = pNode;
		g_pRDNSHome = pNode;
	}
	else
	{
		pNode->pNext = NULL;
		g_pRDNSHome = pNode;
		g_pRDNSTail = pNode;
	}
	
	LeaveCriticalSection(&g_csRDNSList);
}


LONG RDNSThreadRoutine(LPVOID lpv)
{
struct RDNSThreadData *pRdData;
struct hostent* phe;

	pRdData = (struct RDNSThreadData *) lpv;

	phe = gethostbyaddr ((char*)&pRdData->ipNodeAddress, sizeof(DWORD), 0);
	if(phe != NULL) 
	{
		lstrcpy(pRdData->szName, phe->h_name);
		//OutputDebugString(phe->h_name);
	}
	else lstrcpy(pRdData->szName, "");

	Add2RDNSList(pRdData->ipNodeAddress, pRdData->szName);

	HeapFree(GetProcessHeap(), 0, pRdData);
	
	return 0;
}


// TODO: Check the memory. 
//if not found check the local files, 
//if not found check the whois servers, then add it to the local files.

void GetLocationandISPName(IPAddr ipNodeAddress, char *pszCountryName, char *pszLocation, char *pszISPName)
{
int i, nItems;
BOOL bIsIndianNetwork;
BOOL bIsOrgFound, bCountryFound, bLocationFound;
struct IPNETWORK *pipNetwork;
IPAddr ipTemp;
char szCnryCode[64];
int nBaseNet;
int nResult;
//char szCountryName[256];

		bIsIndianNetwork = FALSE;
		bIsOrgFound = FALSE;
		bCountryFound = FALSE;
		bLocationFound = FALSE;

		pszLocation[0] = 0;
		pszISPName[0] = 0;
		pszCountryName[0]=0;

		// Find the ISP name from the indian ISP table
		for(i=0; i < g_nTotalIPNetworks; i++)
		{
			if(g_IP2CompanyTable[i].nBits)
				ipTemp = MasktheIP(ipNodeAddress, g_IP2CompanyTable[i].nBits);
			else ipTemp = ipNodeAddress;

			if(g_IP2CompanyTable[i].ipNetwork == ipTemp)
			{
				bIsOrgFound = TRUE;
				lstrcpy(pszISPName, g_IP2CompanyTable[i].szCompany);
				break;
			}
		}

		// Find the country
			for(i=0; i < g_nTotalIPCountries; i++)
			{

				//wsprintf(pszCountryName, "Looking into g_IP2Country %d, %d\n",  i, g_IP2Country[i].ipNetwork);
				//OutputDebugString(pszCountryName);

				nResult = IPCompare(g_IP2Country[i].ipNetwork, g_IP2Country[i].nBits, ipNodeAddress);
				if(nResult < 0) continue;
				if(0 == nResult)
				{
					//Get the Country Name.
					OutputDebugString("IP Found in the IP2Country Table\n");
					GetCountryNamebyCode(g_IP2Country[i].nCountryCode, pszCountryName);
					bCountryFound = TRUE;
					//break;
				}
				break;
			}


			for(i=0; i < g_nTotalIPLocations; i++)
			{
				if(g_LocationTbl[i].Bits) 
				{
					ipTemp = MasktheIP(ipNodeAddress, g_LocationTbl[i].Bits);
				}
				else ipTemp = ipNodeAddress;
				if (g_LocationTbl[i].Address == ipTemp) 
				{
					lstrcpy(pszLocation, g_LocationTbl[i].strLocation);	
					break;
				}
			}


			if(bIsOrgFound) return;

		// Now find the country name from the regional registries. 
		// Find out from the local cache first, if not found 
		//get it from the registry and add it to the local registry

		//nRegistry = GetRegistry(ipNodeAddress);
		nBaseNet = ipNodeAddress & 0xFF;

#ifdef _DEBUG		
		char szDebugBuf[256];
		wsprintf(szDebugBuf, "Waiting for FILE access %d\n", nBaseNet);
		OutputDebugString(szDebugBuf);
#endif

		EnterCriticalSection(&g_csFileAccess[nBaseNet]);

#ifdef _DEBUG
		wsprintf(szDebugBuf, "Granted FILE access %d\n", nBaseNet);
		OutputDebugString(szDebugBuf);
#endif

		if(TRUE == GettheCountryfromLocalCache(ipNodeAddress, pszCountryName,pszISPName))
		{
#ifdef _DEBUG
			wsprintf(szDebugBuf, "Leaving FILE access %d\n", nBaseNet);
			OutputDebugString(szDebugBuf);
#endif

			LeaveCriticalSection(&g_csFileAccess[nBaseNet]);
			return;
		}

		// Now get it from the registry and add it to the local cache

		if(FALSE == GetCountryCodefromRegistry(ipNodeAddress, &pipNetwork, &nItems, szCnryCode, pszISPName))
		{
#ifdef _DEBUG
			wsprintf(szDebugBuf, "Leaving FILE access %d\n", nBaseNet);
			OutputDebugString(szDebugBuf);
#endif
			LeaveCriticalSection(&g_csFileAccess[nBaseNet]);
			//AfxMessageBox("GetWhos failed", MB_OK | MB_ICONEXCLAMATION);
			return;
		}

		GetCountryName(szCnryCode, pszCountryName, true);

		// Now add it to the local cache

		AddNetnCC2LocalCache(pipNetwork, nItems, szCnryCode,pszISPName);

		HeapFree(GetProcessHeap(), 0, pipNetwork);

#ifdef _DEBUG
			wsprintf(szDebugBuf, "Leaving FILE access %d\n", nBaseNet);
			OutputDebugString(szDebugBuf);
#endif
		LeaveCriticalSection(&g_csFileAccess[nBaseNet]);
}



void NTOA(IPAddr Address, char *Buffer)
{
	union uniIPAddr IPUnion;

	IPUnion.Adddress = Address;

	wsprintf(Buffer, "%d.%d.%d.%d", IPUnion.ByteValue.Addr1, IPUnion.ByteValue.Addr2,IPUnion.ByteValue.Addr3,IPUnion.ByteValue.Addr4);
}

void LoadAllTables()
{
char szFilePath[256], City[64];
int i=0, Bits;
IPAddr LocAddress;


	lstrcpy(szFilePath,g_szDbPath);
	lstrcat(szFilePath, "PF_NIC.txt");
	LoadRegistryDb(szFilePath);

	lstrcpy(szFilePath,g_szDbPath);
	lstrcat(szFilePath, "PF_IPOrg.txt");
	LoadIPOrgTable(szFilePath);

	lstrcpy(szFilePath,g_szDbPath);
	lstrcat(szFilePath, "PF_countries.txt");
	LoadCountryCode(szFilePath);

	lstrcpy(szFilePath,g_szDbPath);
	lstrcat(szFilePath, "PF_IPLocations.txt");
	LoadIPLocationTable(szFilePath);

	lstrcpy(szFilePath,g_szDbPath);
	lstrcat(szFilePath, "PF_IPCountry.txt");
	LoadIP2CountryCode(szFilePath);


	// Read the Private IP string and load Private IP Address List;
	for(i=0; i<3;i++)
	{
		SplitAddressnName(g_szPrivateIPs[i], &LocAddress, &Bits, City, true);
		g_ipPrivateIPs[i].ipNetwork = LocAddress;
		g_ipPrivateIPs[i].nPrefix = Bits;
	}

}

int LoadIPLocationTable(char *szFilePath)
{
char szLine[512], City[64];
IPAddr LocAddress;
int Bits,nRead, i;
FILE *fp;
int nMemElements;

	g_nTotalIPLocations = 0;
	nMemElements =0;

	fp = fopen(szFilePath,"r");
	if(fp == NULL) 
	{
		//Load from memory;
		/*
					if(lstrcmp(g_staticIPOrgDB[nMemElements].szIP, "") == 0) return 0;
			SplitIPnConvert(g_staticIPOrgDB[nMemElements].szIP, &ipNetowork, &nBits);

			g_IP2CompanyTable[g_nTotalIPNetworks].ipNetwork = ipNetowork;
			g_IP2CompanyTable[g_nTotalIPNetworks].nBits = nBits;
			lstrcpy(g_IP2CompanyTable[g_nTotalIPNetworks].szCompany, g_staticIPOrgDB[nMemElements].szName);
			g_nTotalIPNetworks++;
			nMemElements++;
			*/
		while(1)
		{
			if(lstrcmp(g_staticIP2LocDB[nMemElements].szIP, "") == 0) return 0;
			SplitIPnConvert(g_staticIP2LocDB[nMemElements].szIP, &LocAddress, &Bits);
			g_LocationTbl[g_nTotalIPLocations].Address = LocAddress;
			g_LocationTbl[g_nTotalIPLocations].Bits = Bits;
			lstrcpy(g_LocationTbl[g_nTotalIPLocations].strLocation, g_staticIP2LocDB[nMemElements].szName);
			g_nTotalIPLocations++;
			nMemElements++;
		}

	}

	while(1)
	{
		//if( fscanf(fp, "%s", Line) == EOF) break;
		if(feof(fp)) break;
		i=0;
		while(1)
		{
			nRead = fgetc(fp);
			if(nRead == EOF) break;

			szLine[i] = nRead;

			if(nRead == '\n') 
			{
				szLine[i] = 0;
				break;
			}
			i++;
		}


		if('#' == szLine[0]) continue;

		SplitAddressnName(szLine, &LocAddress, &Bits, City, true);
		g_LocationTbl[g_nTotalIPLocations].Address = LocAddress;
		g_LocationTbl[g_nTotalIPLocations].Bits = Bits;
		lstrcpy(g_LocationTbl[g_nTotalIPLocations].strLocation, City);
		g_nTotalIPLocations++;
	}
	fclose(fp);

	return 0;
}

int LoadRegistryDb(char *szFilePath)
{
char szLine[256], szRegistry[256];
IPAddr ipLocAddress;
int nBits,i;
FILE *fp;
int nNumberofstrucEntries;

	g_nTotalRegDB = 0;
	fp = fopen(szFilePath, "r");
	if(fp == NULL) 
	{

		//Load from memory
		nNumberofstrucEntries=0;
		while(1)
		{
			if(lstrcmp(g_staticNICDB[nNumberofstrucEntries].szIP, "") == 0)
			{

				return 0;
			}
			
			SplitIPnConvert(g_staticNICDB[nNumberofstrucEntries].szIP, &ipLocAddress, &nBits);

			g_RegistryDB[g_nTotalRegDB].Address = ipLocAddress;
			g_RegistryDB[g_nTotalRegDB].Bits = nBits;
			
			g_RegistryDB[g_nTotalRegDB].Registry = UNKNOWN_REGISTRY;
			if(_strcmpi(g_staticNICDB[nNumberofstrucEntries].szName, "APNIC") == 0) g_RegistryDB[g_nTotalRegDB].Registry = APNIC_DB;
			if(_strcmpi(g_staticNICDB[nNumberofstrucEntries].szName, "ARIN") == 0) g_RegistryDB[g_nTotalRegDB].Registry = ARIN_DB;
			if(_strcmpi(g_staticNICDB[nNumberofstrucEntries].szName, "RIPE") == 0) g_RegistryDB[g_nTotalRegDB].Registry = RIPE_DB;
			if(_strcmpi(g_staticNICDB[nNumberofstrucEntries].szName, "LACNIC") == 0) g_RegistryDB[g_nTotalRegDB].Registry = LACNIC_DB;
			if(_strcmpi(g_staticNICDB[nNumberofstrucEntries].szName, "AFRI") == 0) g_RegistryDB[g_nTotalRegDB].Registry = AFRINIC_DB;
			if(_strcmpi(g_staticNICDB[nNumberofstrucEntries].szName, "BRNIC") == 0) g_RegistryDB[g_nTotalRegDB].Registry = BRNIC_DB;
			if(_strcmpi(g_staticNICDB[nNumberofstrucEntries].szName, "JPNIC") == 0) 
				g_RegistryDB[g_nTotalRegDB].Registry = JPNIC_DB;

			if (0 == g_RegistryDB[g_nTotalRegDB].Registry) {
				OutputDebugString("Registry not matching\n");
			}

			g_nTotalRegDB++;
			nNumberofstrucEntries++;
		}



	}
	while(1)
	{
		if(fscanf(fp, "%[^\n]%*c", szLine) == EOF) break;
		if('#' == szLine[0]) continue;
		SplitAddressnName(szLine, &ipLocAddress, &nBits, szRegistry, false);
		g_RegistryDB[g_nTotalRegDB].Address = ipLocAddress;
		g_RegistryDB[g_nTotalRegDB].Bits = nBits;
		
		g_RegistryDB[g_nTotalRegDB].Registry = UNKNOWN_REGISTRY;
		
		if(_stricmp(szRegistry, "APNIC") == 0) g_RegistryDB[g_nTotalRegDB].Registry = APNIC_DB;
		else if(_stricmp(szRegistry, "ARIN") == 0) g_RegistryDB[g_nTotalRegDB].Registry = ARIN_DB;
		else if(_stricmp(szRegistry, "RIPE") == 0) g_RegistryDB[g_nTotalRegDB].Registry = RIPE_DB;
		else if(_stricmp(szRegistry, "LACNIC") == 0) g_RegistryDB[g_nTotalRegDB].Registry = LACNIC_DB;
		else if(_stricmp(szRegistry, "AFRI") == 0) g_RegistryDB[g_nTotalRegDB].Registry = AFRINIC_DB;
		else if(_stricmp(szRegistry, "BRNIC") == 0) g_RegistryDB[g_nTotalRegDB].Registry = BRNIC_DB;
		else if(_stricmp(szRegistry, "JPNIC") == 0) g_RegistryDB[g_nTotalRegDB].Registry = JPNIC_DB;

		if (0 == g_RegistryDB[g_nTotalRegDB].Registry) {
			OutputDebugString("Registry not matching\n");
		}

		g_nTotalRegDB++;
		//sprintf(Line, "%d,%s\n",LocAddress,City);
		//OutputDebugString(Line);
		/*
		while(1)
		{
			i = fgetc(fp);
			if(i == '\n') break;
			if(i == EOF) break;
		}
		*/
	}
	fclose(fp);
	return 0;
}

int LoadIPOrgTable(char *szFilePath)
{
FILE *fp;
int nRead, i,nBits;
char szLine[512], szCompany[64];
IPAddr ipNetowork;
int nMemElements;

	g_nTotalIPNetworks = 0;

	fp = fopen(szFilePath, "r");

	if(fp == NULL) 
	{
		nMemElements=0;

		while(1)
		{

			if(lstrcmp(g_staticIPOrgDB[nMemElements].szIP, "") == 0) return 0;
			SplitIPnConvert(g_staticIPOrgDB[nMemElements].szIP, &ipNetowork, &nBits);

			g_IP2CompanyTable[g_nTotalIPNetworks].ipNetwork = ipNetowork;
			g_IP2CompanyTable[g_nTotalIPNetworks].nBits = nBits;
			lstrcpy(g_IP2CompanyTable[g_nTotalIPNetworks].szCompany, g_staticIPOrgDB[nMemElements].szName);
			g_nTotalIPNetworks++;
			nMemElements++;

		}

	}
	while(1)
	{
		if(feof(fp)) break;
		i=0;
		while(1)
		{
			nRead = fgetc(fp);
			if(nRead == EOF) break;

			szLine[i] = nRead;

			if(nRead == '\n') 
			{
				szLine[i] = 0;
				break;
			}
			i++;
		}

		if('#' == szLine[0]) continue;
				
		SplitAddressnName(szLine, &ipNetowork, &nBits, szCompany, true);
		g_IP2CompanyTable[g_nTotalIPNetworks].ipNetwork = ipNetowork;
		g_IP2CompanyTable[g_nTotalIPNetworks].nBits = nBits;
		lstrcpy(g_IP2CompanyTable[g_nTotalIPNetworks].szCompany, szCompany);
		g_nTotalIPNetworks++;
	}
	fclose(fp);

	return 0;
}

int LoadIP2CountryCode(char *szFilePath)
{
//int i, x;
FILE *fp;
int nBits;
IPAddr ipNetwork;
char szLine[256], szCCode[256]; 
//char szTempBuf[256];
int nMemElements;

	g_nTotalIPCountries = 0;
	nMemElements =0;

	fp = fopen(szFilePath, "r");
	if(fp == NULL) 
	{
		while(1)
		{
			if(lstrcmp(g_staticIP2CountryDB[nMemElements].szIP, "") == 0) return 0;
			SplitIPnConvert(g_staticIP2CountryDB[nMemElements].szIP, &ipNetwork, &nBits);

				
			g_IP2Country[g_nTotalIPCountries].ipNetwork = ipNetwork;
			g_IP2Country[g_nTotalIPCountries].nBits = nBits;

			g_IP2Country[g_nTotalIPCountries].nCountryCode = *(short int*)g_staticIP2CountryDB[nMemElements].szName;


			g_nTotalIPCountries++;
			nMemElements++;

		}
		
		
	}

	while(1)
	{

		if( fgets(szLine, 256, fp) == NULL) break;
		if('#' == szLine[0]) continue;

		SplitAddressnCC(szLine, &ipNetwork, &nBits, szCCode);

		g_IP2Country[g_nTotalIPCountries].ipNetwork = ipNetwork;
		g_IP2Country[g_nTotalIPCountries].nBits = nBits;

		g_IP2Country[g_nTotalIPCountries].nCountryCode = *(short int*)szCCode;

		//szLine[2]=0;
		//sprintf(szTempBuf,"Country Code is %d:%s\n", ipNetwork, szCCode);
		//OutputDebugString(szTempBuf);


		g_nTotalIPCountries++;
		if(feof(fp)) break;
	}
	fclose(fp);


	return 0;
}

int LoadCountryCode(char *szFilePath)
{
int i;
//FILE *fp;
char szLine[256];//, szTempBuf[256];

	for(i=0;i<g_nTotalCountries;i++)
	{
		lstrcpy(szLine, g_CountryList[i].szCC);
		g_CountryList[i].nCountryCode = *(short int*)szLine;
	}

#if 0

	g_nTotalCountries = 0;
	fp = fopen(szFilePath, "r");
	if(fp == NULL) return -1;
	while(1)
	{
		i = 0;
		x = fread(szLine, 1, 2, fp);
		if(x < 2) break;
		fgetc(fp);
		g_CountryList[g_nTotalCountries].nCountryCode = *(short int*)szLine;
		szLine[2]=0;
		sprintf(szTempBuf,"Country Code is %d:%s\n", g_CountryList[g_nTotalCountries].nCountryCode, szLine);
		OutputDebugString(szTempBuf);

		i = 0;
		while((szLine[i++] = fgetc(fp)) != '\n');
		szLine[i-1] = 0;
		lstrcpy(g_CountryList[g_nTotalCountries].szName, szLine);
		g_nTotalCountries++;
		if(feof(fp)) break;
	}
	fclose(fp);
#endif

	return 0;
}


void SplitAddress_CC_Org(char *szLine, IPAddr *Addr, int *Bits, char *pszCC, char *pszOrg)
{
char Temp[256];
int i, Pos, nDotsCnt;
BOOL bBits;
i =0;
Pos = 0;
bBits = FALSE;

	*Bits =0;
	nDotsCnt = 0;
	
	while(1)
	{
		Temp[i] = szLine[Pos];
		if(Temp[i] == 0) break;
		if(Temp[i] == '/' || Temp[i] == ',') break;
		if(Temp[i] == '.') nDotsCnt++;
		i++;
		Pos++;
	}
	if(Temp[i] == '/') bBits = TRUE;
	Temp[i] = 0;
	if(nDotsCnt < 3) 
	{
		for(i=nDotsCnt; i <3; i++)	strcat(Temp, ".0");
	}

	*Addr = inet_addr(Temp);
	if(bBits)
	{
		i = 0;
		Pos++;
		while(1)
		{
			Temp[i] = szLine[Pos];
			if(Temp[i] == ',') break;
			i++;
			Pos++;
		}
		Temp[i] = 0;
		*Bits = atoi(Temp);
		*Addr = MasktheIP(*Addr, *Bits);
	}

	i = 0;
	Pos++;
	while(1)
	{
		pszCC[i] = szLine[Pos];
		if(pszCC[i] == 0 || pszCC[i] == ',') break;
		i++;
		Pos++;
	}
	pszCC[i] = 0;

	i=0;
	if(szLine[Pos]== 0) return;
    Pos++;
	while(1)
	{
		pszOrg[i] = szLine[Pos];
		if(pszOrg[i] == 0) break;
		i++;
		Pos++;
	}
}




void SplitAddressnCC(char *szLine, IPAddr *Addr, int *Bits, char *szName)
{
char Temp[256];
int i, Pos, nDotsCnt;
BOOL bBits;
i =0;
Pos = 0;
bBits = FALSE;

	*Bits =0;
	nDotsCnt = 0;
	
	while(1)
	{
		Temp[i] = szLine[Pos];
		if(Temp[i] == 0) break;
		if(Temp[i] == '/' || Temp[i] == ',') break;
		if(Temp[i] == '.') nDotsCnt++;
		i++;
		Pos++;
	}
	if(Temp[i] == '/') bBits = TRUE;
	Temp[i] = 0;
	if(nDotsCnt < 3) 
	{
		for(i=nDotsCnt; i <3; i++)	strcat(Temp, ".0");
	}

	*Addr = inet_addr(Temp);
	if(bBits)
	{
		i = 0;
		Pos++;
		while(1)
		{
			Temp[i] = szLine[Pos];
			if(Temp[i] == ',') break;
			i++;
			Pos++;
		}
		Temp[i] = 0;
		*Bits = atoi(Temp);
		*Addr = MasktheIP(*Addr, *Bits);
	}

	i = 0;
	Pos++;
	while(1)
	{
		szName[i] = szLine[Pos];
		if(szName[i] == 0 || szName[i] == ' ') break;
		i++;
		Pos++;
	}
	szName[i] = 0;
}

void SplitIPnConvert(char *szLine, IPAddr *Addr, int *Bits)
{
char Temp[256];
int i, Pos, nDotsCnt;
BOOL bBits;
i =0;
Pos = 0;
bBits = FALSE;

	*Bits =0;
	nDotsCnt = 0;
	
	while(1)
	{
		Temp[i] = szLine[Pos];
		if(Temp[i] == 0) break;
		if(Temp[i] == '/' || Temp[i] == ',') break;
		if(Temp[i] == '.') nDotsCnt++;
		i++;
		Pos++;
	}
	if(Temp[i] == '/') bBits = TRUE;
	Temp[i] = 0;
	if(nDotsCnt < 3) 
	{
		for(i=nDotsCnt; i <3; i++)	strcat(Temp, ".0");
	}

	*Addr = inet_addr(Temp);
	if(bBits)
	{
		i = 0;
		Pos++;
		while(1)
		{
			Temp[i] = szLine[Pos];
			if(Temp[i] == 0) break;
			i++;
			Pos++;
		}
		Temp[i] = 0;
		*Bits = atoi(Temp);
		*Addr = MasktheIP(*Addr, *Bits);
	}

	/*
	i = 0;
	Pos++;
	while(1)
	{
		szName[i] = szLine[Pos];
		if(szName[i] == 0) break;
		i++;
		Pos++;
	}
	*/
}



void SplitAddressnName(char *szLine, IPAddr *Addr, int *Bits, char *szName, bool NameIncludesSpace)
{
char Temp[256];
int i, Pos, nDotsCnt;
BOOL bBits;
i =0;
Pos = 0;
bBits = FALSE;

	*Bits =0;
	nDotsCnt = 0;
	
	while(1)
	{
		Temp[i] = szLine[Pos];
		if(Temp[i] == 0) break;
		if(Temp[i] == '/' || Temp[i] == ',') break;
		if(Temp[i] == '.') nDotsCnt++;
		i++;
		Pos++;
	}
	if(Temp[i] == '/') bBits = TRUE;
	Temp[i] = 0;
	if(nDotsCnt < 3) 
	{
		for(i=nDotsCnt; i <3; i++)	strcat(Temp, ".0");
	}

	*Addr = inet_addr(Temp);
	if(bBits)
	{
		i = 0;
		Pos++;
		while(1)
		{
			Temp[i] = szLine[Pos];
			if(Temp[i] == ',') break;
			i++;
			Pos++;
		}
		Temp[i] = 0;
		*Bits = atoi(Temp);
		*Addr = MasktheIP(*Addr, *Bits);
	}

	i = 0;
	Pos++;
	while(1)
	{
		szName[i] = szLine[Pos];
		if(0 == szName[i] ) break;
		if (' ' == szName[i] && !NameIncludesSpace) {
			szName[i] = 0;
			break;
		}
		i++;
		Pos++;
	}
}


IPAddr MasktheIP(IPAddr Addr, int nMaskbits)
{
	
	ReverseAddress(&Addr);
	Addr = Addr & (0xFFFFFFFF << (32-nMaskbits));
	ReverseAddress(&Addr);

	return Addr;
}


IPAddr GetSourceIP(IPAddr DestinationIP)
{
DWORD dwIf, dwTableSize, i, dwResult;
PMIB_IPADDRTABLE pIPAddrTable;
IPAddr SourceIP;

	dwResult = GetBestInterface(DestinationIP, &dwIf);
	if(dwResult != NO_ERROR)
	{
		return 0;
	}

	dwTableSize = 0;
	pIPAddrTable = NULL;
	GetIpAddrTable(pIPAddrTable, &dwTableSize, FALSE);

	pIPAddrTable = (PMIB_IPADDRTABLE) HeapAlloc(GetProcessHeap(), 0, dwTableSize);

	GetIpAddrTable(pIPAddrTable, &dwTableSize, FALSE);

	SourceIP = 0;
	for(i=0; i < pIPAddrTable->dwNumEntries; i++)
	{
		if(pIPAddrTable->table[i].dwIndex == dwIf) 
		{
			SourceIP = pIPAddrTable->table[i].dwAddr;
			break;
		}
	}

	HeapFree(GetProcessHeap(), 0, pIPAddrTable);

	return SourceIP;
}



BOOL GettheCountryfromLocalCache(IPAddr ipNodeAddress, char *pszCC,char *pszISPName)
{
FILE *fp;
char szFileName[256], szLine[256], szCCode[16],szOrgName[256];
int nBits;
IPAddr ipNetwork;
int nResult;


	lstrcpy(szFileName, g_szAppDataPath);
	wsprintf(szLine, "Network%03d.txt", ipNodeAddress & 0xFF);
	lstrcat(szFileName, szLine);


	fp = fopen(szFileName, "r");
	if(fp == NULL) return FALSE;

	while(1)
	{
		if( fgets(szLine,256,fp) == NULL) break;

		SplitAddress_CC_Org(szLine, &ipNetwork, &nBits, szCCode,szOrgName);

		nResult = IPCompare(ipNetwork, nBits, ipNodeAddress);

		if(nResult == 0) break;
		if(nResult < 0) continue;
		break;
	}

	fclose(fp);
	if(nResult == 0) 
	{
		GetCountryName(szCCode, pszCC, true);
		lstrcpy(pszISPName, szOrgName);
		return TRUE;
	}
	return FALSE;
}

void AddNetnCC2LocalCache(struct IPNETWORK *pNet, int nItems, char *pszCnryCode, char *pszISPName)
{
FILE *fp, *fpBackup;
char szFileName[256], szBackupFile[256], szLine[256], szTempLine[256], szIPNet[32];
char szCCode[32], szOrgName[256];
int nBits;
IPAddr ipNetwork;
int nResult,i, nBasenet;
struct IPNETWORK *pipNetTemp;
BOOL bContinueRead;

	nBasenet = pNet->ipNetwork & 0xFF;

	lstrcpy(szFileName, g_szAppDataPath);
	wsprintf(szLine, "Network%03d.txt", nBasenet);
	lstrcat(szFileName, szLine);

	//EnterCriticalSection(&g_csFileAccess[nBasenet]);
	fp = fopen(szFileName, "r");
	if(fp == NULL) 
	{
		fp = fopen(szFileName, "w");
		if(fp != NULL) 
		{
			for(i=0;i< nItems; i++)
			{
				NTOA(pNet->ipNetwork, szIPNet);
				sprintf(szLine,"%s/%d,%s,%s\n",szIPNet, pNet->nPrefix, pszCnryCode,pszISPName);
				OutputDebugString(szLine);
				fprintf(fp, "%s/%d,%s,%s\n",szIPNet, pNet->nPrefix, pszCnryCode,pszISPName);
				pNet++;
			}
			fclose(fp);
		}
	}
	else 
	{
	// Read and inser this records in the correct place.
		lstrcpy(szBackupFile, g_szAppDataPath);
		wsprintf(szLine, "Network%03d.bak", nBasenet);
		lstrcat(szBackupFile, szLine);

		OutputDebugString("Creating new file ");
		OutputDebugString(szBackupFile);
		OutputDebugString("\n");


		fpBackup = fopen(szBackupFile, "w");
		if(fpBackup)
		{
			while(1)
			{
				if( fgets(szLine, 256, fp) == NULL) break;

				SplitAddress_CC_Org(szLine, &ipNetwork, &nBits, szCCode,szOrgName);

				// Find out whether we have reached the place to insert.
				nResult = IPCompare(ipNetwork, nBits, pNet->ipNetwork);

				if(nResult >= 0) break; 
			
				//Continue copying file from the old file to new file
				fputs(szLine, fpBackup);
				//fputs("\n", fpBackup);
			}
			if(!feof(fp))
			{			
				while(1)
				{
					pipNetTemp = pNet;
					bContinueRead = FALSE;
					for(i=0; i < nItems; i++)
					{
						if(0 == IPCompare(pipNetTemp->ipNetwork, pipNetTemp->nPrefix, ipNetwork))
						{
							bContinueRead = TRUE;
							break;
						}
						pipNetTemp++;
					}
					if(!bContinueRead) break;
					if( fgets(szLine, 256, fp) == NULL) break;

					SplitAddress_CC_Org(szLine, &ipNetwork, &nBits, szCCode,szOrgName);
				}
			}

			for(i=0;i< nItems; i++)
			{
				NTOA(pNet->ipNetwork, szTempLine);
				fprintf(fpBackup, "%s/%d,%s,%s\n",szTempLine, pNet->nPrefix, pszCnryCode,pszISPName);
				pNet++;
			}
			if(!feof(fp))
			{
				fputs(szLine, fpBackup);
				//fputs("\n", fpBackup);
				while(1)
				{
					if( fgets(szLine,256,fp) == NULL) break;
					fputs(szLine, fpBackup);
					//fputs("\n", fpBackup);
				}
			}
			fclose(fpBackup);
		}	
		fclose(fp);
		// Delete the original file
		DeleteFile(szFileName);
		//Rename the backup file
		MoveFile(szBackupFile, szFileName);
	}
	//LeaveCriticalSection(&g_csFileAccess[nBasenet]);

}


int IPCompare(IPAddr ipNetwork, int nPrefix, IPAddr ipNodeAddress)
{
IPAddr ipTemp;
//char szNetwork[256],szAddress[256], szTemp[256];
unsigned char *cNetwork, *cTemp;
int i;

	ipTemp = MasktheIP(ipNodeAddress, nPrefix);

	//NTOA(ipNetwork, szNetwork);
	//NTOA(ipNodeAddress, szAddress);
	//NTOA(ipTemp, szTemp);

	if(ipTemp == ipNetwork) return 0;

	cNetwork = (unsigned char*) &ipNetwork;
	cTemp = (unsigned char*) &ipTemp;

	for(i=0; i < 4; i++)
	{
		if(cNetwork[i] == cTemp[i]) continue;
		if(cNetwork[i] < cTemp[i]) return -1;
		return 1;
	}

	return 0;
}


void GetCountryNamebyCode(int CC, char *pszCountry)
{
int i;


	for(i=0; i < g_nTotalCountries; i++)
	{
		if(g_CountryList[i].nCountryCode == CC) 
		{
			lstrcpy(pszCountry, g_CountryList[i].szName);
			break;
		}
	}
}



void GetCountryName(char *pszCnryCode, char *pszCountry,bool addprefix)
{
int nCCode, i;

	if(pszCnryCode[0] >= 'A' && pszCnryCode[0] <= 'Z') pszCnryCode[0] += 'a' - 'A';
	if(pszCnryCode[1] >= 'A' && pszCnryCode[1] <= 'Z') pszCnryCode[1] += 'a' - 'A';

	nCCode = *(short int*)(pszCnryCode);

	if(addprefix) lstrcpy(pszCountry, "~");
	else pszCountry[0] = 0;

	for(i=0; i < g_nTotalCountries; i++)
	{
		if(g_CountryList[i].nCountryCode == nCCode) 
		{
			lstrcat(pszCountry, g_CountryList[i].szName);
			break;
		}
	}
}


BOOL GetCountryCodefromRegistry(IPAddr ipNodeAddress, struct IPNETWORK **ppNet, int *pnItems, char *pszCnryCode, char *pszISPName)
{
int nRegRegistry;
int nResult;
char *pszWhois, szIPAddress[256];
//struct IPNETWORK *pNet;
//int nItems;

	nResult = 0;
	nRegRegistry = GetRegistry(ipNodeAddress);

	if(nRegRegistry != 0)
	{
		NTOA(ipNodeAddress, szIPAddress);
		pszWhois = GetIPDetails(nRegRegistry, szIPAddress); 

		if(pszWhois) 
		{
			//OutputDebugString(szIPAddress);
			//OutputDebugString("\n");

			//OutputDebugString(pszWhois);
			//OutputDebugString("\n");
			if(g_nLogWhoIsData)
				Write2File(szIPAddress, pszWhois);

			if(JPNIC_DB == nRegRegistry) 
				nResult = GetCCnNetworkfromJP(pszWhois, nRegRegistry, ppNet, pnItems, pszCnryCode,pszISPName);
			else nResult = GetCCnNetwork(pszWhois, nRegRegistry, ppNet, pnItems, pszCnryCode,pszISPName);
			free(pszWhois);

		}
	}
	return nResult;
}

int GetRegistry(IPAddr ipAddress)
{
int i, PreBits;
IPAddr ipTemp;
	

	PreBits =0;
	ipTemp = ipAddress;
	
	for(i=0;i < g_nTotalRegDB; i++)
	{
		if(PreBits != g_RegistryDB[i].Bits) 
		{
			if( IPCompare(g_RegistryDB[i].Address, g_RegistryDB[i].Bits, ipAddress) == 0)
			{
				if(g_RegistryDB[i].Registry == 0) break;
				return g_RegistryDB[i].Registry;
			}
		}
	}

	//Find the registry from ARIN 
	//return GetRegistryFromARIN(ipAddress);
	return 0;
}

int GetRegistryFromARIN(IPAddr ipAddress)
{
char *pszWhois,szIPAddress[256];
int nRIR;

	nRIR = 0;
	NTOA(ipAddress, szIPAddress);
	pszWhois = GetIPDetails(ARIN_DB, szIPAddress);
	if(pszWhois)
	{
		nRIR = GetRegistryfromWhoisRecord(pszWhois);
		free(pszWhois);
	}
	return nRIR;
}

//OrgID: AFRINIC, APNIC, RIPE, LACNIC
int GetRegistryfromWhoisRecord(char *pszWhois)
{
char szTokenBuf[256];
int nTokenFound, nBufIndex, nOrgIDFound, nRegistry,nTokBufIndex;
	
	nBufIndex = 0;
	nOrgIDFound = 0;
	nRegistry = 0;
	
	while(!nOrgIDFound)
	{
		nTokenFound = 0;
		switch(pszWhois[nBufIndex])
		{
		case 0:
			return 0;
		case '%':
		case '#':
			while(pszWhois[nBufIndex++] != '\n');
			break;
		case '\n':
			nBufIndex++;
			break;
		case '\r':
			nBufIndex++;
			break;
		default:
			nTokBufIndex = 0;
			while(1)
			{
				szTokenBuf[nTokBufIndex] = pszWhois[nBufIndex++];
				if(szTokenBuf[nTokBufIndex] == ':')
				{
					nTokenFound = 1;
					szTokenBuf[nTokBufIndex] = 0;
					break;
				}
				if(szTokenBuf[nTokBufIndex] == '\n') break;
				if(szTokenBuf[nTokBufIndex] == 0)
				{
					nBufIndex--;
					break;
				}
				nTokBufIndex++;
			}
			if(nTokenFound == 0) break;
			while(1) 
			{
				if(pszWhois[nBufIndex] == ' ') 
				{
					nBufIndex++;
					continue;
				}
				break;
			}
			if(_strcmpi(szTokenBuf, "OrgID") == 0)
			{
				nTokBufIndex = 0;
				while(1)
				{
					szTokenBuf[nTokBufIndex] = pszWhois[nBufIndex++];
					if(szTokenBuf[nTokBufIndex] == '\n' || szTokenBuf[nTokBufIndex] == ' ' || szTokenBuf[nTokBufIndex] == 0)
					{
						szTokenBuf[nTokBufIndex] = 0;
						break;
					}
					nTokBufIndex++;
				}
				nOrgIDFound = 1;
			}

		}
	}


	if(nOrgIDFound)
	{
		if(_strcmpi(szTokenBuf, "APNIC") == 0) nRegistry = APNIC_DB;
		else if(_strcmpi(szTokenBuf, "RIPE") == 0) nRegistry = RIPE_DB;
		else if(_strcmpi(szTokenBuf, "AFRINIC") == 0) nRegistry = AFRINIC_DB;
		else if(_strcmpi(szTokenBuf, "LACNIC") == 0) nRegistry = LACNIC_DB;
	}


	return 0;
}

int GetCCnNetworkfromJP(char *pszWhois, int nRegistry, struct IPNETWORK **ppNet, int *pnItems, char *pszCnryCode,char *pszOrgName)
{
char *pTempBuf;
char BufA[512], BufB[512], BufC[512];
int i;
//, BufD[512];


		pszCnryCode[0] = 'j';
		pszCnryCode[1] = 'p';
		pszCnryCode[2] = 0;

	pTempBuf = strstr(pszWhois, "Network Number");
	if(pTempBuf != NULL) 
	{
		sscanf(pTempBuf, "%s %s %s",  &BufA, &BufB, &BufC);
		//printf("Data size is %d\n", *contentsize);
		pTempBuf = strstr(BufC, "/");
		if(pTempBuf != NULL)
			CIDRList2Network(BufC, ppNet, pnItems);
		else INetNum2Network(BufC, ppNet, pnItems);

	}
	else return 0;

	//scanf("%[^\n]s", str);

	pTempBuf = strstr(pszWhois, "Organization");
	if(pTempBuf != NULL) 
	{
		sscanf(pTempBuf+13, "%[^\n]s", &BufA);
		i=0;
		while(BufA[i] == ' ') i++;
		strcpy(pszOrgName, BufA+i);
		//sscanf(pTempBuf, "%s %s",  &BufA, &BufB);
		//printf("Data size is %d\n", *contentsize);
	}



	return 1;
}


// US Registry CIDR
// RIPE & APNIC inetnum

int GetCCnNetwork(char *pszWhois, int nRegistry, struct IPNETWORK **ppNet, int *pnItems, char *pszCnryCode,char *pszOrgName)
{
char *pszCIDR = "CIDR";
char *pszINETNUM = "inetnum";
char *pszCountr = "country";
char szOrg[256] = "Org-name";
char szDesc[256];
char szNetName[256];
char szAddress[256];
//char *pszOrg1 = "OrgName";
//char szOrgName[256];

char szCIDRList[512],szInetNum[64], szTempBuf[256];
//IPAddr ipTemp;
//int nPrefix;

int nNetFound, nCountryFound, nTokenFound, nOrgFound, endofString, nNetNameFound;
int nAddressFound;
int i, j;
int nRouteFound=0;
int nDescFound=0;

	nNetFound = 0;
	nCountryFound = 0;
	nOrgFound = 0;
	endofString=0;
	nNetNameFound=0;
	nAddressFound=0;
	i = 0;

	if(ARIN_DB == nRegistry) lstrcpy(szOrg, "OrgName"); 
	else if(LACNIC_DB == nRegistry || BRNIC_DB == nRegistry) lstrcpy(szOrg, "owner");

	if(BRNIC_DB == nRegistry) 
	{
		nCountryFound = 1;
		pszCnryCode[0] = 'b';
		pszCnryCode[1] = 'r';
		pszCnryCode[2] = 0;
	}



	//OutputDebugString(pszWhois);
	while(!nNetFound || !nCountryFound || !nOrgFound)
	{
		nTokenFound = 0;
		switch(pszWhois[i])
		{
		case 0:
			endofString=1;
			break;
		case '%':
		case '#':
			while(pszWhois[i++] != '\n');
			break;
		case '\n':
			i++;
			break;
		case '\r':
			i++;
			break;
		default:
			j = 0;
			while(1)
			{
				szTempBuf[j] = pszWhois[i++];
				if(szTempBuf[j] == ':') 
				{
					nTokenFound = 1;
					szTempBuf[j] = 0;
					break;
				}
				if(szTempBuf[j] == '\n') break;
				if(szTempBuf[j] == 0) 
				{
					i--;
					break;
				}
				j++;
			}
			if(nTokenFound == 0) break;

			while(1) 
			{
				if(pszWhois[i] == ' ' || pszWhois[i] == 9) 
				{
					i++;
					continue;
				}
				break;
			}


			if(!nAddressFound)
			{
				if(lstrcmpi(szTempBuf, "address") == 0) {
									
					j = 0;
					//i--;
					while((szAddress[j++] = pszWhois[i++]) != '\n');
					szAddress[j-1] = 0;

					nAddressFound=1;
					break;
				}
			}


			if(!nRouteFound) 
			{
				if(lstrcmpi(szTempBuf, "route") == 0) {
					while(pszWhois[i++] != '\n');
					nRouteFound=1;
					break;
				}
			}

			if(!nDescFound) 
			{
				if(lstrcmpi(szTempBuf, "descr") == 0)
				{
					j = 0;
					//i--;
					while((szDesc[j++] = pszWhois[i++]) != '\n');
					szDesc[j-1] = 0;
					nDescFound = 1;
					break;
				}
			}

			

			if(!nNetNameFound)
			{
				if(lstrcmpi(szTempBuf, "netname") == 0)
				{
					j = 0;
					//i--;
					while((szNetName[j++] = pszWhois[i++]) != '\n');
					szNetName[j-1] = 0;
					nNetNameFound = 1;
					break;
				}
			}
			

			if(!nOrgFound) 
			{
				if((lstrcmpi(szTempBuf, szOrg) == 0) || (lstrcmpi(szTempBuf,"Organization") == 0))
				{
					j = 0;
					//i--;
					while((pszOrgName[j++] = pszWhois[i++]) != '\n');
					pszOrgName[j-1] = 0;
					nOrgFound = 1;
					break;
				}
			}



			if(!nCountryFound) 
			{
				if(lstrcmpi(szTempBuf, pszCountr) == 0)
				{
					j = 0;
					//i--;
					while((pszCnryCode[j++] = pszWhois[i++]) != '\n');
					pszCnryCode[2] = 0;
					nCountryFound = 1;
					break;
				}
			}
			if(!nNetFound)
			{
				/*
				if(lstrcmpi(szTempBuf, pszCIDR) == 0)
				{
					j = 0;
					//i--;
					while((szCIDRList[j++] = pszWhois[i++]) != '\n');
					szCIDRList[j-1] = 0;
					nNetFound = 1;
					break;
				}
				
				else 
				*/
				if((lstrcmpi(szTempBuf, pszINETNUM) == 0) || (lstrcmpi(szTempBuf,"NetRange") == 0))
				{
					j = 0;
					//i--;
					while((szInetNum[j++] = pszWhois[i++]) != '\n');
					
					if(LACNIC_DB == nRegistry || BRNIC_DB == nRegistry) 
					{
						szInetNum[j-1] = 0;
						nNetFound = 3;
					}
					else 
					{
						szInetNum[j-1] = 0;
						nNetFound = 2;
					}
					break;
				}
			}
			while(pszWhois[i++] != '\n');
		}
		if(endofString) break;
	}
	


	if(!nCountryFound) 
	{
		if(LACNIC_DB == nRegistry) 
		{
			nCountryFound = 1;
			pszCnryCode[0] = 'b';
			pszCnryCode[1] = 'r';
			pszCnryCode[2] = 0;
		}
	}
		

	if(!nCountryFound) return 0;
	if(!nNetFound) return 0;

	if(RIPE_DB == nRegistry) 
	{
		if(lstrcmpi(pszCnryCode, "eu") == 0) 
		{

			if(nNetNameFound && (lstrcmpi(szNetName, "NON-RIPE-NCC-MANAGED-ADDRESS-BLOCK") == 0)) return 0;
		
		}
	}
	else if (ARIN_DB == nRegistry) 
	{
		if(lstrcmpi(pszCnryCode, "mu") == 0) 
		{
			if(nNetNameFound && (strstr(szNetName, "AFRINIC-ERX") != NULL)) 
			{
				lstrcpy(pszOrgName, "");
				return 0;
			}
		}
		else if(lstrcmpi(pszCnryCode, "nl") == 0)
		{
			if(nNetNameFound && (strstr(szNetName, "RIPE-ERX") != NULL)) 
			{
				lstrcpy(pszOrgName, "");
				return 0;
			}
		}
		else if(lstrcmpi(pszCnryCode, "au") == 0)
		{
			if(nNetNameFound && (strstr(szNetName, "APNIC-ERX") != NULL))
			{
				lstrcpy(pszOrgName, "");
				return 0;
			}
		}

	}
	else if(APNIC_DB == nRegistry)
	{
		if(lstrcmpi(pszCnryCode, "au") == 0)
		{
			if(nNetNameFound && (strstr(szNetName, "ERX-NETBLOCK") != NULL))
			{
				lstrcpy(pszOrgName, "");
				return 0;
			}
		}
		else if(lstrcmpi(pszCnryCode, "jp") == 0)
		{
			//JPNIC-NET-JP
			if(nNetNameFound && (strstr(szNetName, "JPNIC-NET-JP") != NULL))
			{
				lstrcpy(pszOrgName, "");
				return 0;
			}

		}
	}

	//if(BRNIC_DB == nRegistry) nNetFound=3;

	
	if(!nOrgFound) {

		if((APNIC_DB == nRegistry) && nDescFound) 
		{
			lstrcpy(pszOrgName, szDesc);
		}
		else if(RIPE_DB == nRegistry) 
		{
			if(nAddressFound) lstrcpy(pszOrgName, szAddress);
		}
		else if(nNetNameFound) 
		{

			lstrcpy(pszOrgName, szNetName);
		}
	}
	

	if(nNetFound == 1) 
	{
		CIDRList2Network(szCIDRList, ppNet, pnItems);
		return 1;
	}
	else if(nNetFound == 2) 
	{
		INetNum2Network(szInetNum, ppNet, pnItems);
		return 1;
	}
	else
	{
		CIDRList2Network(szInetNum, ppNet, pnItems);
		return 1;

	}
	return 0;
}

void CIDRList2Network(char *pszCIDRList, struct IPNETWORK **ppNet, int *pnItems)
{
int i, j, nNoNets, nDotCount,nLoop;
char szTempBuf[256], szPrefix[256];
IPAddr ipTemp;
struct IPNETWORK *pNetworkList, *pNetworkNode;

	// Number of nets
	i = 0;
	nNoNets = 1;
	while(pszCIDRList[i] != 0)
	{
		if(pszCIDRList[i++] == ',') nNoNets++;
	}
	// Allocate memory
	pNetworkList = (struct IPNETWORK *)HeapAlloc(GetProcessHeap(), 0, sizeof(struct IPNETWORK) * nNoNets);
	pNetworkNode = pNetworkList;

	i=0;
	//nNoNets = 0;
	while(1)
	{
		j = 0;
		nDotCount = 0;
		while(1)
		{
			szTempBuf[j] = pszCIDRList[i];
			if(szTempBuf[j]  == '/') break;
			if(szTempBuf[j] == '.') nDotCount++;
			i++;
			j++;
		}
		szTempBuf[j] = 0;
		i++;

		if(nDotCount < 3) 
		{
			for(nLoop=nDotCount; nLoop <3; nLoop++)	lstrcat(szTempBuf, ".0");
		}

		j = 0;
		while(1)
		{
			szPrefix[j] = pszCIDRList[i];
			if(szPrefix[j] == ',' || szPrefix[j] == 0) 
			{
				szPrefix[j] = 0;
				break;
			}
			i++;
			j++;
		}
		ipTemp = inet_addr(szTempBuf);
		j = atoi(szPrefix);
		ipTemp = MasktheIP(ipTemp, j);
		pNetworkNode->ipNetwork = ipTemp;
		pNetworkNode->nPrefix = j;
		pNetworkNode++;
		//nNoNets++;

		if(pszCIDRList[i] == 0) break;
		while(pszCIDRList[i++] != ' '); 
		if(pszCIDRList[i] == 0) break;
	}

	*ppNet = pNetworkList;
	*pnItems = nNoNets;
}


void ReverseAddress(unsigned long *pnAddress)
{
union uniIPAddr ipuTemp1, ipuTemp2;

	ipuTemp1.Adddress = *pnAddress;

	ipuTemp2.ByteValue.Addr1 = ipuTemp1.ByteValue.Addr4;
	ipuTemp2.ByteValue.Addr2 = ipuTemp1.ByteValue.Addr3;
	ipuTemp2.ByteValue.Addr3 = ipuTemp1.ByteValue.Addr2;
	ipuTemp2.ByteValue.Addr4 = ipuTemp1.ByteValue.Addr1;


	*pnAddress = ipuTemp2.Adddress;


}

void INetNum2Network(char *pszInetNum, struct IPNETWORK **ppNet, int *pnItems)
{
int i, j;
char szStartAddr[32], szEndAddr[32];
IPAddr ipStart, ipEnd, ipTemp1;
struct IPNETWORK *pNetworkList, *pNetworkNode;
int nNoNets;


pNetworkList = NULL;
	i  = 0;
	j = 0;


	while((szStartAddr[j] = pszInetNum[i]) != '-') 
	{
		i++;
		j++;
	}			
	szStartAddr[j] = 0;

	j=0;
	i++;
	while(pszInetNum[i] == ' ' ) i++;
	while((szEndAddr[j++] = pszInetNum[i++]) != 0);
	szEndAddr[j]=0;


	ipStart = inet_addr(szStartAddr);
	ipEnd = inet_addr(szEndAddr);

	ReverseAddress(&ipStart);
	ReverseAddress(&ipEnd);

	ipStart &= 0xFFFFFF00;
	ipEnd |= 0xFF;

	nNoNets =0;

	{
		unsigned int Bit = 0;
		unsigned int Mask = 0;
		ipTemp1 = ipStart;

		while(ipTemp1 <= ipEnd)
		{
			Mask |= (1 << Bit);
			if( (ipTemp1 & Mask)||  ((ipTemp1 | Mask )> ipEnd))
			{
				nNoNets++;
				ipTemp1 += ( 1 << Bit);
				Bit = 0;
				Mask = 0;
			}
			else Bit++;
		}
	}


	if(nNoNets)
	{

		pNetworkList = (struct IPNETWORK *)HeapAlloc(GetProcessHeap(), 0, sizeof(struct IPNETWORK) * nNoNets);
		pNetworkNode = pNetworkList;
		
		{
			unsigned int Bit = 0;
			unsigned int Mask = 0;
			ipTemp1 = ipStart;

			while(ipTemp1 <= ipEnd)
			{
				Mask |= (1 << Bit);
				if( (ipTemp1 & Mask)||  ((ipTemp1 | Mask )> ipEnd))
				{
					pNetworkNode->ipNetwork = ipTemp1;
					ReverseAddress(&pNetworkNode->ipNetwork);
					pNetworkNode->nPrefix = 32-Bit;
					pNetworkNode++;

					ipTemp1 += ( 1 << Bit);
					Bit = 0;
					Mask = 0;
				}
				else Bit++;
			}
		}
	}

	*ppNet = pNetworkList;
	*pnItems = nNoNets;
}


void Write2File(char *pszFileName, char *pszData)
{
FILE *fp;
char szFullPath[256];

	lstrcpy(szFullPath, g_szAppDataPath);
	//wsprintf(szLine, "Network%03d.txt", nBasenet);
	lstrcat(szFullPath, pszFileName);
	lstrcat(szFullPath,".txt");

		fp = fopen(szFullPath, "w");
		if(fp != NULL) 
		{
			fwrite(pszData, strlen(pszData), 1, fp);
			fclose(fp);
		}
}