#define UM_INSERT_NORMALROW WM_USER+100
#define UM_INSERT_TOUTROW WM_USER+101
#define UM_INSERT_LASTROW WM_USER+102
#define UM_ADD_NAME_LOC_ISP WM_USER+103
#define UM_ADD_NAME WM_USER+104

#define	MAX_RDNSLIST 2048

// Define it in GetIP details file also
#define UNKNOWN_REGISTRY 0
#define APNIC_DB 1
#define ARIN_DB 2
#define RIPE_DB 3
#define LACNIC_DB 4
#define AFRINIC_DB 5
#define KRNIC_DB 6
#define BRNIC_DB 7
#define JPNIC_DB 8


typedef unsigned long   IPAddr;

union uniIPAddr
{
	struct
	{
		BYTE Addr1,Addr2,Addr3,Addr4;
	}ByteValue;
	DWORD Adddress;
};


struct THREADBUF
{
HWND hViewWnd; 
IPAddr ipNode;
int nIndex;
} ;

struct LOCATIONTBL{
IPAddr Address;
int Bits;
char strLocation[64];
};

struct RegDB {
IPAddr Address;
int Bits;
int Registry;
};

struct INDIAN_IP {
IPAddr ipNetwork;
int nBits;
char szCompany[256];
};

struct IP2LOCATION {
IPAddr ipNetwork;
int nBits;
int nCountryCode;
};

struct STATICIPDATA
{
	char szIP[32];
	char szName[256];
};

struct COUNTRYLIST
{
	int nCountryCode;
	char szCC[3];
	char szName[256];
};

struct PathData
{
	int nIndex;
	IPAddr Address;
	int nRTT;
	int nCode;
};

struct RDNS
{
	IPAddr ipNodeAddress;
	char szName[256];
	struct RDNS *pNext, *pPrev;
};

struct RDNSThreadData 
{
	IPAddr ipNodeAddress;
	char *szName;
};

struct NODEINFO
{
	int Index;
	char szNodeName[256];
	char szCountryName[256];
	char szLocation[256];
	char szISPName[256];
};

struct IPNETWORK
{

	IPAddr ipNetwork;
	int nPrefix;
};


void Write2File(char *pszFileName, char *pszData);
void LoadHelperDLLs();

int LoadIPLocationTable(char *szFilePath);
int LoadRegistryDb(char *szFilePath);
int LoadIPOrgTable(char *szFilePath);
int LoadCountryCode(char *szFilePath);
int LoadIP2CountryCode(char *szFilePath);
void SplitAddressnName(char *Line, IPAddr *Addr, int *Bits, char *Name, bool NameIncludesSpace);
void SplitAddressnCC(char *Line, IPAddr *Addr, int *Bits, char *Name);
void SplitAddress_CC_Org(char *szLine, IPAddr *Addr, int *Bits, char *pszCC, char *pszOrg);
void SplitIPnConvert(char *szLine, IPAddr *Addr, int *Bits);

IPAddr MasktheIP(IPAddr Addr, int bits);

LONG TraceThreadRoutine(LPVOID lpv);
LONG LookupThreadRoutine(LPVOID lpv);
LONG RDNSThreadRoutine(LPVOID lpv);


IPAddr GetSourceIP(IPAddr DestinationIP);
int StartTracing(char *szDestBuf, HWND hViewWnd, DWORD *pIPaddress);
void NTOA(IPAddr Address, char *Buffer);
BOOL GetRDNSfromList(IPAddr ipNodeAddress, char *pszNodeName);
void Add2RDNSList(IPAddr ipNodeAddress, char *pszNodeName);
void GetLocationandISPName(IPAddr ipNodeAddress, char *pszCountryName, char *pszLocation, char *pszISPName);
BOOL GettheCountryfromLocalCache(IPAddr ipNodeAddress, char *pszLocation,char *pszISPName);

BOOL GetCountryCodefromRegistry(IPAddr ipNodeAddress, struct IPNETWORK **ppNet, int *pnItems, char *pszCnryCode, char *pszISPName);

void GetCountryName(char *pszCnryCode, char *pszLocation, bool addprefix);
void GetCountryNamebyCode(int CC, char *pszCountry);
//void AddNetnCC2LocalCache(IPAddr ipNetwork, int nPrefix, char *pszCnryCode);
void AddNetnCC2LocalCache(struct IPNETWORK *pNet, int nItems, char *pszCnryCode, char *pszISPName);

int IPCompare(IPAddr ipNetwork, int nPrefix, IPAddr ipNodeAddress);
int GetRegistry(IPAddr ipAddress);
char *GetIPDetails(int nRegistry, char *pszAddress);
int GetCCnNetwork(char *pszWhois, int nRegistry, struct IPNETWORK **ppNet, int *pnItems, char *pszCnryCode, char *pszOrgName);
int GetCCnNetworkfromJP(char *pszWhois, int nRegistry, struct IPNETWORK **ppNet, int *pnItems, char *pszCnryCode, char *pszOrgName);

void CIDRList2Network(char *pszCIDRList, struct IPNETWORK **ppNet, int *pnItems);
void INetNum2Network(char *pszInetNum, struct IPNETWORK **ppNet, int *pnItems);



