
#include "pch.h"
#include <string.h>
# include <winsock2.h>

#define APNIC_DB 1
#define ARIN_DB 2
#define RIPE_DB 3
#define LACNIC_DB 4
#define AFRINIC_DB 5
#define KRNIC_DB 6
#define BRNIC_DB 7
#define JPNIC_DB 8


/* Macro definitions */
/* MIN and MAX get defined vis <sys/types.h> on Unix */
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

int iReadResponse (int iSocketFD, char **ppReturnBuf);

char *GetIPDetails(int Server, char *Address)
{

char szWhoisServer[256];
struct hostent *pheHost;
int iSocketFD = 0;
struct sockaddr_in sinSocket;
const short int kiWhoisPortNumber = 43; /* Standard WHOIS service port number */
char *pszRequestBuffer = NULL;
//char *pszDomainToFind = NULL;
char *pszResultString = NULL;


	switch(Server)
	{
	case APNIC_DB:
		strcpy(szWhoisServer, "whois.apnic.net");
		break;
	case ARIN_DB:
		strcpy(szWhoisServer, "whois.arin.net");
		break;
	case RIPE_DB:
		strcpy(szWhoisServer, "whois.ripe.net");
		break;
	case LACNIC_DB:
		strcpy(szWhoisServer, "whois.lacnic.net");
		break;
	case AFRINIC_DB:
		strcpy(szWhoisServer, "whois.afrinic.net");
		break;
	case BRNIC_DB:
		strcpy(szWhoisServer, "whois.registro.br");
		break;
	case JPNIC_DB:
		strcpy(szWhoisServer, "whois.nic.ad.jp");
		break;
	}


	pheHost = gethostbyname(szWhoisServer);
    if ( !pheHost ) return NULL;


      if ( (iSocketFD = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
      {
        //perror( "whois: unable to create socket");
        //exit( EXIT_FAILURE );
		  return NULL;
      }

      /* Fill in the name & address structure for remote server socket */
      sinSocket.sin_family = pheHost->h_addrtype;
      sinSocket.sin_port = htons(kiWhoisPortNumber);
      memcpy( &sinSocket.sin_addr, pheHost->h_addr, pheHost->h_length);


      /* Connect to whois server */
      if ( connect(iSocketFD,(struct sockaddr *)&sinSocket,sizeof(sinSocket) )  < 0)
      {
        //perror("whois: unable to connect to WHOIS server");
        //perror(pszWhoisServer);
        //exit( EXIT_FAILURE );
		  return NULL;
      }

      /* Allocate buffer to hold request string */
      pszRequestBuffer = (char*)calloc(strlen(Address)+strlen("\r\n")+3,1);

	  strcat(pszRequestBuffer, Address);
	  if(JPNIC_DB == Server) strcat(pszRequestBuffer, "/e");
      strcat(pszRequestBuffer, "\r\n");
      send( iSocketFD,  pszRequestBuffer, strlen(pszRequestBuffer), 0 );
      /* Free the request buffer and NULL out the pointer */
      free(pszRequestBuffer);
      pszRequestBuffer = NULL;

      /* Read in result string */
      if ( iReadResponse(iSocketFD,&pszResultString) > 0 )
      {
			//OutputDebugString(pszResultString);
      }
      else
      {
        pszResultString = NULL;
      }

      /* Shut down socket gracefully (other end should have done this already) */
      if ( shutdown(iSocketFD, SD_SEND) == -1)
      {
        perror ("whois: shutdown failed");
      }

      /* Close socket */
      if ( closesocket(iSocketFD) != 0 )
      {
        perror ("whois: socket close failed");
      }

	  return pszResultString;

}

/*---- Read from socket, return data in buffer, caller's job to free buffer --*/
int
iReadResponse (int iSocketFD, char **ppReturnBuf)
{
  const int ciMaxByteSize = 65535;
  char *pBuf = NULL;
  char *pTmp = NULL;
  int iStatus = 0;
  int iCount = 0; /* Bytes */
  int iTotal = 0; /* Bytes */
  int iEndOfResponse = 0;
  int iFlag = 0;      /* 0 or MSG_OOB */
  int iBufSize = 1024;
  fd_set rmask;
  fd_set wmask;
  fd_set emask;
  struct timeval tval;
  /* End of declarations ... */
  
  pBuf = (char*)calloc (iBufSize, 1);
  
  while (!iEndOfResponse)
  {
    FD_ZERO (&rmask);
    FD_ZERO (&wmask);
    FD_ZERO (&emask);
    FD_SET (iSocketFD, &rmask);
    FD_SET (iSocketFD, &emask);
    
    tval.tv_sec = 20;       /* 20 second timeout */
    tval.tv_usec = 0;
    
    /* Use select to spot incoming data and allow for timeout */
    iStatus = select (iSocketFD + 1, &rmask, &wmask, &emask, &tval);
    
    if (iStatus <= 0 || FD_ISSET (iSocketFD, &emask) || !FD_ISSET (iSocketFD, &rmask))
    {
      iEndOfResponse = 1;
      iTotal = -1;
      if (iStatus == 0)
      {
        perror("whois: timeout waiting for response from server");
      }
      
    }
    else
    {
      /* Got something to read */
      iCount = recv (iSocketFD, &pBuf[iTotal], MIN (ciMaxByteSize, (iBufSize - iTotal)), iFlag);
      if (iCount < 0)
      {
        /* Error or result of remote server closing socket after sending data */
        iEndOfResponse = 1;
      }
      else if (iCount == 0)
      {
        /* Remote server broke connection, often done after data is all sent */
        iEndOfResponse = 1;
      }
      else
      {
        iTotal += iCount;
        if (iTotal >= iBufSize)
        {
          pTmp = (char*)realloc (pBuf, iTotal + iBufSize / 2);
          if (pTmp)
          {
            pBuf = pTmp;
            /* Clear new storage space */
            (void)memset (&pBuf[iTotal], 0, iBufSize / 2);
          }
          
          iBufSize = iTotal + iBufSize / 2;
        }
        
      }
      
    }
    
  }
  
  if (iTotal >= 0)
  {
    *ppReturnBuf = pBuf;
  }
  else
  {
    free (pBuf);
    *ppReturnBuf = NULL;
  }
  
  return (iTotal);
}
