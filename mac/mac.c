#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/wait.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/un.h>
#include <getopt.h>



void getDeviceMac(char* eth, char * macStr)
{
    int fd;
    struct ifreq ifr_mac;

    if((eth==NULL) || (macStr==NULL)){return;}
    
    if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("getDeviceMac failed...\n");
        return;
    }

    memset(&ifr_mac, 0, sizeof(ifr_mac));

    strncpy(ifr_mac.ifr_name, eth, strlen(eth));

    if(ioctl(fd, SIOCGIFHWADDR, &ifr_mac) < 0)
    {
        printf("getDeviceMac ioctrl failed...\n");
        return;
    }
    close(fd);
    if(macStr != NULL) {
        sprintf(macStr,"%02x%02x%02x%02x%02x%02x", 
                (unsigned   char)ifr_mac.ifr_hwaddr.sa_data[0], 
                (unsigned   char)ifr_mac.ifr_hwaddr.sa_data[1], 
                (unsigned   char)ifr_mac.ifr_hwaddr.sa_data[2], 
                (unsigned   char)ifr_mac.ifr_hwaddr.sa_data[3], 
                (unsigned   char)ifr_mac.ifr_hwaddr.sa_data[4], 
                (unsigned   char)ifr_mac.ifr_hwaddr.sa_data[5]); 
    }
    printf("%s\n",macStr);
}


int main (int argc, char *argv[])
{
	char strMac[1024] = {0};
	getDeviceMac(argv[1],strMac);
	return 0;
}
