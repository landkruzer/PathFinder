//#include "stdafx.h"
//#include <winsock.h>
#include "pch.h"

//#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
# include <winsock2.h>

//#include "PathFinder.h"

//#include "MainFrm.h"
//#include "PathFinderDoc.h"
//#include "PathFinderView.h"

//#include "winiphelper.h"
//#include "traceroute.h"

#define US_SERVER_NAME "windowstraceroute.000webhostapp.com"
#define INDIA_SERVER_NAME "oakswood.hopto.org"

#define US_PORT_NUMBER 80
#define INDIA_PORT_NUMBER 8080
#define MAX_DB_BUF_SIZE 100000

#define NIC_DB 1
#define IPORG_DB 2
#define IPCTY_DB 3
#define IPLOC_DB 4

HANDLE g_hGetDBThread;
DWORD g_dwGetDBThreadID;
LONG GetDBfromCloudThreadRoutine(LPVOID lpv);
void UpdateDBVersionNumber(int DBType, int DBVersion);

extern char g_szDbPath[256];
extern int g_nNICDBVersion,g_nIP2ORGVersion,g_nIP2CNTRYVersion,g_nIP2LocVersion;

char g_szDBServer[256];

static int Write2File1(char *pszFileName, char *pszData)
{
FILE *fp;
char szFullPath[256];

	//lstrcpy(szFullPath, g_szAppDataPath);
	//wsprintf(szLine, "Network%03d.txt", nBasenet);
	lstrcpy(szFullPath, pszFileName);
	//lstrcat(szFullPath,".txt");

		fp = fopen(szFullPath, "wb");
		if(fp != NULL) 
		{
			fwrite(pszData, strlen(pszData), 1, fp);
			fclose(fp);
			return 0;
		}
		return -1;
}

char *ParseHTTPHeader(char *httpHeader,  int *contentsize)
{
//header HTTP/1.1 200 OK
//Content-Length:
char LineBuf[256];
int httpResponseCode;
char *pTempBuf;

	sscanf(httpHeader, "%s %d", &LineBuf, &httpResponseCode);

	//printf("%s %d\n", LineBuf, httpResponseCode );

	if(httpResponseCode != 200) return NULL;

	pTempBuf = strstr(httpHeader, "Content-Length:");

	if(pTempBuf != NULL) 
	{
		sscanf(pTempBuf, "%s %d",  &LineBuf, contentsize);
		//printf("Data size is %d\n", *contentsize);
	}

	pTempBuf = strstr(httpHeader, "\r\n\r\n");

	if(pTempBuf == NULL) return NULL;
	
	

	return pTempBuf+4;
}

int readhttpresponse(int iSocketFD, char *pReturnBuf)
{
int recv_size;
char *rxBuf;
DWORD timeout;
int  contentsize;
//int result;

char headerBuf[1024];
char *tempBuf;
int DataSize=0;
int TimeoutCount=100000;
	//rxBuf = (char*) malloc(100000);

	rxBuf = pReturnBuf;

	timeout = 5000;
	setsockopt(iSocketFD, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));

	/* Receive the header */
	recv_size = recv(iSocketFD, headerBuf, 500, 0);

	if(recv_size == -1 || recv_size == 0) return -1;
	//Parse the header
	headerBuf[recv_size]=0;
	//printf("header %s\n", headerBuf);
	
	tempBuf = ParseHTTPHeader(headerBuf, &contentsize);
	if(tempBuf) {
		//if(recv_size > headersize) 
		printf("Content Lenght is %d\n", contentsize);
		lstrcpy(rxBuf, tempBuf);
		DataSize=lstrlen(rxBuf);
	}
	else return -1;
	//puts(rxBuf);
	//printf("Data size %d\n", DataSize);

	timeout = 1000;
	setsockopt(iSocketFD, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));


	while(1) {
		recv_size = recv(iSocketFD, rxBuf+DataSize, 100000, 0);
		//printf("Data received %d\n", recv_size);
		if(recv_size <= 0) 
		{
			int error = WSAGetLastError();
			//printf("recv error %d\n", error);
			//printf("Rxed Lenth and said lenght %d %d\n",  DataSize,contentsize);
			if(DataSize < contentsize) TimeoutCount--;
			else break;
			if(TimeoutCount) continue;
			break;
		}
		DataSize+=recv_size;


	}

	if(DataSize != contentsize) return -1;
	rxBuf[DataSize]=0;
	//puts(rxBuf);
	printf("Rxed Content Length %d\n", DataSize);
	//*ppReturnBuf = rxBuf;

	return contentsize;
}

int GetWebFile(int iSocketFD, char *fileName, char *pszContent)
{
char szRequest[512];
int sentBytes, Bytes2Send;


	sprintf(szRequest, "GET /%s HTTP/1.1\r\nHost: %s\r\nConnection: Keep-alive\r\nAccept: text/html\r\n\r\n",
		fileName, g_szDBServer);

	Bytes2Send = strlen(szRequest);

	sentBytes = send( iSocketFD,  szRequest, Bytes2Send, 0 );

	if(sentBytes != Bytes2Send) {
		printf("Send failed %d\n", sentBytes);
		return -1;
	}

	 
	 sentBytes = readhttpresponse(iSocketFD,pszContent);



	return sentBytes;
}

void GetDBVersions(char *pszContent, int* pnNICVersion, int* pnORGVersion, int* pnCNTRYVersion, int* pnLocVersion)
{
char *pszTokenStart;
char szTempBuf[256];

	_strlwr(pszContent);

	pszTokenStart = strstr(pszContent, "nic");
	if(pszTokenStart) 
	{
		sscanf(pszTokenStart, "%s %d", &szTempBuf, pnNICVersion);
	}
	pszTokenStart = strstr(pszContent, "org");
	if(pszTokenStart) 
	{
		sscanf(pszTokenStart, "%s %d", &szTempBuf, pnORGVersion);
	}
	pszTokenStart = strstr(pszContent, "country");
	if(pszTokenStart) 
	{
		sscanf(pszTokenStart, "%s %d", &szTempBuf, pnCNTRYVersion);
	}
	pszTokenStart = strstr(pszContent, "location");
	if(pszTokenStart) 
	{
		sscanf(pszTokenStart, "%s %d", &szTempBuf, pnLocVersion);
	}


}

void GetDBfromClound()
{


		g_hGetDBThread = CreateThread(NULL, 0, 
		(LPTHREAD_START_ROUTINE)GetDBfromCloudThreadRoutine,
		0,  0,   &g_dwGetDBThreadID);


}


LONG GetDBfromCloudThreadRoutine(LPVOID lpv)
{

struct hostent *pheHost;
struct sockaddr_in sinSocket;
int iSocketFD = 0;
char *pszContent;
int ContentLength;
int nNICVersion, nORGVersion, nCNTRYVersion, nLocVersion;
bool bGetNICDB, bGetORGDB, bGetCNTRYDB, bGetLocDB;
char szFileName[1024];

	bGetNICDB=0;
	bGetORGDB=0;
	bGetCNTRYDB=0;
	bGetLocDB=0;
	
	pszContent = (char*) malloc(MAX_DB_BUF_SIZE);


	lstrcpy(g_szDBServer, INDIA_SERVER_NAME);
	pheHost = gethostbyname(g_szDBServer);
    if ( !pheHost ) {
		//OutputDebugString("Cant resolve host name\n");
		//perror(szWhoisServer);
		//getchar();
		return -1;
	}

    if ( (iSocketFD = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
      {
        //perror( "whois: unable to create socket");
        //exit( EXIT_FAILURE );
		  return -1;
      }

     sinSocket.sin_family = pheHost->h_addrtype;
     sinSocket.sin_port = htons(INDIA_PORT_NUMBER);
     memcpy( &sinSocket.sin_addr, pheHost->h_addr, pheHost->h_length);

    if ( connect(iSocketFD,(struct sockaddr *)&sinSocket,sizeof(sinSocket) )  < 0)
      {

		lstrcpy(g_szDBServer, US_SERVER_NAME);
		pheHost = gethostbyname(g_szDBServer);
		if ( !pheHost ) {
		//OutputDebugString("Cant resolve host name\n");
		//perror(szWhoisServer);
		//getchar();
			return -1;
		}
		sinSocket.sin_port = htons(US_PORT_NUMBER);
		memcpy( &sinSocket.sin_addr, pheHost->h_addr, pheHost->h_length);

		  if ( connect(iSocketFD,(struct sockaddr *)&sinSocket,sizeof(sinSocket) )  < 0)
		  {
			  return -1;
		  }
  
      }

	 ContentLength = GetWebFile(iSocketFD, "DBVersion.txt", pszContent);
	 if(ContentLength <= 0) return -1;

	  nNICVersion=0;
	  nORGVersion=0;
	  nCNTRYVersion=0;
	  nLocVersion=0;

	  GetDBVersions(pszContent, &nNICVersion, &nORGVersion, &nCNTRYVersion, &nLocVersion);

	  //bool bGetNICDB, bGetORGDB, bGetCNTRYDB, bGetLocDB;

	  if(nNICVersion >= 1000 && nNICVersion > g_nNICDBVersion) bGetNICDB=1;
	  if(nORGVersion >= 1000 && nORGVersion > g_nIP2ORGVersion) bGetORGDB=1;
	  if(nCNTRYVersion >= 1000 && nCNTRYVersion > g_nIP2CNTRYVersion) bGetCNTRYDB=1;
	  if(nLocVersion >= 1000 && nLocVersion > g_nIP2LocVersion) bGetLocDB=1;


	if(bGetNICDB)
	{
		ContentLength = GetWebFile(iSocketFD, "PF_NIC.txt", pszContent);

		lstrcpy(szFileName, g_szDbPath);
		lstrcat(szFileName, "PF_NIC.txt");
		if(ContentLength) 
		{
			if (0 == Write2File1(szFileName, pszContent))
				UpdateDBVersionNumber(NIC_DB, nNICVersion);
		}

	}

	if(bGetORGDB)
	{
		ContentLength = GetWebFile(iSocketFD, "PF_IPOrg.txt", pszContent);

		lstrcpy(szFileName, g_szDbPath);
		lstrcat(szFileName, "PF_IPOrg.txt");
		if(ContentLength) 
		{
			if(0 == Write2File1(szFileName, pszContent))
				UpdateDBVersionNumber(IPORG_DB, nORGVersion);
		}
	}
	if(bGetCNTRYDB)
	{
		ContentLength = GetWebFile(iSocketFD, "PF_IPCountry.txt", pszContent);

		lstrcpy(szFileName, g_szDbPath);
		lstrcat(szFileName, "PF_IPCountry.txt");
		if(ContentLength) 
		{
			if(0==Write2File1(szFileName, pszContent))
				UpdateDBVersionNumber(IPCTY_DB, nCNTRYVersion);
			
		}
	}
	if(bGetLocDB)
	{
		ContentLength = GetWebFile(iSocketFD, "PF_IPLocations.txt", pszContent);

		lstrcpy(szFileName, g_szDbPath);
		lstrcat(szFileName, "PF_IPLocations.txt");
		if(ContentLength) {
			if(0 == Write2File1(szFileName, pszContent))
				UpdateDBVersionNumber(IPLOC_DB, nLocVersion);
		}
	}

	if ( shutdown(iSocketFD, SD_SEND) == -1)
      {
        perror ("whois: shutdown failed");
      }

      /* Close socket */
      if ( closesocket(iSocketFD) != 0 )
      {
        perror ("whois: socket close failed");
      }




	return 0;
}