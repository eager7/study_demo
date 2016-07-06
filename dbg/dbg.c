/*************************************************************************
	> File Name: dbg.c
	> Author: 
	> Mail: 
	> Created Time: 2016年02月17日 星期三 11时16分38秒
 ************************************************************************/

#include<stdio.h>
#include <sys/types.h>

#define TL_UI_BLUE(x) 	    "\e[34;1m"x"\e[0m"
#define TL_UI_GREEN(x) 	    "\e[32;1m"x"\e[0m"
#define TL_UI_YELLOW(x)     "\e[33;40;1m"x"\e[0m"
#define TL_UI_RED(x)	            "\e[31;1m"x"\e[0m"
#define TL_UI_BACK_BLUE(x) "\e[0;37;41m"x"\e[0m" 
enum DBGTYPE
{
	E=1,
	W=2,
	I=4,
	D=8
};
#define MODE      "[BLE_SERVER]"
#define DBGSWITCH (E | W | I |D)        //DUG  LEAVE SWITCH
#define ERR   (E & DBGSWITCH)
#define WAR   (W & DBGSWITCH)
#define INFO  (I & DBGSWITCH)
#define DBG   (D & DBGSWITCH)
#define ENTER "\r\n"

#define IDBG(TYPE,FORMAT,...) { time_t startTime; time(&startTime);(INFO)? printf (TL_UI_GREEN("[%d]:%s[I]-->") "%s:"FORMAT"%s",startTime,MODE,TYPE,##__VA_ARGS__,ENTER):0;}
#define EDBG(TYPE,FORMAT,...) { time_t startTime; time(&startTime);(ERR)? printf (TL_UI_GREEN("[%d]:%s[E]-->") "%s:"FORMAT"%s",startTime,MODE,TYPE,##__VA_ARGS__,ENTER):0;}
#define WDBG(TYPE,FORMAT,...) { time_t startTime; time(&startTime);(WAR)? printf (TL_UI_GREEN("[%d]:%s[W]-->") "%s:"FORMAT"%s",startTime,MODE,TYPE,##__VA_ARGS__,ENTER):0;}
#define DDBG(TYPE,FORMAT,...) { time_t startTime; time(&startTime);(DBG)? printf (TL_UI_GREEN("[%d]:%s[D]-->") "%s:"FORMAT"%s",startTime,MODE,TYPE,##__VA_ARGS__,ENTER):0;}

int main()
{
	IDBG("idbg","test1\n");
	return 0;
}