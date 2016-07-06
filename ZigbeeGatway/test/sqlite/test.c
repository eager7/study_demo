#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <errno.h>
#include "JIP_Sqlite.h"
#include "time.h"//PCT
#include <unistd.h>

typedef struct _ProfileTime{
	uint8_t TimeHour;
	uint8_t TimeMin;
}ProfileTime_t;

static void StrToTime(Scene_Table_t *SceneTable, char *TimeBuffer)
{
    ProfileTime_t TimeData;
	memset(&TimeData, 0, sizeof(ProfileTime_t));
	TimeData.TimeHour = atoi(&TimeBuffer[0]);	
	TimeData.TimeMin = atoi(&TimeBuffer[3]);		
	if(TimeData.TimeMin<55)//Calculate Start Scene time
	{
		if(TimeData.TimeMin<9)
		{
			if(TimeData.TimeHour<9)
			{
				sprintf(SceneTable->TimeSpan,"0%d:0%d-0%d:0%d",
					TimeData.TimeHour, TimeData.TimeMin,
					TimeData.TimeHour, TimeData.TimeMin+1);
			}
			else
			{
				sprintf(SceneTable->TimeSpan,"%d:0%d-%d:0%d",
					TimeData.TimeHour, TimeData.TimeMin,
					TimeData.TimeHour, TimeData.TimeMin+1);
			}
		}
		else
		{
			if(TimeData.TimeHour<9)
			{
				sprintf(SceneTable->TimeSpan,"0%d:%d-0%d:%d",
					TimeData.TimeHour, TimeData.TimeMin,
					TimeData.TimeHour, TimeData.TimeMin+1);
			}
			else
			{
				sprintf(SceneTable->TimeSpan,"%d:%d-%d:%d",
					TimeData.TimeHour, TimeData.TimeMin,
					TimeData.TimeHour, TimeData.TimeMin+1);
			}
		}
	}
	else
	{
		if(TimeData.TimeHour<9)
		{
			sprintf(SceneTable->TimeSpan,"0%d:%d-0%d:%d",
				TimeData.TimeHour, TimeData.TimeMin-1,
				TimeData.TimeHour, TimeData.TimeMin);
		}
		else
		{
			sprintf(SceneTable->TimeSpan,"%d:%d-%d:%d",
				TimeData.TimeHour, TimeData.TimeMin-1,
				TimeData.TimeHour, TimeData.TimeMin);
		}
	}
}
void main()
{
	uint8_t ret = 0;
	OpenSqliteDB("SQLITE.DB");	
#if 0	
	Device_Table_t DeviceData,*DTemp,*DTemp2;
	memset(&DeviceData,0,sizeof(Device_Table_t));
	if(ret = DeviceTableSearch(&DeviceData))
	{
		CloseSqliteDB();
		DBG_vPrintf(1,"---->Error is %d\n",ret); 
	}
		
	dl_list_for_each(DTemp,&DeviceData.list,Device_Table_t,list)
	{		
		DBG_vPrintf(1, "DeviceData.IPV6 = %s\n",DTemp->DeviceIpv6);
	}
	DeviceTableSearchFree(&DeviceData);//Free Memory
	
	Profile_Table_t ProfileTableGetData, *ProfileTableTemp=NULL;
	ProfileTableSearch( &ProfileTableGetData );
	printf("dl_list_len = %d\n",dl_list_len(&ProfileTableGetData.list));
	if(0 == dl_list_len(&ProfileTableGetData.list))
	{
		printf("[]");
		ProfileTableSearchFree(&ProfileTableGetData);
	}
//////////////////////////////////////////
	Room_Table_t RoomData;
	Room_Table_t *RTemp = NULL;

	printf("RoomTableSearch = %d", RoomTableSearch(&RoomData));
	dl_list_for_each(RTemp,&RoomData.list,Room_Table_t,list)
	{		
		DBG_vPrintf(1, "Room id = %d\n",RTemp->GroupId);
	}
	RoomTableSearchFree(&RoomData);//Free Memeory

	
	Profile_Table_t ProfileTableData;
	char *devices = "0123456789012345678901234567890123456789012345678901234567890123456789111111111111111";
	ProfileTableData.ProfileTableDevices = devices;
	ProfileTableData.ProfileId = DBUnUsedProfileId();
	printf("ProfileTableData.ProfileId %d\n",ProfileTableData.ProfileId);
	ProfileTableAddRecord(&ProfileTableData);

	ProfileTableData.ProfileId = 1;
	ProfileTableOnOff(&ProfileTableData, 0);
	Scene_Table_t SceneListData, *SceneTemp = NULL;
	ProfileSceneListSearch(1, &SceneListData);
	dl_list_for_each(SceneTemp,&SceneListData.list,Scene_Table_t,list)
	{		
		printf("Scene id = %d\n",SceneTemp->SceneId);
	}
	SceneTableSearchFree(&SceneListData);//Free Memeory
	
	Device_Table_t DeviceData;
	Device_Table_t *DeviceTemp = NULL;
	ProfileDeviceListSearch(1, &DeviceData);
	
	//Remove Scene From Devices
	dl_list_for_each(DeviceTemp,&DeviceData.list,Device_Table_t,list)
	{
		printf("%s\n", DeviceTemp->DeviceIpv6);
	}
#endif
#if 0	
		Scene_Table_t SceneData;
		memset(&SceneData, 0, sizeof(Scene_Table_t));
		Scene_Table_t *SceneTemp = NULL;
		Device_Table_t DeviceData;
		memset(&DeviceData, 0, sizeof(Device_Table_t));
		Device_Table_t *DeviceTemp = NULL;

		uint8_t nProfileId = 1;	//Find Scene of Profile
		ProfileSceneListSearch(nProfileId,&SceneData);
		ProfileDeviceListSearch(nProfileId, &DeviceData);
		
		//Remove Scene From Devices
		dl_list_for_each(SceneTemp,&SceneData.list,Scene_Table_t,list)
		{
			printf("%d\n", SceneTemp->SceneId);
			dl_list_for_each(DeviceTemp,&DeviceData.list,Device_Table_t,list)
			{
				printf("%s\n", DeviceTemp->DeviceIpv6);
			}
		}
		//Delelte Scene Device List Table	
		dl_list_for_each(SceneTemp,&SceneData.list,Scene_Table_t,list)
		{
			Scene_Device_Table_t SceneDeviceData;
			memset(&SceneDeviceData, 0, sizeof(Scene_Device_Table_t));
			SceneDeviceData.SceneData.Index =  SceneTemp->Index;
			if(SceneDeviceTableDelete(&SceneDeviceData))
			{
				printf("SceneDeviceTableDelete\n");
			}
		}
		//Delete Profile Secene List Table
		dl_list_for_each(SceneTemp,&SceneData.list,Scene_Table_t,list)
		{
			Profile_List_Table_t ProfileListTableData;
			memset(&ProfileListTableData, 0, sizeof(Profile_List_Table_t));
			ProfileListTableData.SceneId.Index =  SceneTemp->Index;
			if(ProfileListTableDelete(&ProfileListTableData))
			{
				printf("ProfileListTableData\n");
			}
		}

		//Delete Scene Table
		dl_list_for_each(SceneTemp,&SceneData.list,Scene_Table_t,list)
		{
			if(SceneTableDelete(SceneTemp))
			{
				printf("SceneTableData\n");
			}
		}

		//Delete Profile Table
		Profile_Table_t ProfileTableData;
		memset(&ProfileTableData, 0, sizeof(Profile_Table_t));
		ProfileTableData.ProfileId = nProfileId;
		ProfileTableDelete(&ProfileTableData);
		//Free Memory
		SceneTableSearchFree(&SceneData);
		DeviceTableSearchFree(&DeviceData);
#endif
/*	Device_Table_t Device;
	memset(&Device, 0, sizeof(Device_Table_t));
	Scene_Table_t Scene;
	Profile_Table_t Profile;
	
	Profile.ProfileId = 1;
	ProfileTableAddRecord(&Profile);
	
	sprintf(Device.DeviceIpv6, "%s", "fd04:bd3:80e8:10:15:8d00:53:6649");
	DeviceTableAddRecord(&Device);
	
	Scene.SceneId = 2;
	SceneTableAddRecord(&Scene);
	Scene.SceneId = 3;
	SceneTableAddRecord(&Scene);

	Scene_Device_Table_t SceneDeviceData;
	memset(&SceneDeviceData, 0, sizeof(Scene_Device_Table_t));
	SceneDeviceData.SceneData.Index = 6;
	SceneDeviceData.DeviceData.Index = 5;
	//SceneDeviceTableAddRecord( &SceneDeviceData );
	
	Scene_Device_Table_t SceneDeviceData1;
	memset(&SceneDeviceData1, 0, sizeof(Scene_Device_Table_t));
	SceneDeviceData1.SceneData.Index =  6;
	if(SceneDeviceTableDelete(&SceneDeviceData1))
	{
		printf("SceneDeviceTableDelete\n");
	}*/
#if 0	
		Scene_Table_t SceneData;
		memset(&SceneData, 0, sizeof(Scene_Table_t));
		Scene_Table_t *SceneTemp = NULL;
		Device_Table_t DeviceData;
		memset(&DeviceData, 0, sizeof(Device_Table_t));
		Device_Table_t *DeviceTemp = NULL;

		uint8_t nProfileId = 1;	//Find Scene of Profile

		ProfileSceneListSearch(nProfileId,&SceneData);
		ProfileDeviceListSearch(nProfileId, &DeviceData);
		
		//Remove Scene From Devices
		dl_list_for_each(SceneTemp,&SceneData.list,Scene_Table_t,list)
		{
			printf("%d\n", SceneTemp->SceneId);
			dl_list_for_each(DeviceTemp,&DeviceData.list,Device_Table_t,list)
			{
				printf("%s\n", DeviceTemp->DeviceIpv6);
			}
		}
		//Delelte Scene Device List Table	
		dl_list_for_each(SceneTemp,&SceneData.list,Scene_Table_t,list)
		{
			Scene_Device_Table_t SceneDeviceData;
			memset(&SceneDeviceData, 0, sizeof(Scene_Device_Table_t));
			SceneDeviceData.SceneData.Index =  SceneTemp->Index;
			if(SceneDeviceTableDelete(&SceneDeviceData))
			{
				printf("SceneDeviceTableDelete\n");
			}
		}
		//Delete Profile Secene List Table
		//dl_list_for_each(SceneTemp,&SceneData.list,Scene_Table_t,list)
		{
			Profile_List_Table_t ProfileListTableData;
			memset(&ProfileListTableData, 0, sizeof(Profile_List_Table_t));
			ProfileListTableData.ProfileId.Index =  nProfileId;
			printf("111\n");
			if(ProfileListTableDelete(&ProfileListTableData))
			{
				printf("ProfileListTableData\n");
			}
		}
		//Delete Scene Table
		dl_list_for_each(SceneTemp,&SceneData.list,Scene_Table_t,list)
		{
			if(SceneTableDelete(SceneTemp))
			{
				printf("SceneTableData\n");
			}
		}

		//Delete Profile Table
		Profile_Table_t ProfileTableData;
		memset(&ProfileTableData, 0, sizeof(Profile_Table_t));
		ProfileTableData.ProfileId = nProfileId;
		ProfileTableDelete(&ProfileTableData);
		//Free Memory
		SceneTableSearchFree(&SceneData);
		DeviceTableSearchFree(&DeviceData);
#endif		
#if 0		
	char * deviceipv6 = "fd04:bd3:80e8:10:15:8d00:53:6649";
	Device_Table_t Device;
	memset(&Device, 0, sizeof(Device_Table_t));
	sprintf(Device.DeviceIpv6,"%s", deviceipv6);
	DeviceTableSearchOne(&Device);
	printf("%d\n",Device.Index);
#endif
#if 0
	Room_Table_t RoomTableData, *RoomTableTemp = NULL;
	memset(&RoomTableData, 0, sizeof(Room_Table_t));
	RoomTableSearch(&RoomTableData);
	dl_list_for_each(RoomTableTemp,&RoomTableData.list,Room_Table_t,list)
	{
		Device_Table_t DeviceTableData, *DeviceTempData = NULL;
		memset(&DeviceTableData, 0, sizeof(Device_Table_t));
		RoomDeviceListSearch(RoomTableTemp->GroupId,&DeviceTableData);
		if(0 == dl_list_len(&DeviceTableData.list))
		{
			printf("0\n");	
		}
		dl_list_for_each(DeviceTempData,&DeviceTableData.list,Device_Table_t,list)
		{
			//if(!strcmp(DeviceTempData->DeviceIpv6,json_object_to_json_string(psJsonNodeAddress)))
			{
			printf("1\n");	
			}
			//else
			{
			printf("0\n");	
			}
		}
	}
#endif
#if 0
  	Device_Table_t DeviceData;	
	memset(&DeviceData, 0, sizeof(Device_Table_t));
	char acNameBuf[15] = "Mono Lamp BF39";
	memcpy(DeviceData.DeviceName,acNameBuf,sizeof(acNameBuf));//PCT 
	DeviceData.DeviceType = 1;
	DeviceData.DeviceIsOnline = 1;
	DeviceTableAddRecord(&DeviceData);
#endif
#if 0
	//while(1)
	{
		uint32_t i = 0;
		time_t TNow;	
		struct tm *TimeNow;	
		time(&TNow);	
		TimeNow = localtime(&TNow);	//Get time now	

		const uint16_t StartYear = 1900+TimeNow->tm_year;
		const uint8_t StartMonth = 1+TimeNow->tm_mon;
		const uint8_t StartDay = TimeNow->tm_mday;
		const uint8_t StartHour = TimeNow->tm_hour;
		const uint8_t StartMin = TimeNow->tm_min;

		uint16_t EndYear = 1900+TimeNow->tm_year;
		uint8_t EndMonth = 1+TimeNow->tm_mon;
		uint8_t EndDay = TimeNow->tm_mday;
		uint8_t EndHour = TimeNow->tm_hour;
		uint8_t EndMin = TimeNow->tm_min;	
	printf("%d-%d-%d-%d-%d\n",StartYear,StartMonth,StartDay,StartHour,StartMin);
	printf("%d-%d-%d-%d-%d\n",EndYear,EndMonth,EndDay,EndHour,EndMin);

		uint64_t GuardTime = 	(StartYear - EndYear)*12*30*24*60+
								(StartMonth - EndMonth)*30*24*60+
								(StartDay - EndDay)*24*60+
								(StartHour - EndHour)*60+
								(StartMin - EndMin);
		printf("%llu", GuardTime);

	}
#endif
#if 0
	char *ProfileTime = "02:56-02:59";
	Scene_Table_t SceneTableStart, SceneTableEnd;//Create Start & End Scene
	StrToTime(&SceneTableStart, &ProfileTime[0]);//Convert Time fome char to int
	StrToTime(&SceneTableEnd, &ProfileTime[6]);//Convert Time fome char to int

	printf("%s----%s\n",SceneTableStart.TimeSpan,SceneTableEnd.TimeSpan);

	printf("%02d\n", 22);

	while(NULL)
	{
		sleep(2);
		printf("run...\n");
	}
#endif
#if 0
	Profile_Table_t ProfileTableGetData, *ProfileTableTemp=NULL;
	memset(&ProfileTableGetData, 0, sizeof(Profile_Table_t));
	ProfileTableSearch( &ProfileTableGetData );//Create Profile Table List
	printf("dl_list_len = %d\n",dl_list_len(&ProfileTableGetData.list));
	
	dl_list_for_each( ProfileTableTemp,&ProfileTableGetData.list, Profile_Table_t, list )
	{
		printf("%s\n",ProfileTableTemp->ProfileName);
	}
#endif

#if 1
	char *pcNodeAddress = "fd04:bd3:80e8:10:15:8d00:53:6624";
            Device_Table_t DeviceTableGetValue = {0};
            sprintf(DeviceTableGetValue.DeviceIpv6, "%s", pcNodeAddress);
            if(DeviceTableSearchOne(&DeviceTableGetValue))
            {
                sprintf(pcDescription, 
                       "{\"IPv6Address\": %s,\"DeviceID\": %d,%s}", 
                       DeviceTableGetValue.DeviceIpv6, DeviceTableGetValue.DeviceId, DeviceTableGetValue.DeviceAppendstr); 
            }

#endif


	
	CloseSqliteDB();	

	printf("Test Finish...\n");
}
