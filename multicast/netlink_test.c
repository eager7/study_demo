#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/socket.h>

#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/wireless.h>
#include <linux/autoconf.h>  //kernel config
#include <dirent.h>
#include <errno.h>

#include "easypriv.h"

#define NETLINK_TEST 17
#define MAX_PAYLOAD 1024    /* maximum payload size*/

#define NET_LINK_CHANNEL	0x01
#define NET_LINK_SSIDPWD	0x02
#define NET_LINK_REIPPORT	0x03

#if 0
typedef struct EasyLink
{	
	UCHAR 	uSSIDlen;
	UCHAR 	uPwdLen;
	UCHAR 	uChannel;   
	UCHAR 	uLinkType;  //发送LINT类型:NET_LINK_CHANNEL,NET_LINK_SSIDPWD
	UINT32 	uRecOK;
	USHORT 	uPort;
	UINT32	uIP;
	UCHAR 	uPwd[32];
	UCHAR 	uSSID[32];
}TEasyLink
#endif

typedef struct EasyLink
{
	unsigned char 	uSSIDlen;
	unsigned char 	uPwdLen;
	unsigned char 	uChannel;   
	unsigned char 	uLinkType;  //发送LINT类型:NET_LINK_CHANNEL,NET_LINK_SSIDPWD
	unsigned int  	uRecOK;
	unsigned short 	uPort;
	unsigned int  	uIP;
	unsigned char 	uPwd[32];
	unsigned char 	uSSID[32];
}TEasyLink;

struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
int sock_fd;
int recflag = -1;
struct msghdr msg;
int ret = 0;

void ConverterStringToDisplay(char *str)
{
    int  len, i;
    char buffer[193];
    char *pOut;

    memset(buffer,0,193);
    len = strlen(str);
    pOut = &buffer[0];

    for (i = 0; i < len; i++) {
		switch (str[i]) {
			case 38:
				strcpy (pOut, "&amp;");
				pOut += 5;
				break;

			case 60: 
				strcpy (pOut, "&lt;");
				pOut += 4;
				break;

			case 62: 
				strcpy (pOut, "&gt;");
				pOut += 4;
				break;

			case 34:
				strcpy (pOut, "&#34;");
				pOut += 5;
				break;

			case 39:
				strcpy (pOut, "&#39;");
				pOut += 5;
				break;
			case 32:
				strcpy (pOut, "&nbsp;");
				pOut += 6;
				break;

			default:
				if ((str[i]>=0) && (str[i]<=31)) {
					//Device Control Characters
					sprintf(pOut, "&#%02d;", str[i]);
					pOut += 5;
				} else if ((str[i]==39) || (str[i]==47) || (str[i]==59) || (str[i]==92)) {
					// ' / ; (backslash)
					sprintf(pOut, "&#%02d;", str[i]);
					pOut += 5;
				} else if (str[i]>=127) {
					//Device Control Characters
					sprintf(pOut, "&#%03d;", str[i]);
					pOut += 6;
				} else {
					*pOut = str[i];
					pOut++;
				}
				break;
		}
    }
    *pOut = '\0';
    strcpy(str, buffer);
}

int OidQueryInformation(unsigned long OidQueryCode, int socket_id, char *DeviceName, void *ptr, unsigned long PtrLength)
{
	struct iwreq wrq;

	strcpy(wrq.ifr_name, DeviceName);
	wrq.u.data.length = PtrLength;
	wrq.u.data.pointer = (caddr_t) ptr;
	wrq.u.data.flags = OidQueryCode;

	return (ioctl(socket_id, RT_PRIV_IOCTL, &wrq));
}
int OidSetInformation(unsigned long OidQueryCode, int socket_id, char *DeviceName, void *ptr, unsigned long PtrLength)
{
	struct iwreq wrq;

	strcpy(wrq.ifr_name, DeviceName);
	wrq.u.data.length = PtrLength;
	wrq.u.data.pointer = (caddr_t) ptr;
	wrq.u.data.flags = OidQueryCode | OID_GET_SET_TOGGLE;

	return (ioctl(socket_id, RT_PRIV_IOCTL, &wrq));
}
unsigned int Sleep(unsigned int secs)
{
	int rc;
	struct timespec ts, remain;
	ts.tv_sec  = secs;
	ts.tv_nsec = 0;

sleep_again:
	rc = nanosleep(&ts, &remain);
	if(rc == -1 && errno == EINTR){
		ts.tv_sec = remain.tv_sec;
		ts.tv_nsec = remain.tv_nsec;
		goto sleep_again;
	}	
	return 0;
}
int getsitesurvey(SiteSurvey APList[])
{
	int                         s, ret, retry=1;
	unsigned int                lBufLen = 4096, we_version=16; // 64K
	PNDIS_802_11_BSSID_LIST_EX	pBssidList;
	PNDIS_WLAN_BSSID_EX  		pBssid;
	unsigned int                ConnectStatus = NdisMediaStateDisconnected;
	unsigned char               BssidQuery[6];
	NDIS_802_11_SSID            SSIDQuery;
	int							QueryCount=0, EAGAIN_Count=0;
	int num=0;

	//SiteSurvey	APList[32];

	s = socket(AF_INET, SOCK_DGRAM, 0);
	pBssidList = (PNDIS_802_11_BSSID_LIST_EX) malloc(65536*2);  //64k
	memset(pBssidList, 0x00, sizeof(char)*65536*2);
	
	//step 1
	while(ConnectStatus != NdisMediaStateConnected && QueryCount < 3) {
		if (OidQueryInformation(OID_GEN_MEDIA_CONNECT_STATUS, s, "ra0", &ConnectStatus, sizeof(ConnectStatus)) < 0) {
			printf("Query OID_GEN_MEDIA_CONNECT_STATUS failed!\n");
			free(pBssidList); close(s);
			return -1;
		}
		sleep(2);
		QueryCount++;
	}

	//step 2
	if (OidQueryInformation(RT_OID_802_11_RADIO, s, "ra0", &G_bRadio, sizeof(G_bRadio)) < 0) {
		printf("Query RT_OID_802_11_RADIO failed!\n");
		free(pBssidList); close(s);
		return -1;
	}

	if (ConnectStatus == NdisMediaStateConnected && G_bRadio) {
		memset(&BssidQuery, 0x00, sizeof(BssidQuery));
		OidQueryInformation(OID_802_11_BSSID, s, "ra0", &BssidQuery, sizeof(BssidQuery));
		memset(&SSIDQuery, 0x00, sizeof(SSIDQuery));
		OidQueryInformation(OID_802_11_SSID, s, "ra0", &SSIDQuery, sizeof(SSIDQuery));
	}

	//step 3
	if (OidSetInformation(OID_802_11_BSSID_LIST_SCAN, s, "ra0", 0, 0) < 0) {
		printf("Set OID_802_11_BSSID_LIST_SCAN failed!\n");
		free(pBssidList); close(s);
		return -1;
	}
	// wait a few seconds to get all AP.
	Sleep(2);

	//step 4
	ret = OidQueryInformation(RT_OID_WE_VERSION_COMPILED, s, "ra0", &we_version, sizeof(we_version));
	if (ret< 0)
	{
		printf("Query RT_OID_WE_VERSION_COMPILED error! return=%d\n", ret);
		free(pBssidList);
		close(s);
		return -1;
	}
	if(we_version >= 17)
		lBufLen=8192;
	else
		lBufLen=4096;

	ret = -1;
	retry = 1;
	while (ret < 0) {
		memset(pBssidList, 0x00, sizeof(char)*65536*2);
		
		//printf("GetSSIDS before OidQueryInformation-1\r\n");
		ret = OidQueryInformation(OID_802_11_BSSID_LIST, s, "ra0", pBssidList, lBufLen);
		//printf("GetSSIDS after  OidQueryInformation-2\r\n");
		
		if (errno == EAGAIN) {
			sleep(1);
			// fprintf(stderr, "errno == EAGAIN\n");
			EAGAIN_Count++;
			if(EAGAIN_Count>25) {
				printf("Query OID_802_11_BSSID_LIST error! errno == EAGAIN\n");
				free(pBssidList);
				close(s);
				return -1;
			}
			else
				continue;
		} else if (errno == E2BIG) {
			// fprintf(stderr, "errno == E2BIG\n");
			// lBufLen = lBufLen + 4096*retry;
			if ((lBufLen * 2) > 65535)
				lBufLen = 65535;
			else
				lBufLen *= 2;
			if (lBufLen < 65536 && retry <= 20) {
				retry++;
				// fprintf(stderr,"lBufLen=%d\n",lBufLen);
				continue;
			} else {
				printf("Query OID_802_11_BSSID_LIST error! E2BIG\n");
				free(pBssidList);
				close(s);
				return -1;
			}
		}
		else if( ret != 0 ) {
			printf("Query OID_802_11_BSSID_LIST error! return=%d\n", ret);
			free(pBssidList);
			close(s);
			return -1;
		}
	}
	if ( pBssidList->NumberOfItems == 0) {
		free(pBssidList);
		close(s);
		printf("Bssid List number is 0!\n");
		return 0;
	} else {

		//unsigned char tmpSSIDII[32];
		int i=0, j=0;

		pBssid = (PNDIS_WLAN_BSSID_EX) pBssidList->Bssid;
		G_ConnectStatus = NdisMediaStateDisconnected;

		num = pBssidList->NumberOfItems;
		//printf("%d\n",num);
		for (i = 0; i < pBssidList->NumberOfItems; i++)
		{
			//printf("----%d----\n",i);
			
			//memset(APList[i].SsidLength, 0x00, sizeof(APList[i].SsidLength));
			memset(APList[i].SSID, 0x00, sizeof(APList[i].SSID));
			memset(APList[i].Auth, 0x00, sizeof(APList[i].Auth));
			memset(APList[i].Encry, 0x00, sizeof(APList[i].Encry));
			memset(APList[i].Infr, 0x00, sizeof(APList[i].Infr));

			strncpy((char *)(APList[i].SSID), (char *)pBssid->Ssid.Ssid, pBssid->Ssid.SsidLength);
			//printf("sid->Ssid.Ssid=%s\n", pBssid->Ssid.Ssid);
			ConverterStringToDisplay((char *)(APList[i].SSID));
			//strcpy((char *)(APList[i].SsidLength), (char *)pBssid->Ssid.SsidLength);
//			printf("[%d]APList.SSID=%s\n",__LINE__, APList[i].SSID);

			unsigned char bTKIP = FALSE;
			unsigned char bAESWRAP = FALSE;
			unsigned char bAESCCMP = FALSE;
			unsigned char bWPA = FALSE;
			unsigned char bWPAPSK = FALSE;
			unsigned char bWPANONE = FALSE;
			unsigned char bWPA2 = FALSE;
			unsigned char bWPA2PSK = FALSE;
			unsigned char bWPA2NONE = FALSE;
			unsigned char bCCKM = FALSE; // CCKM for Cisco, add by candy 2006.11.24

			if ((pBssid->Length > sizeof(NDIS_WLAN_BSSID)) && (pBssid->IELength > sizeof(NDIS_802_11_FIXED_IEs)))
			{
				unsigned int lIELoc = 0;
				PNDIS_802_11_FIXED_IEs pFixIE = (PNDIS_802_11_FIXED_IEs)pBssid->IEs;
				PNDIS_802_11_VARIABLE_IEs pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pFixIE + sizeof(NDIS_802_11_FIXED_IEs));
				lIELoc += sizeof(NDIS_802_11_FIXED_IEs);
				
				while (pBssid->IELength > (lIELoc + sizeof(NDIS_802_11_VARIABLE_IEs)))
				{
					if ((pVarIE->ElementID == 221) && (pVarIE->Length >= 16))
					{
						unsigned int* pOUI = (unsigned int*)((char*)pVarIE->data);
						if (*pOUI != WPA_OUI_TYPE)
						{
							lIELoc += pVarIE->Length;
							lIELoc += 2;
							pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pVarIE + pVarIE->Length + 2);

							if(pVarIE->Length <= 0)
								break;

							continue;
						}

						unsigned int* plGroupKey; 
						unsigned short* pdPairKeyCount;
						unsigned int* plPairwiseKey=NULL;
						unsigned int* plAuthenKey=NULL;
						unsigned short* pdAuthenKeyCount;
						plGroupKey = (unsigned int*)((char*)pVarIE + 8);
				
						unsigned int lGroupKey = *plGroupKey & 0x00ffffff;
						//fprintf(stderr, "lGroupKey=%d\n", lGroupKey);
						if (lGroupKey == WPA_OUI) {
							lGroupKey = (*plGroupKey & 0xff000000) >> 0x18;
							if (lGroupKey == 2)
								bTKIP = TRUE;
							else if (lGroupKey == 3)
								bAESWRAP = TRUE;
							else if (lGroupKey == 4)
								bAESCCMP = TRUE;
						}
						else
						{
							lIELoc += pVarIE->Length;
							lIELoc += 2;
							pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pVarIE + pVarIE->Length + 2);

							if(pVarIE->Length <= 0)
								break;
							
							continue;
						}
				
						pdPairKeyCount = (unsigned short*)((char*)plGroupKey + 4);
						plPairwiseKey = (unsigned int*) ((char*)pdPairKeyCount + 2);
						unsigned short k = 0;
						for (k = 0; k < *pdPairKeyCount; k++) {
							unsigned int lPairKey = *plPairwiseKey & 0x00ffffff;
							if (lPairKey == WPA_OUI )//|| (lPairKey & 0xffffff00) == WPA_OUI_1)
							{
								lPairKey = (*plPairwiseKey & 0xff000000) >> 0x18;
								if (lPairKey == 2)
									bTKIP = TRUE;
								else if (lPairKey == 3)
									bAESWRAP = TRUE;
								else if (lPairKey == 4)
									bAESCCMP = TRUE;
							}
							else
								break;
							
							plPairwiseKey++;
						}
				
						pdAuthenKeyCount = (unsigned short*)((char*)pdPairKeyCount + 2 + 4 * (*pdPairKeyCount));
						plAuthenKey = (unsigned int*)((char*)pdAuthenKeyCount + 2);

						for(k = 0; k < *pdAuthenKeyCount; k++)
						{
							unsigned int lAuthenKey = *plAuthenKey & 0x00ffffff;
							if (lAuthenKey == CISCO_OUI) {
								bCCKM = TRUE; // CCKM for Cisco
							}
							else if (lAuthenKey == WPA_OUI) {
								lAuthenKey = (*plAuthenKey & 0xff000000) >> 0x18;

								if (lAuthenKey == 1)
									bWPA = TRUE;
								else if (lAuthenKey == 0 || lAuthenKey == 2) {
									if (pBssid->InfrastructureMode)
										bWPAPSK = TRUE;
									else
										bWPANONE = TRUE;
								}
							}					
							plAuthenKey++;
						}
					//break;
					}
					else if (pVarIE->ElementID == 48 && pVarIE->Length >= 12)
					{
						unsigned int* plGroupKey; 
						unsigned int* plPairwiseKey; 
						unsigned short* pdPairKeyCount;
						unsigned int* plAuthenKey; 
						unsigned short* pdAuthenKeyCount;
						plGroupKey = (unsigned int*)((char*)pVarIE + 4);

						unsigned int lGroupKey = *plGroupKey & 0x00ffffff;
						if (lGroupKey == WPA2_OUI) {
							lGroupKey = (*plGroupKey & 0xff000000) >> 0x18;
							if (lGroupKey == 2)
								bTKIP = TRUE;
							else if (lGroupKey == 3)
								bAESWRAP = TRUE;
							else if (lGroupKey == 4)
								bAESCCMP = TRUE;
						}
						else
						{
							lIELoc += pVarIE->Length;
							lIELoc += 2;
							pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pVarIE + pVarIE->Length + 2);

							if(pVarIE->Length <= 0)
								break;
							
							continue;
						}

						pdPairKeyCount = (unsigned short*)((char*)plGroupKey + 4);
						plPairwiseKey = (unsigned int*)((char*)pdPairKeyCount + 2);
						unsigned short k = 0;

						for (k = 0; k < *pdPairKeyCount; k++)
						{
							unsigned int lPairKey = *plPairwiseKey & 0x00ffffff;
							if (lPairKey == WPA2_OUI) {
								lPairKey = (*plPairwiseKey & 0xff000000) >> 0x18;
								if (lPairKey == 2)
									bTKIP = TRUE;
								else if (lPairKey == 3)
									bAESWRAP = TRUE;
								else if (lPairKey == 4)
									bAESCCMP = TRUE;
							}
							else
								break;
							plPairwiseKey++;
						}
			
						pdAuthenKeyCount = (unsigned short*)((char*)pdPairKeyCount + 2 + 4 * *pdPairKeyCount);
						plAuthenKey = (unsigned int*)((char*)pdAuthenKeyCount + 2);
						for (k = 0; k < *pdAuthenKeyCount; k++)
						{
							unsigned int lAuthenKey = *plAuthenKey & 0x00ffffff;
							if (lAuthenKey == CISCO_OUI) {
								bCCKM = TRUE; // CCKM for Cisco
							}
	                        else if (lAuthenKey == WPA2_OUI) {
								lAuthenKey = (*plAuthenKey & 0xff000000) >> 0x18;
								if (lAuthenKey == 1)
									bWPA2 = TRUE;
								else if (lAuthenKey == 0 || lAuthenKey == 2) {
									if (pBssid->InfrastructureMode)
										bWPA2PSK = TRUE;
									else
										bWPA2NONE = TRUE;
								}
							}					
							plAuthenKey++;
						}
					}
					lIELoc += pVarIE->Length;
					lIELoc += 2;
					pVarIE = (PNDIS_802_11_VARIABLE_IEs)((char*)pVarIE + pVarIE->Length + 2);

					if (pVarIE->Length <= 0)
						break;
				}
			}
			if (bCCKM)
				strcpy(APList[i].Auth, "CCKM");
			if (bWPA)
				strcpy(APList[i].Auth, "WPA");
			else if (bWPAPSK)
				strcpy(APList[i].Auth, "WPAPSK");
			else if (bWPANONE)
				strcpy(APList[i].Auth, "WPANONE");
			else if (bWPA2)
				strcpy(APList[i].Auth, "WPA2");
			else if (bWPA2PSK)
				strcpy(APList[i].Auth, "WPA2PSK");
			else if (bWPA2NONE)
				strcpy(APList[i].Auth, "WPA2NONE");
			else {
				strcpy((char *)APList[i].Auth, "Unknown");
			}
			
			if (bTKIP)
				strcpy(APList[i].Encry, "TKIP");
			else if (bAESWRAP || bAESCCMP)
				strcpy(APList[i].Encry, "AES");
			else {
				if (pBssid->Privacy)  // privacy value is on/of
					strcpy(APList[i].Encry, "WEP");
				else {
					strcpy(APList[i].Encry, "NONE");
					strcpy(APList[i].Auth, "OPEN");
				}
			}

			if (pBssid->InfrastructureMode == Ndis802_11Infrastructure)
				strcpy(APList[i].Infr, "Infra");
			else
				strcpy(APList[i].Infr, "Adhoc");

		//printf("Site[%d]: %s %s %s %s\n", i, APList[i].SSID, APList[i].Auth, APList[i].Encry, APList[i].Infr);
			
			pBssid = (PNDIS_WLAN_BSSID_EX)((char *)pBssid + pBssid->Length);
			
		}
	}
	free(pBssidList);
	close(s);
	return num;
}

int ConnectAP(SiteSurvey APList[], TEasyLink *g_tEasyLink, int i)
{
	char cmdssid[1024], cmdpwd[1024], cmdauth[1024], cmdencry[1024], cmdinfr[1024];

	printf("ConnectAP[%d]: %s %s %s %s\n", i, APList[i].SSID, APList[i].Auth, APList[i].Encry, APList[i].Infr);
	
	sprintf(cmdinfr, "iwpriv ra0 set NetworkType=%s", APList[i].Infr);
	sprintf(cmdauth, "iwpriv ra0 set AuthMode=%s", APList[i].Auth);
	sprintf(cmdencry, "iwpriv ra0 set EncrypType=%s", APList[i].Encry);
	sprintf(cmdssid, "iwpriv ra0 set SSID=%s", APList[i].SSID);
	sprintf(cmdpwd, "iwpriv ra0 set WPAPSK=%s", g_tEasyLink->uPwd);
	
	system("brctl delif br0 ra0");
	system("ifconfig br0 down");
	system("ifconfig ra0 up");
	
	if(!strcmp("Infra", APList[i].Infr))
	{

		system(cmdinfr);
		system(cmdauth);
		system(cmdencry);

		if (!strcmp("OPEN", APList[i].Auth))
			{
				printf("Auth=%s\n", APList[i].Auth);

				system(cmdssid);
			}
		else if (strstr(APList[i].Auth, "WPA") != NULL)
			{
				printf("Auth=%s\n", APList[i].Auth);
				system(cmdssid);
				system(cmdpwd);
				system(cmdssid);
			}
		else{
				return -1;
			}

		system("killall udhcpc");
		
		system("udhcpc -i ra0 -s /sbin/udhcpc.sh -p /var/run/udhcpc_ra0.pid");
		
	}
	return 0;
}



//函数名称: CheckConnect
//函数说明: 检测和目标主机是否连接
//传入变量: char *dst:需要ping的目标主机,int cnt:ping 尝试次数
//函数返回: -1表示连接失败,其它值连接成功
int CheckConnect(char *dst, int cnt)  
{  
    int i = 0;  
    FILE *stream;  
    char recvBuf[16] = {0};  
    char cmdBuf[256] = {0};  
  
    if (NULL == dst || cnt <= 0)  
        return -1;  
  
    sprintf(cmdBuf, "ping %s -c %d | grep time= | wc -l", dst, cnt);  
		printf("CheckConnect: cmdBuf=%s\r\n",cmdBuf);
	
    stream = popen(cmdBuf, "r");  
    fread(recvBuf, sizeof(char), sizeof(recvBuf)-1, stream);  
    pclose(stream);  

  	printf("CheckConnect: recvBuf=%s\r\n",recvBuf);
    if (atoi(recvBuf) > 0)  
    return 0;  
  
    return -1;  
}


//函数名称: CheckConnect
//函数说明: 检测和目标主机是否连接
//传入变量: char *dst:需要ping的目标主机,int cnt:ping 尝试次数
//函数返回: -1表示连接失败,其它值连接成功
int CheckWifiStatus(void)  
{  
    int i = 0;  
    FILE *stream;  
    char recvBuf[16] = {0};  
    char cmdBuf[256] = {0};  
   
    sprintf(cmdBuf, "iwconfig ra0 |grep Auto");  
	printf("CheckConnect: cmdBuf=%s\r\n",cmdBuf);
	
    stream = popen(cmdBuf, "r");  
    i=fread(recvBuf, sizeof(char), sizeof(recvBuf)-1, stream);  
    pclose(stream);  

  	printf("CheckConnect: recvBuf=%s\r\n",recvBuf);
    //if (strlen(recvBuf) == 0)  
    if(i <= 0)
    {
    	return 0;  
  	}
  	else
  	{
    	return 1;
    }  
}

int main(int argc, char* argv[]) 
{
	int ret, i;	
	TEasyLink g_tEasyLink;
	SiteSurvey	APList[100];
	int iChannel; 
	char cChannelCmd[50];
	int iFountChannel = 0;
	int iRecIpPort = 0;
	int iLinkRoute = 0;
	
	//socket
	int    sockfd, num;  
	char   buf[100];  
	struct hostent *he;  
	struct sockaddr_in server;  
	
	unsigned char if_addr[20]; 
	memset(if_addr, 0, sizeof(if_addr));
	
	memset(&cChannelCmd, 0, sizeof(cChannelCmd));
	memset(&g_tEasyLink, 0, sizeof(g_tEasyLink));

	sock_fd = -1;
	while(sock_fd == -1)
	{
		sock_fd=socket(PF_NETLINK, SOCK_RAW, NETLINK_TEST);
		if(sock_fd == -1)
		{
			printf("Creat socket err!!!");
			//return 0;
		}
	}
	
    memset(&src_addr, 0, sizeof(src_addr));
    memset(&msg, 0, sizeof(msg));
    
    src_addr.nl_family = AF_NETLINK;
	src_addr.nl_pid = getpid();  /* self pid */
	//src_addr.nl_family = PF_NETLINK;
	 //src_addr.nl_pid = 0;
    /* interested in group 1<<0 */
    src_addr.nl_groups = 1;
	ret = -1;
	ret=bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));
	if(ret<0)
	{
		printf("bink err!!!");
		//return 0;
	}

    memset(&dest_addr, 0, sizeof(dest_addr));
    nlh = (struct nlmsghdr *)malloc(sizeof(g_tEasyLink));
    memset(nlh, 0, sizeof(g_tEasyLink));

    iov.iov_base = (void *)nlh;
    iov.iov_len = sizeof(g_tEasyLink);
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
	
	iChannel = 1;
	iFountChannel = 0;
	system("iwconfig ra0 mode monitor");
	while(1)
	{
		//printf("---NO DATA FROM KERNEL---\n");
		/* Read message from kernel */
		//system("free");
		//system("cat /proc/meminfo");
		if(iFountChannel == 0)
		{
			//iChannel = 3;
			sprintf(cChannelCmd, "iwconfig ra0 channel %d\r\n", iChannel);
			system(cChannelCmd);
			usleep(1000);
			system("iwconfig ra0 mode monitor");
			iChannel ++;
			if(iChannel > 14)
			{
				iChannel = 1;
			}
		}
		else if(iFountChannel == 1)
		{
			sprintf(cChannelCmd, "iwconfig ra0 channel %d\r\n", g_tEasyLink.uChannel);
			system(cChannelCmd);
			usleep(1000);
			system("iwconfig ra0 mode monitor");
			iFountChannel = 2;
		}
		usleep(100*1000);
		recflag = recvmsg(sock_fd, &msg, MSG_DONTWAIT);
		if(recflag > 0)
		{		
			memcpy(&g_tEasyLink, NLMSG_DATA(nlh), sizeof(g_tEasyLink));
			
			printf("Received message payload: recflag=%d,Type=%d,Channel=%d,SSID=%s,PWD=%s,IP=%x,Port=%d\n",  \
			recflag, g_tEasyLink.uLinkType, g_tEasyLink.uChannel, g_tEasyLink.uSSID, g_tEasyLink.uPwd, \
			g_tEasyLink.uIP, g_tEasyLink.uPort);;

			int index = 0;
			printf("***start to print ssid***\r\n");
			for(index=0; index<strlen(g_tEasyLink.uSSID); index++)
			{
				printf("SSID.%d=%x\r\n",index, g_tEasyLink.uSSID[index]);
			}
			printf("***print ssid over***\r\n");
			
			if(g_tEasyLink.uLinkType == NET_LINK_CHANNEL)
			{	
				iFountChannel = 1;
				sprintf(cChannelCmd, "iwconfig ra0 channel %d", g_tEasyLink.uChannel);
				system(cChannelCmd);
			}
			else if(g_tEasyLink.uLinkType == NET_LINK_SSIDPWD)
			{
				/*1.switch mode*/
				while(CheckWifiStatus() == 0)
				{
					printf("***iwconfig ra0 mode managed***\r\n");
					system("iwconfig ra0 mode managed");
					//system("ap_sta_switch.sh sta");
					usleep(200*1000); 
				}
				
				/*2.scan ap */
				ret = getsitesurvey(APList);
				//printf("ret = %d\n",ret);
								
					
				/*3.connect ap */
				for (i=0; i<ret; i++)
				{
					printf("Main[%d]: %s %s %s %s\n", i, APList[i].SSID, APList[i].Auth, APList[i].Encry, APList[i].Infr);
					int len1 = strlen(g_tEasyLink.uSSID);
					int len2 = strlen(APList[i].SSID);
					printf("len scan=%d, len rec=%d\r\n",len1,len2);
					int len = (len1>len2)?len1:len2;
				
					if(strcmp(g_tEasyLink.uSSID, APList[i].SSID) == 0)
					{
						printf("***Match the SSID:start to Connect AP***\r\n");
						ConnectAP(APList, &g_tEasyLink, i);
						iLinkRoute = 1;
						printf("Connect AP ok!!!===TCL Tonly");
						break;
						//return 0;
					}
				}	
				if(i == ret)
				{
					printf("***Can't find the match SSID***");
					for (i=0; i<ret; i++)
					{					
						//if(strstr(g_tEasyLink.uSSID, APList[i].SSID) != NULL)
						//{
						//	printf("***Match the SSID:start to Connect AP1***\r\n");
						//	ConnectAP(APList, &g_tEasyLink, i);
						//	iLinkRoute = 1;
						//	printf("Connect AP1 ok!!!===TCL Tonly");
						//	//return 0;
						//}
						if(strstr(APList[i].SSID,g_tEasyLink.uSSID ) != NULL)
						{
							printf("***Match the SSID:start to Connect AP2***\r\n");
							ConnectAP(APList, &g_tEasyLink, i);
							iLinkRoute = 1;
							printf("Connect AP2 ok!!!===TCL Tonly");
							break;
							//return 0;
						}
					}	
				}
			}
			else if(g_tEasyLink.uLinkType == NET_LINK_REIPPORT)
			{
				iRecIpPort=1;
			}
			
			//连接手机建立socket
			if((iLinkRoute==1) && (iRecIpPort==1))
			{
				
				//if((he=gethostbyname(argv[1]))==NULL)
				//{  
				//	printf("gethostbyname()error\n");  
				//   exit(1);  
			    //}  
			   printf("start socket...");
			   if((sockfd=socket(AF_INET, SOCK_STREAM, 0))==-1)
			   {  
				   printf("socket()error\n");  
				   exit(1);  
			   }  
			   bzero(&server,sizeof(server));  
			   server.sin_family= AF_INET;  
			   server.sin_port = htons(g_tEasyLink.uPort);  
			   
			   strcpy(if_addr, inet_ntoa(*((struct in_addr *)&(g_tEasyLink.uIP))));
			   inet_aton(if_addr, &server.sin_addr.s_addr);
			   //server.sin_addr.s_addr =((unsigned long)g_tEasyLink.uIP); 			   
			   //inet_aton("63.161.169.137", &myaddr.sin_addr.s_addr);
			   printf("server ip1=%s,ip=%s,port=%d=0x%x\r\n",if_addr, inet_ntoa(server.sin_addr), \
			   server.sin_port,server.sin_port);
			   
			   //检测主机是否能够连接上: 由于busybox中ping 命令不支持 -i参数超时时间过长  
			   //while(CheckConnect(if_addr, 2) == -1)
			   //{
				////	printf("can't connect to the server!");
			   //}
			   
			   if(connect(sockfd,(struct sockaddr *)&server,sizeof(server))<0)
			   {  
					printf("connect()error\n");   
					printf("%s\n", strerror(errno));  
					exit(1);  
			   }   
			   system("reboot");
			   if((num=recv(sockfd,buf,1024,0)) == -1)
			   {  
				   printf("recv() error\n");  
				   exit(1);  
			   }  
			   buf[num-1]='\0';  
			   printf("Server Message: %s\n",buf);  
			  
			   close(sockfd);  
			}
		}
		//printf("Received message payload: %s\");		
		//usleep(200*1000); 
    }    
    close(sock_fd);  
}
