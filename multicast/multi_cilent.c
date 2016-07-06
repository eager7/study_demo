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
#define 	TYPE_REIP		(0x03<<5) 	//IP地址端口,已经会重新进行简易连接

#define BUFLEN 				10
#define BACKLOG				5           //listen队列中等待的连接数  
#define MAXDATASIZE 		1024        //缓冲区大小  
  
typedef struct _CLIENT  
{  
    int fd;                     //客户端socket描述符  
    char name[20];              //客户端名称  
    struct sockaddr_in addr;    //客户端地址信息结构体  
    char data[MAXDATASIZE];     //客户端私有数据指针  
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
	
    // 创建 socket 用于UDP通讯 
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket\n");
        return 0;
    }
	
	if(g_MultiCastType == 0)
	{
		GetIfIp("br0", local_ip); //使用网口
	}
	else
	{
		GetIfIp("ra0", local_ip); //wifi
	}
	
	//这里设置本地IP地址
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

    // 设置对方的端口和IP信息 
    memset(&peeraddr, 0, socklen);
    peeraddr.sin_family = AF_INET;
	peeraddr.sin_port = htons(port);
      
    // 注意这里设置的对方地址是指组播地址，而不是对方的实际IP地址 
	//printf("MultiCast:Ipaddr-%s\r\n",IpAddr);
    if(inet_pton(AF_INET, IpAddr, &peeraddr.sin_addr)<=0){
        printf("MultiCast:wrong group address!\n");
        return 0;
    }
   
    // 循环接受用户输入的消息发送组播消息,每个数据发送一次不再处理
    //for (;;)
	{
		//发送消息,减少发送数据长度为2个字节
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
	//IP组播地址用于标识一个IP组播组。IANA把D类地址空间分配给组播使用，范围从224.0.0.0到239.255.255.255。 
	//组播地址最高几位为 1110 1110 :238
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
	
	if(bChanged == 1) //重新组帧
	{		
		//printf("WifiEasyConnect-ssid:%s,len=%d;pwd:%s,len-%d\r\n",pbSSID, ilenSSID, pbPwd, ilenPwd);
		//发送信道标志帧
		//memset(bFrm, 0, sizeof(bFrm));
		//MakeSendFrm(&bFrm, TYPE_SSID, index, &bSendData);
		//MultiCast(&bFrm, 0x8080);
		
		bSendData[0] = 0x01;
		bSendData[1] = 0x02;
		memset(bChannelData, 0, sizeof(bChannelData));
		MakeSendFrm(&bChannelData, TYPE_CHANNEL, 0, &bSendData);
		
		
		//组IPd地址和端口数据帧
		iLen = 3+2;
		wSum = 0;	
		memset(bReIPData, 0, sizeof(bReIPData));
		
		memset(bSendData, 0, sizeof(bSendData));
		bSendData[0] = 0xAA;			
		bSendData[1] = iLen;
		wSum = bSendData[0]+bSendData[1];
		MakeSendFrm(&bReIPData[0], TYPE_REIP, 0, &bSendData);
		
		//IP地址
		bSendData[0] = (unsigned char)((g_IfIPAddr&0xFF000000)>>24);	
		bSendData[1] = (unsigned char)((g_IfIPAddr&0x00FF0000)>>16);		
		wSum += bSendData[0]+bSendData[1];
		MakeSendFrm(&bReIPData[1], TYPE_REIP, 1, &bSendData);
		
		
		bSendData[0] = (unsigned char)((g_IfIPAddr&0x0000FF00)>>8);
		bSendData[1] = (unsigned char)((g_IfIPAddr&0x000000FF)>>0);
		wSum += bSendData[0]+bSendData[1];
		MakeSendFrm(&bReIPData[2], TYPE_REIP, 2, &bSendData);
		
		//端口号
		bSendData[0] = (unsigned char)((g_IfPort&0xFF00)>>8);			
		bSendData[1] = (unsigned char)((g_IfPort&0x00FF)>>0);
		wSum += bSendData[0]+bSendData[1];
		MakeSendFrm(&bReIPData[3], TYPE_REIP, 3, &bSendData);
		
		//校验
		bSendData[0] = (wSum&0xFF00)>>8;
		bSendData[1] = (wSum&0xFF);
		MakeSendFrm(&bReIPData[4], TYPE_REIP, 4, &bSendData);		
		
		//发送SSID
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
				//生成校验
				bSendData[0] = (wSum&0xFF00)>>8;
				bSendData[1] = (wSum&0xFF);
				//printf("WifiEasyConnect-ssid:%d-%d\r\n",bSendData[1],bSendData[0]);
			
			}
			else
			{

				bSendData[0] = pbSSID[(index-1)*2];	
				if((index+2==iLen) && (ilenSSID%2))  //只有奇数个时该数据填充零
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
		
		
		//发送PASSWORD 
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
				//生成校验
				bSendData[0] = (wSum&0xFF00)>>8;
				bSendData[1] = (wSum&0xFF);
				//printf("WifiEasyConnect-pwd:%d-%d\r\n",bSendData[0],bSendData[1]);
			}
			else
			{
				bSendData[0] = pbPwd[(index-1)*2];	
				if((index+2==iLen) && (ilenPwd%2))  //只有奇数个时该数据填充零
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
	
	//信道每次发送多个00 01 02用来确定信道
	MultiCast(&bChannelData, 0x8080);
	MultiCast(&bChannelData, 0x8080);
	MultiCast(&bChannelData, 0x8080);
	MultiCast(&bChannelData, 0x8080);
	
	//发送IP地址端口
	iLen = 3+2;			
	for(index=0; index<iLen; index++)
	{
		MultiCast(&bReIPData[index], 0x8080);
	}
	
	//发送PWD
	iLen = (ilenPwd+1)/2+2;		
	for(index=0; index<iLen; index++)
	{
		MultiCast(&bPwdData[index], 0x8080);
	}
	
	//发送SSID
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
    fd_set rset, allset;        //select所需的文件描述符集合  
    int listenfd, connectfd;    //socket文件描述符  
    struct sockaddr_in server;  //服务器地址信息结构体  
	char local_ip[16];
  
    CLIENT client[FD_SETSIZE];  //FD_SETSIZE为select函数支持的最大描述符个数  
    char recvbuf[MAXDATASIZE];  //缓冲区  
    int sin_size;               //地址信息结构体大小  
  
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)  
    {                          	//调用socket创建用于监听客户端的socket  
        perror("Creating socket failed.");  
        exit(1);  
    }  
  
    int opt = SO_REUSEADDR;  
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));  //设置socket属性  
  
    bzero(&server, sizeof(server));  
    server.sin_family = AF_INET;  
    server.sin_port = htons(0x8000);  
	
	if(g_MultiCastType == 0)
	{
		GetIfIp("br0", local_ip); //使用网口
	}
	else
	{
		GetIfIp("ra0", local_ip); //wifi
	}
	
	//这里设置本地IP地址
    if(inet_pton(AF_INET, &local_ip, &server.sin_addr)<=0){
        printf("MultiCast:loack addr wrong!\n");
        return 0;
    }
    //server.sin_addr.s_addr = htonl(INADDR_ANY);  
  
    if (bind(listenfd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)  
    {                           //调用bind绑定地址  
        perror("Bind error.");  
        exit(1);  
    }  
  
    if (listen(listenfd, BACKLOG) == -1)  
    {                           //调用listen开始监听  
        perror("listen() error\n");  
        exit(1);  
    }  
  
    //初始化select  
    maxfd = listenfd;  
    maxi = -1;  
    for (i = 0; i < FD_SETSIZE; i++)  
    {  
        client[i].fd = -1;  
    }  
    FD_ZERO(&allset);           //清空  
    FD_SET(listenfd, &allset);  //将监听socket加入select检测的描述符集合  
  
    while (1)  
    {  
        struct sockaddr_in addr;  
        rset = allset;  
        nready = select(maxfd + 1, &rset, NULL, NULL, NULL);    //调用select  
        printf("Select() break and the return num is %d. \n", nready);  
  
        if (FD_ISSET(listenfd, &rset))  
        {                       //检测是否有新客户端请求  
            printf("Accept a connection.\n");  
			//g_MultiCast = 0; //停止广播数据 
			
			
            //调用accept，返回服务器与客户端连接的socket描述符  
            sin_size = sizeof(struct sockaddr_in);  
            if ((connectfd =  
                 accept(listenfd, (struct sockaddr *)&addr, (socklen_t *) & sin_size)) == -1)  
            {  
                perror("Accept() error\n");  
                continue;  
            }  
  
            //将新客户端的加入数组  
            for (i = 0; i < FD_SETSIZE; i++)  
            {  
                if (client[i].fd < 0)  
                {  
                    char buffer[20];  
                    client[i].fd = connectfd;   //保存客户端描述符  
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
            FD_SET(connectfd, &allset); //将新socket连接放入select监听集合  
            if (connectfd > maxfd)  
                maxfd = connectfd;  //确认maxfd是最大描述符  
            if (i > maxi)       //数组最大元素值  
                maxi = i;  
            if (--nready <= 0)  
                continue;       //如果没有新客户端连接，继续循环  
        }  
  
        for (i = 0; i <= maxi; i++)  
        {  
            if ((sockfd = client[i].fd) < 0)    //如果客户端描述符小于0，则没有客户端连接，检测下一个  
                continue;  
            // 有客户连接，检测是否有数据  
            if (FD_ISSET(sockfd, &rset))  
            {  
                printf("Receive from connect fd[%d].\n", i);  
                if ((n = recv(sockfd, recvbuf, MAXDATASIZE, 0)) == 0)  
                {               //从客户端socket读数据，等于0表示网络中断  
                    close(sockfd);  //关闭socket连接  
                    printf("%s closed. User's data: %s\n", client[i].name, client[i].data);  
                    FD_CLR(sockfd, &allset);    //从监听集合中删除此socket连接  
                    client[i].fd = -1;  //数组元素设初始值，表示没客户端连接  
                }  
                else  
				{
					//process_client(&client[i], recvbuf, n); //接收到客户数据，开始处理  
                }
				if (--nready <= 0)  
                    break;      //如果没有新客户端有数据，跳出for循环回到while循环  
            }  
        }  
    }  
    close(listenfd);            //关闭服务器监听socket       
}  

//函数名称: CheckWifiStatus
//函数说明: 检测和目标主机是否连接
//传入变量: char *dst:需要ping的目标主机,int cnt:ping 尝试次数
//函数返回: 返回字符串长度
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
	
	//创建一个线程用于接听是否有广播数据帧过来
	printf("Create listen thread\r\n"); 
	
	//获取本地IP地址
	if(g_MultiCastType == 0)
	{
		GetIfIp("br0", ip); //使用网口
	}
	else
	{
		GetIfIp("ra0", ip); //wifi
	}
	//IP变量 g_IfIPAddr,端口使用0x8000
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