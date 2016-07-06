
#include "linux/autoconf.h"

#define NDIS_802_11_LENGTH_SSID         32
#define NDIS_802_11_LENGTH_RATES        8
#define NDIS_802_11_LENGTH_RATES_EX     16

#define WPA_OUI_TYPE        0x01F25000
#define WPA_OUI             0x00F25000
#define WPA_OUI_1           0x00030000
#define WPA2_OUI            0x00AC0F00
#define CISCO_OUI           0x00964000

#define PACKED __attribute__ ((packed))


#if WIRELESS_EXT <= 11
#ifndef SIOCDEVPRIVATE
#define SIOCDEVPRIVATE				0x8BE0
#endif
#define SIOCIWFIRSTPRIV				SIOCDEVPRIVATE
#endif

#ifndef AP_MODE
#define RT_PRIV_IOCTL				(SIOCIWFIRSTPRIV + 0x0E)
#else
#define RT_PRIV_IOCTL				(SIOCIWFIRSTPRIV + 0x01)
#endif	/* AP_MODE */

#define OID_GET_SET_TOGGLE					0x8000

#define OID_802_11_BSSID_LIST_SCAN                  0x0508
#define OID_802_11_SSID                             0x0509
#define OID_802_11_BSSID                            0x050A
#define RT_OID_802_11_RADIO                         0x050B

#define OID_802_11_BSSID_LIST                       0x0609

#define OID_GEN_MEDIA_CONNECT_STATUS                0x060B

#define RT_OID_WE_VERSION_COMPILED                  0x0622


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

//#define SIOCIWFIRSTPRIV				0x8BE0
//#define RT_PRIV_IOCTL				(SIOCIWFIRSTPRIV + 0x01)
#define OID_SITE_SURVEY_LIST		0x0999   
char    G_bRadio = 1; //TRUE
#define NdisMediaStateConnected                 1
#define NdisMediaStateDisconnected              0

int G_ConnectStatus = NdisMediaStateDisconnected;

typedef struct _SiteSurvey
{
	//unsigned int  SsidLength;
	unsigned char bChannel;
	unsigned char SSID[32];
	unsigned char Auth[32];
	unsigned char Encry[32];
	unsigned char Infr[32];
}SiteSurvey, *PSiteSurvey;

typedef struct PACKED _NDIS_802_11_SSID
{
	unsigned int    SsidLength;   // length of SSID field below, in bytes;
                                  // this can be zero.
	unsigned char   Ssid[NDIS_802_11_LENGTH_SSID]; // SSID information field
} NDIS_802_11_SSID, *PNDIS_802_11_SSID;

// Received Signal Strength Indication
typedef long NDIS_802_11_RSSI; 

// Added new types for OFDM 5G and 2.4G
typedef enum _NDIS_802_11_NETWORK_TYPE
{
	Ndis802_11FH,
	Ndis802_11DS,
	Ndis802_11OFDM5,
	Ndis802_11OFDM24,
	Ndis802_11Automode,
	Ndis802_11OFDM5_N,
	Ndis802_11OFDM24_N,
	Ndis802_11NetworkTypeMax    // not a real type, defined as an upper bound
} NDIS_802_11_NETWORK_TYPE, *PNDIS_802_11_NETWORK_TYPE;

typedef enum _NDIS_802_11_NETWORK_INFRASTRUCTURE
{
	Ndis802_11IBSS,
	Ndis802_11Infrastructure,
	Ndis802_11AutoUnknown,
	Ndis802_11Monitor,
	Ndis802_11InfrastructureMax         // Not a real value, defined as upper bound
} NDIS_802_11_NETWORK_INFRASTRUCTURE, *PNDIS_802_11_NETWORK_INFRASTRUCTURE;

typedef unsigned char  NDIS_802_11_RATES[NDIS_802_11_LENGTH_RATES];        // Set of 8 data rates
typedef unsigned char  NDIS_802_11_RATES_EX[NDIS_802_11_LENGTH_RATES_EX];  // Set of 16 data rates

typedef struct _NDIS_802_11_CONFIGURATION_FH
{
	unsigned long   Length;             // Length of structure
	unsigned long   HopPattern;         // As defined by 802.11, MSB set
	unsigned long   HopSet;             // to one if non-802.11
	unsigned long   DwellTime;          // units are Kusec
} NDIS_802_11_CONFIGURATION_FH, *PNDIS_802_11_CONFIGURATION_FH;

typedef struct _NDIS_802_11_CONFIGURATION
{
	unsigned long   Length;             // Length of structure
	unsigned long   BeaconPeriod;       // units are Kusec
	unsigned long   ATIMWindow;         // units are Kusec
	unsigned long   DSConfig;           // Frequency, units are kHz
	NDIS_802_11_CONFIGURATION_FH    FHConfig;
} NDIS_802_11_CONFIGURATION, *PNDIS_802_11_CONFIGURATION;


// Added Capabilities, IELength and IEs for each BSSID
typedef struct PACKED _NDIS_WLAN_BSSID_EX
{
	unsigned long                       Length;             // Length of this structure
	unsigned char                       MacAddress[6];      // BSSID
	unsigned char                       Reserved[2];
	NDIS_802_11_SSID                    Ssid;               // SSID
	unsigned int						Privacy;            // WEP encryption requirement
	NDIS_802_11_RSSI                    Rssi;               // receive signal
                                                            // strength in dBm
	NDIS_802_11_NETWORK_TYPE            NetworkTypeInUse;
	NDIS_802_11_CONFIGURATION           Configuration;
	NDIS_802_11_NETWORK_INFRASTRUCTURE  InfrastructureMode;
	NDIS_802_11_RATES_EX                SupportedRates;
	unsigned long                       IELength;
	unsigned char                       IEs[1];
} NDIS_WLAN_BSSID_EX, *PNDIS_WLAN_BSSID_EX;

typedef struct PACKED _NDIS_WLAN_BSSID
{
	unsigned long                       Length;             // Length of this structure
	unsigned char                       MacAddress[6];      // BSSID
	unsigned char                       Reserved[2];
	NDIS_802_11_SSID                    Ssid;               // SSID
	unsigned long                       Privacy;            // WEP encryption requirement
	NDIS_802_11_RSSI                    Rssi;               // receive signal
                                                            // strength in dBm
	NDIS_802_11_NETWORK_TYPE            NetworkTypeInUse;
	NDIS_802_11_CONFIGURATION           Configuration;
	NDIS_802_11_NETWORK_INFRASTRUCTURE  InfrastructureMode;
	NDIS_802_11_RATES                   SupportedRates;
} NDIS_WLAN_BSSID, *PNDIS_WLAN_BSSID;


typedef struct PACKED _NDIS_802_11_BSSID_LIST_EX
{
	unsigned int            NumberOfItems;      // in list below, at least 1
	NDIS_WLAN_BSSID_EX      Bssid[1];
} NDIS_802_11_BSSID_LIST_EX, *PNDIS_802_11_BSSID_LIST_EX;

typedef struct PACKED _NDIS_802_11_FIXED_IEs
{
	unsigned char   Timestamp[8];
	unsigned short  BeaconInterval;
	unsigned short  Capabilities;
} NDIS_802_11_FIXED_IEs, *PNDIS_802_11_FIXED_IEs;

typedef struct PACKED _NDIS_802_11_VARIABLE_IEs
{
	unsigned char   ElementID;
	unsigned char   Length;        // Number of bytes in data field
	unsigned char   data[1];
} NDIS_802_11_VARIABLE_IEs, *PNDIS_802_11_VARIABLE_IEs;



