#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <net/if.h>
#include <netinet/in.h>
#include <linux/sockios.h>
#include <pthread.h>

#define     TYPE_CHANNEL	(0x00<<5)
#define		TYPE_SSID 		(0x01<<5)
#define 	TYPE_PSWD		(0x02<<5)
#define 	TYPE_REIP		(0x03<<5) 	//IP��ַ�˿�,�Ѿ������½��м�������

#define BUFLEN 				10
#define BACKLOG				5           //listen�����еȴ���������  
#define MAXDATASIZE 		1024        //��������С  
  
typedef struct _CLIENT  
{  
    int fd;                     //�ͻ���socket������  
    char name[20];              //�ͻ�������  
    struct sockaddr_in addr;    //�ͻ��˵�ַ��Ϣ�ṹ��  
    char data[MAXDATASIZE];     //�ͻ���˽������ָ��  
} CLIENT;  

unsigned int g_IfIPAddr = 0;
unsigned int g_IfPort = 0;
int g_MultiCast = 1;
int g_MultiCastType = 0;

/*
 * arguments: ifname  - interface name
 *            if_addr - a 16-byte buffer to store ip address
 * description: fetch ip address, netmask associated to given interface name
 */
int GetIfIp(char *ifname, char *if_addr)
{
	struct ifreq ifr;
	int skfd = 0;

	if((skfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("GetIfIp:loack addr wrong!\n");
		return -1;
	}

	strncpy(ifr.ifr_name, ifname, IF_NAMESIZE);
	if (ioctl(skfd, SIOCGIFADDR, &ifr) < 0) {
		close(skfd);
		printf("GetIfIp:GetIfIp!\n");
		return -1;
	}
	strcpy(if_addr, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));	
	//g_IfIPAddr = (unsigned int)(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
	close(skfd);
	return 0;
}

int MultiCast(unsigned char IpAddr[],unsigned int port)
{
    struct sockaddr_in peeraddr;
	char local_ip[16];
    int sockfd;
    char recmsg[BUFLEN+1]={0x11,0x22,0x33,0x44};
    unsigned int socklen;
	struct sockaddr_in local_addr_in;
	
    // ���� socket ����UDPͨѶ 
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket\n");
        return 0;
    }
	
	if(g_MultiCastType == 0)
	{
		GetIfIp("br0", local_ip); //ʹ������
	}
	else
	{
		GetIfIp("ra0", local_ip); //wifi
	}
	
	//�������ñ���IP��ַ
    if(inet_pton(AF_INET, &local_ip, &local_addr_in.sin_addr)<=0){
        printf("MultiCast:loack addr wrong!\n");
        return 0;
    }
	local_addr_in.sin_family = AF_INET;
	local_addr_in.sin_port = htons(8080);
	//local_addr_in.sin_addr.s_addr = INADDR_ANY;
	if(bind(sockfd, (struct sockaddr *)&local_addr_in, sizeof(struct sockaddr_in))== -1)
	{
		printf("MultiCast: Bind error\r\n");
		return 0;
	}	
	socklen = sizeof(struct sockaddr_in);

    // ���öԷ��Ķ˿ں�IP��Ϣ 
    memset(&peeraddr, 0, socklen);
    peeraddr.sin_family = AF_INET;
	peeraddr.sin_port = htons(port);
      
    // ע���������õĶԷ���ַ��ָ�鲥��ַ�������ǶԷ���ʵ��IP��ַ 
	//printf("MultiCast:Ipaddr-%s\r\n",IpAddr);
    if(inet_pton(AF_INET, IpAddr, &peeraddr.sin_addr)<=0){
        printf("MultiCast:wrong group address!\n");
        return 0;
    }
   
    // ѭ�������û��������Ϣ�����鲥��Ϣ,ÿ�����ݷ���һ�β��ٴ���
    //for (;;)
	{
		//������Ϣ,���ٷ������ݳ���Ϊ2���ֽ�
        if(sendto(sockfd,recmsg, 2,0,(struct sockaddr *)&peeraddr,sizeof(struct sockaddr_in))<0){
            printf("MultiCast:sendto error!\n");
            exit(errno);
			return 0;
        }
        //printf("MultiCast:multicase success!\n");
    }
	close(sockfd);
    return 1;
}

int MakeSendFrm(unsigned char *pbSendData, unsigned char type, unsigned char index, unsigned char *pData)
{
	unsigned char bMultiAddr = 238;
	sprintf(pbSendData, "%d.%d.%d.%d",bMultiAddr, (type)|(index), pData[0], pData[1]);
	//printf("MakeSendFrm:%s\r\n",pbSendData);
	printf("%0.2x.%0.2x.%0.2x.%0.2x\r\n",bMultiAddr, (type)|(index), pData[0], pData[1]);
	return 1;
}


int WifiEasyConnect(unsigned char bChanged, unsigned char *pbSSID, int ilenSSID,unsigned char *pbPwd, int ilenPwd)
{
	//IP�鲥��ַ���ڱ�ʶһ��IP�鲥�顣IANA��D���ַ�ռ������鲥ʹ�ã���Χ��224.0.0.0��239.255.255.255�� 
	//�鲥��ַ��߼�λΪ 1110 1110 :238
	unsigned char bFrm[20]; 
	unsigned char bSendData[2];
	unsigned short wSum;
	int index = 0;
	int iLen  = 0;
	static unsigned char bSSIDData[18][16];
	static unsigned char bPwdData[18][16];
	static unsigned char bReIPData[18][16];
	static unsigned char bChannelData[16];
	static int iMakeFrm = 0;
	
	if(bChanged == 1) //������֡
	{		
		//printf("WifiEasyConnect-ssid:%s,len=%d;pwd:%s,len-%d\r\n",pbSSID, ilenSSID, pbPwd, ilenPwd);
		//�����ŵ���־֡
		//memset(bFrm, 0, sizeof(bFrm));
		//MakeSendFrm(&bFrm, TYPE_SSID, index, &bSendData);
		//MultiCast(&bFrm, 0x8080);
		
		bSendData[0] = 0x01;
		bSendData[1] = 0x02;
		memset(bChannelData, 0, sizeof(bChannelData));
		MakeSendFrm(&bChannelData, TYPE_CHANNEL, 0, &bSendData);
		
		
		//��IPd��ַ�Ͷ˿�����֡
		iLen = 3+2;
		wSum = 0;	
		memset(bReIPData, 0, sizeof(bReIPData));
		
		memset(bSendData, 0, sizeof(bSendData));
		bSendData[0] = 0xAA;			
		bSendData[1] = iLen;
		wSum = bSendData[0]+bSendData[1];
		MakeSendFrm(&bReIPData[0], TYPE_REIP, 0, &bSendData);
		
		//IP��ַ
		bSendData[0] = (unsigned char)((g_IfIPAddr&0xFF000000)>>24);	
		bSendData[1] = (unsigned char)((g_IfIPAddr&0x00FF0000)>>16);		
		wSum += bSendData[0]+bSendData[1];
		MakeSendFrm(&bReIPData[1], TYPE_REIP, 1, &bSendData);
		
		
		bSendData[0] = (unsigned char)((g_IfIPAddr&0x0000FF00)>>8);
		bSendData[1] = (unsigned char)((g_IfIPAddr&0x000000FF)>>0);
		wSum += bSendData[0]+bSendData[1];
		MakeSendFrm(&bReIPData[2], TYPE_REIP, 2, &bSendData);
		
		//�˿ں�
		bSendData[0] = (unsigned char)((g_IfPort&0xFF00)>>8);			
		bSendData[1] = (unsigned char)((g_IfPort&0x00FF)>>0);
		wSum += bSendData[0]+bSendData[1];
		MakeSendFrm(&bReIPData[3], TYPE_REIP, 3, &bSendData);
		
		//У��
		bSendData[0] = (wSum&0xFF00)>>8;
		bSendData[1] = (wSum&0xFF);
		MakeSendFrm(&bReIPData[4], TYPE_REIP, 4, &bSendData);		
		
		//����SSID
		iLen = (ilenSSID+1)/2+2;
		wSum = 0;	
		memset(bSSIDData, 0, sizeof(bSSIDData));
		for(index=0; index<iLen; index++)
		{
			memset(bSendData, 0, sizeof(bSendData));	
			if(index == 0)
			{		
				bSendData[0] = 0xAA;			
				bSendData[1] = iLen;
				wSum = bSendData[0]+bSendData[1];
			}
			else if(index+1 == iLen)
			{
				//����У��
				bSendData[0] = (wSum&0xFF00)>>8;
				bSendData[1] = (wSum&0xFF);
				//printf("WifiEasyConnect-ssid:%d-%d\r\n",bSendData[1],bSendData[0]);
			
			}
			else
			{

				bSendData[0] = pbSSID[(index-1)*2];	
				if((index+2==iLen) && (ilenSSID%2))  //ֻ��������ʱ�����������
				{
					bSendData[1] = 0;
				}
				else
				{
					bSendData[1] = pbSSID[(index-1)*2+1];			
				}
				wSum = wSum+bSendData[0]+bSendData[1];			
				
				//printf("$%d-%d-%d\r\n",index,pbSSID[(index-1)*2],pbSSID[(index-1)*2+1]);
				//printf("#%d-%d-%d\r\n",index,bSendData[0],bSendData[1]);
				
			}
			//memset(bSSIDData, 0, sizeof(bSSIDData));
			MakeSendFrm(&bSSIDData[index], TYPE_SSID, index, &bSendData);
			//MultiCast(&bFrm, 0x8080);
		}
		
		
		//����PASSWORD 
		iLen = (ilenPwd+1)/2+2;
		wSum = 0;	
		memset(bPwdData, 0, sizeof(bPwdData));
		for(index=0; index<iLen; index++)
		{
			memset(bSendData, 0, sizeof(bSendData));	
			if(index == 0)
			{		
				bSendData[0] = 0xAA;			
				bSendData[1] = iLen;
				wSum = bSendData[0]+bSendData[1];
			}
			else if(index+1 == iLen)
			{
				//����У��
				bSendData[0] = (wSum&0xFF00)>>8;
				bSendData[1] = (wSum&0xFF);
				//printf("WifiEasyConnect-pwd:%d-%d\r\n",bSendData[0],bSendData[1]);
			}
			else
			{
				bSendData[0] = pbPwd[(index-1)*2];	
				if((index+2==iLen) && (ilenPwd%2))  //ֻ��������ʱ�����������
				{
					bSendData[1] = 0;
				}
				else
				{
					bSendData[1] = pbPwd[(index-1)*2+1];			
				}
				wSum = wSum+bSendData[0]+bSendData[1];
				//printf("$%d-%d-%d\r\n",index,pbPwd[(index-1)*2],pbPwd[(index-1)*2+1]);
				//printf("#%d-%d-%d\r\n",index,bSendData[0],bSendData[1]);
			}
			//memset(bPwdData, 0, sizeof(bPwdData));
			MakeSendFrm(&bPwdData[index], TYPE_PSWD, index, &bSendData);
			//MultiCast(&bFrm, 0x8080);
			
			
		}	
	}
	
	//�ŵ�ÿ�η��Ͷ��00 01 02����ȷ���ŵ�
	MultiCast(&bChannelData, 0x8080);
	MultiCast(&bChannelData, 0x8080);
	MultiCast(&bChannelData, 0x8080);
	MultiCast(&bChannelData, 0x8080);
	
	//����IP��ַ�˿�
	iLen = 3+2;			
	for(index=0; index<iLen; index++)
	{
		MultiCast(&bReIPData[index], 0x8080);
	}
	
	//����PWD
	iLen = (ilenPwd+1)/2+2;		
	for(index=0; index<iLen; index++)
	{
		MultiCast(&bPwdData[index], 0x8080);
	}
	
	//����SSID
	iLen = (ilenSSID+1)/2+2;			
	for(index=0; index<iLen; index++)
	{
		MultiCast(&bSSIDData[index], 0x8080);
	}
	
	return 1;
}

void TcpServerThread(void)                                                               
{   
	int i, maxi, maxfd, sockfd;  
    int nready;  
    ssize_t n;  
    fd_set rset, allset;        //select������ļ�����������  
    int listenfd, connectfd;    //socket�ļ�������  
    struct sockaddr_in server;  //��������ַ��Ϣ�ṹ��  
	char local_ip[16];
  
    CLIENT client[FD_SETSIZE];  //FD_SETSIZEΪselect����֧�ֵ��������������  
    char recvbuf[MAXDATASIZE];  //������  
    int sin_size;               //��ַ��Ϣ�ṹ���С  
  
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)  
    {                          	//����socket�������ڼ����ͻ��˵�socket  
        perror("Creating socket failed.");  
        exit(1);  
    }  
  
    int opt = SO_REUSEADDR;  
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));  //����socket����  
  
    bzero(&server, sizeof(server));  
    server.sin_family = AF_INET;  
    server.sin_port = htons(0x8000);  
	
	if(g_MultiCastType == 0)
	{
		GetIfIp("br0", local_ip); //ʹ������
	}
	else
	{
		GetIfIp("ra0", local_ip); //wifi
	}
	
	//�������ñ���IP��ַ
    if(inet_pton(AF_INET, &local_ip, &server.sin_addr)<=0){
        printf("MultiCast:loack addr wrong!\n");
        return 0;
    }
    //server.sin_addr.s_addr = htonl(INADDR_ANY);  
  
    if (bind(listenfd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)  
    {                           //����bind�󶨵�ַ  
        perror("Bind error.");  
        exit(1);  
    }  
  
    if (listen(listenfd, BACKLOG) == -1)  
    {                           //����listen��ʼ����  
        perror("listen() error\n");  
        exit(1);  
    }  
  
    //��ʼ��select  
    maxfd = listenfd;  
    maxi = -1;  
    for (i = 0; i < FD_SETSIZE; i++)  
    {  
        client[i].fd = -1;  
    }  
    FD_ZERO(&allset);           //���  
    FD_SET(listenfd, &allset);  //������socket����select��������������  
  
    while (1)  
    {  
        struct sockaddr_in addr;  
        rset = allset;  
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);    //����select  
        printf("Select() break and the return num is %d. \n", nready);  
  
        if (FD_ISSET(listenfd, &rset))  
        {                       //����Ƿ����¿ͻ�������  
            printf("Accept a connection.\n");  
			//g_MultiCast = 0; //ֹͣ�㲥���� 
			
			
            //����accept�����ط�������ͻ������ӵ�socket������  
            sin_size = sizeof(struct sockaddr_in);  
            if ((connectfd =  
                 accept(listenfd, (struct sockaddr *)&addr, (socklen_t *) & sin_size)) == -1)  
            {  
                perror("Accept() error\n");  
                continue;  
            }  
  
            //���¿ͻ��˵ļ�������  
            for (i = 0; i < FD_SETSIZE; i++)  
            {  
                if (client[i].fd < 0)  
                {  
                    char buffer[20];  
                    client[i].fd = connectfd;   //����ͻ���������  
                    memset(buffer, '0', sizeof(buffer));  
                    sprintf(buffer, "Client[%.2d]", i);  
                    memcpy(client[i].name, buffer, strlen(buffer));  
                    client[i].addr = addr;  
                    memset(buffer, '0', sizeof(buffer));  
                    sprintf(buffer, "Only For Test!");  
                    memcpy(client[i].data, buffer, strlen(buffer));  
                    printf("You got a connection from %s:%d.\n", inet_ntoa(client[i].addr.sin_addr),ntohs(client[i].addr.sin_port));  
                    printf("Add a new connection:%s\n",client[i].name);  
                    break;  
                }  
            }  
              
            if (i == FD_SETSIZE)  
                printf("Too many clients\n");  
            FD_SET(connectfd, &allset); //����socket���ӷ���select��������  
            if (connectfd > maxfd)  
                maxfd = connectfd;  //ȷ��maxfd�����������  
            if (i > maxi)       //�������Ԫ��ֵ  
                maxi = i;  
            if (--nready <= 0)  
                continue;       //���û���¿ͻ������ӣ�����ѭ��  
        }  
  
        for (i = 0; i <= maxi; i++)  
        {  
            if ((sockfd = client[i].fd) < 0)    //����ͻ���������С��0����û�пͻ������ӣ������һ��  
                continue;  
            // �пͻ����ӣ�����Ƿ�������  
            if (FD_ISSET(sockfd, &rset))  
            {  
                printf("Receive from connect fd[%d].\n", i);  
                if ((n = recv(sockfd, recvbuf, MAXDATASIZE, 0)) == 0)  
                {               //�ӿͻ���socket�����ݣ�����0��ʾ�����ж�  
                    close(sockfd);  //�ر�socket����  
                    printf("%s closed. User's data: %s\n", client[i].name, client[i].data);  
                    FD_CLR(sockfd, &allset);    //�Ӽ���������ɾ����socket����  
                    client[i].fd = -1;  //����Ԫ�����ʼֵ����ʾû�ͻ�������  
                }  
                else  
				{
					//process_client(&client[i], recvbuf, n); //���յ��ͻ����ݣ���ʼ����  
                }
				if (--nready <= 0)  
                    break;      //���û���¿ͻ��������ݣ�����forѭ���ص�whileѭ��  
            }  
        }  
    }  
    close(listenfd);            //�رշ���������socket       
}  

//��������: CheckWifiStatus
//����˵��: ����Ŀ�������Ƿ�����
//�������: char *dst:��Ҫping��Ŀ������,int cnt:ping ���Դ���
//��������: �����ַ�������
int CheckWifiStatus(unsigned char *pString)  
{  
    int i = 0;  
    FILE *stream;  
    char recvBuf[16] = {0};  
    char cmdBuf[256] = {0};  
   
    sprintf(cmdBuf, "iwpriv ra0 show SSID | awk '{print $3}'");  
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
		i--;
		memcpy(pString, recvBuf, i);
    	return i;
    }  
}
 

int main(int argc, char *argv[])
{
	//unsigned char bSSID_data[] = "HUAWEI-DCSVDU";//
	//unsigned char bPwd_data[]  = "12345678";//"tonly";
	char ip[16];
	unsigned char bSSID_data[40];
	unsigned char bPwd_data[40];
	int iSSIDLen = 0;
	int ipwdLen = 0;
	//unsigned char bSSID_data[] = "NETGEAR-Guest1";
	//unsigned char bPwd_data[]  = "";	
	int i,ret;
	pthread_t id; 	
	
	g_MultiCast = 1;
	if(argc > 1)
	{
		if(argv[1][0] == '0')
		{
			g_MultiCastType = 0;
			printf("multi_cast: use br0 \r\n");
		}
		else
		{
			g_MultiCastType = 1;
			printf("multi_cast: use ra0 \r\n");			
		}
	}
	
	memset(bPwd_data, 0, sizeof(bPwd_data));
	if(argv[2] != NULL)
	{
		ipwdLen = strlen(argv[2]);
		if(ipwdLen>=0)
		{
			memcpy(bPwd_data, argv[2], ipwdLen);
			printf("Multi pwd:%d-%s\r\n",ipwdLen,bPwd_data);			
		}
	}
	else
	{
		ipwdLen = 0;
		printf("Multi pwd:%d-NULL\r\n",ipwdLen);		
	}
	
	memset(bSSID_data, 0, sizeof(bSSID_data));
	iSSIDLen=CheckWifiStatus(&bSSID_data);
	if(iSSIDLen > 0)
	{
		printf("Multi SSID:%d-%s\r\n",iSSIDLen,bSSID_data);
	}
	else
	{
		return 0;
	}
	
	//����һ���߳����ڽ����Ƿ��й㲥����֡����
	printf("Create listen thread\r\n"); 
	
	//��ȡ����IP��ַ
	if(g_MultiCastType == 0)
	{
		GetIfIp("br0", ip); //ʹ������
	}
	else
	{
		GetIfIp("ra0", ip); //wifi
	}
	//IP���� g_IfIPAddr,�˿�ʹ��0x8000
	g_IfIPAddr = (unsigned long)inet_addr(ip);
	g_IfPort = 0x8000;	
	printf("IP=%08x,Port=%d\r\n",g_IfIPAddr,g_IfPort);
	
	ret=pthread_create(&id, NULL,(void *)TcpServerThread, NULL); 
	if(ret!=0)
	{                                                                 
        printf ("Create pthread fail!\n");                                     
        exit (1);                                                               
    }  
	else
	{
		//printf ("Create Key Play pthread sucess!\n");                                     
	}	
	
	WifiEasyConnect(1, &bSSID_data, iSSIDLen, &bPwd_data, ipwdLen);
	while(g_MultiCast)
	{
		WifiEasyConnect(0, &bSSID_data, iSSIDLen, &bPwd_data, ipwdLen);
		usleep(40*1000);
		//usleep(100*1000);
	}
	return 0;
	
}