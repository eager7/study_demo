#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#define uint8_t unsigned char

#define PCT_DBG(b,ARGS...) do{	FILE *fp=fopen("myout.txt","a+");		\
	fprintf(fp," %s,%s,%d\n",__FILE__,__FUNCTION__,__LINE__); 	\
	fprintf(fp,b,## ARGS);												\
	fclose(fp);													\
	}while(0)
	
typedef struct _Time_Data{
	uint8_t StartHour;
	uint8_t StartMinute;
	uint8_t EndHour;
	uint8_t EndMinute;
}Time_Data_t;

static uint8_t StrToTime(Time_Data_t *TimeData, char *TimeBuffer);
static void GetCurrentTime();


int main(int argc, char *argv[])
{
	unsigned char i = 1;
	unsigned char j = 2;

	unsigned char time[] = "08:14-18:55";
	Time_Data_t TimeData;
	PCT_DBG("%d\n",time[0]);
	PCT_DBG("%d\n",time[1]);
	PCT_DBG("%d\n",(atoi(&time[3]))*10);
	StrToTime(&TimeData,time);
	GetCurrentTime();
	
	j = i - j;
	PCT_DBG("result = %d",j);
	return 0;
}


static uint8_t StrToTime(Time_Data_t *TimeData, char *TimeBuffer)
{
	TimeData->StartHour = atoi(&TimeBuffer[0]);
	TimeData->StartMinute = atoi(&TimeBuffer[3]);
	
	TimeData->EndHour = atoi(&TimeBuffer[6]);
	TimeData->EndMinute = atoi(&TimeBuffer[9]);
	printf("%d,%d,%d,%d\n",TimeData->StartHour,TimeData->StartMinute,TimeData->EndHour,TimeData->EndMinute);
}

static void GetCurrentTime()
{
	time_t TimeNow;
	struct tm *Temp;
	time(&TimeNow);
	Temp = localtime(&TimeNow);
	
	printf("Now is %d:%d:%d\n",Temp->tm_hour,Temp->tm_min,Temp->tm_sec);
}