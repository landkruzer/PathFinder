typedef unsigned long   IPAddr;

//
// IP_STATUS codes returned from IP APIs
//

#define IP_STATUS_BASE              11000

#define IP_SUCCESS                  0
#define IP_BUF_TOO_SMALL            (IP_STATUS_BASE + 1)
#define IP_DEST_NET_UNREACHABLE     (IP_STATUS_BASE + 2)
#define IP_DEST_HOST_UNREACHABLE    (IP_STATUS_BASE + 3)
#define IP_DEST_PROT_UNREACHABLE    (IP_STATUS_BASE + 4)
#define IP_DEST_PORT_UNREACHABLE    (IP_STATUS_BASE + 5)
#define IP_NO_RESOURCES             (IP_STATUS_BASE + 6)
#define IP_BAD_OPTION               (IP_STATUS_BASE + 7)
#define IP_HW_ERROR                 (IP_STATUS_BASE + 8)
#define IP_PACKET_TOO_BIG           (IP_STATUS_BASE + 9)
#define IP_REQ_TIMED_OUT            (IP_STATUS_BASE + 10)
#define IP_BAD_REQ                  (IP_STATUS_BASE + 11)
#define IP_BAD_ROUTE                (IP_STATUS_BASE + 12)
#define IP_TTL_EXPIRED_TRANSIT      (IP_STATUS_BASE + 13)
#define IP_TTL_EXPIRED_REASSEM      (IP_STATUS_BASE + 14)
#define IP_PARAM_PROBLEM            (IP_STATUS_BASE + 15)
#define IP_SOURCE_QUENCH            (IP_STATUS_BASE + 16)
#define IP_OPTION_TOO_BIG           (IP_STATUS_BASE + 17)
#define IP_BAD_DESTINATION          (IP_STATUS_BASE + 18)


/*
struct ip_option_information {
    unsigned char      Ttl;             // Time To Live
    unsigned char      Tos;             // Type Of Service
    unsigned char      Flags;           // IP header flags
    unsigned char      OptionsSize;     // Size in bytes of options data
    unsigned char FAR *OptionsData;     // Pointer to options data
}; /* ip_option_information */
*/

//
// The icmp_echo_reply structure describes the data returned in response
// to an echo request.
//
/*
struct icmp_echo_reply {
    IPAddr                         Address;         // Replying address
    unsigned long                  Status;          // Reply IP_STATUS
    unsigned long                  RoundTripTime;   // RTT in milliseconds
    unsigned short                 DataSize;        // Reply data size in bytes
    unsigned short                 Reserved;        // Reserved for system use
    void FAR                      *Data;            // Pointer to the reply data
    struct ip_option_information   Options;         // Reply options
}; /* icmp_echo_reply */

*/

typedef struct ip_option_information IP_OPTION_INFORMATION,
                                     FAR *PIP_OPTION_INFORMATION;
typedef struct icmp_echo_reply ICMP_ECHO_REPLY,
                               FAR *PICMP_ECHO_REPLY;


typedef struct _MIB_IPADDRROW
{
    DWORD        dwAddr;
    DWORD        dwIndex;
    DWORD        dwMask;
    DWORD        dwBCastAddr;
    DWORD        dwReasmSize;
    unsigned short    unused1;
    unsigned short    wType;
} MIB_IPADDRROW, *PMIB_IPADDRROW;

typedef struct _MIB_IPADDRTABLE
{
    DWORD         dwNumEntries;
    MIB_IPADDRROW table[1];
} MIB_IPADDRTABLE, *PMIB_IPADDRTABLE;



typedef HANDLE (WINAPI *ICMPCREATEFILE)();
typedef DWORD (WINAPI *ICMPSENDECHO)(HANDLE IcmpHandle, 
IPAddr DestinationAddress,
LPVOID RequestData, 
WORD RequestSize, 
PIP_OPTION_INFORMATION RequestOptions, 
LPVOID ReplyBuffer,
DWORD ReplySize, 
DWORD Timeout );
typedef BOOL (WINAPI *ICMPCLOSEHANDLE)(HANDLE IcmpHandle );

typedef DWORD (WINAPI *GETBESTINTERFACE)(
  IPAddr dwDestAddr,     // destination IP address
  PDWORD pdwBestIfIndex  // index of interface with the best route
);

typedef DWORD (WINAPI *GETIPADDRTABLE) (
  PMIB_IPADDRTABLE pIpAddrTable,
  PULONG pdwSize,
  BOOL bOrder
);
