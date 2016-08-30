#include <stdio.h>
#include <string.h>
#include "json_object.h"
#include "bits.h"
struct json_object* json_tokener_parse(const char *str);

#define UI_BACK_BLUE(x) "\e[0;37;41m"x"\e[0m" //white font,red background
#define DBG_vPrintf(a,b,ARGS...) do {  if (a) printf(UI_BACK_BLUE("[PCT-LED-%d]") b, __LINE__,## ARGS); } while(0)
	
int main (int argc, char *argv[])
{
#if 0
	char * profile_id = "0001";
	char * profile_name = "1234";
	char * profile_time_start = "11:00";
	char * profile_time_end = "12:00";
	char * profile_time_repeat = "{\"time_repeat_week_value\":0,\"time_repeat_type\":0}";
	char * profile_devices = "[{\"device_brightness\":80,\"device_onoff\":1,\"device_id\":134289232,\"device_ipv6\":\"fd04:bd3:80e8:10:15:8d00:53:665a\"}]";

	struct json_object* psJson;
	psJson = json_object_new_object();
	
	json_object_object_add(psJson,"profile_id",json_object_new_string(profile_id));
	json_object_object_add(psJson,"profile_name",json_object_new_string(profile_name));
	json_object_object_add(psJson,"profile_time_start",json_object_new_string(profile_time_start));
	json_object_object_add(psJson,"profile_time_end",json_object_new_string(profile_time_end));
	json_object_object_add(psJson,"profile_time_repeat",json_object_new_string(profile_time_repeat));
	json_object_object_add(psJson,"profile_devices",json_object_new_string(profile_devices));
	
	struct json_object* psJson2;
	psJson2 = json_object_new_object();
	
	json_object_object_add(psJson2,"profile_id",json_object_new_string(profile_id));
	json_object_object_add(psJson2,"profile_name",json_object_new_string(profile_name));
	json_object_object_add(psJson2,"profile_time_start",json_object_new_string(profile_time_start));
	json_object_object_add(psJson2,"profile_time_end",json_object_new_string(profile_time_end));
	json_object_object_add(psJson2,"profile_time_repeat",json_object_new_string(profile_time_repeat));
	json_object_object_add(psJson2,"profile_devices",json_object_new_string(profile_devices));

	struct json_object* JsonArry = json_object_new_array();
	json_object_array_add(JsonArry, psJson);
	json_object_array_add(JsonArry, psJson2);
	
	printf("Json = %s\n", json_object_to_json_string(JsonArry));
#endif	
	
#if 0	
	printf("\n\n******************************************************\n\n");
	struct json_object* psJsonProfileDetailResult = json_object_new_object();
	char *test1 = "{\"time_repeat_week_value\":0,\"time_repeat_type\":0}";
	char *test2 = "[{\"device_brightness\":80,\"device_onoff\":1,\"device_id\":134289232,\"device_ipv6\":\"fd04:bd3:80e8:10:15:8d00:53:6649\"}]";
	printf("%s\n", test1);
	printf("%s\n", test2);
	
	printf("%s\n", json_object_to_json_string(json_object_new_string(test1)));
	printf("%s\n", json_object_get_string(json_object_new_string(test1)));
	printf("%s\n", json_object_to_json_string(json_object_new_string(test2)));
	printf("%s\n", json_object_get_string(json_object_new_string(test2)));
	
	json_object_object_add(psJsonProfileDetailResult,"profile_time_repeat",json_object_new_string(test1));
	json_object_object_add(psJsonProfileDetailResult,"profile_devices",json_object_new_string(test2));
	printf("%s\n",json_object_get_string(psJsonProfileDetailResult));
	/**
	***执行结果：
	******************************************************

    {"time_repeat_week_value":0,"time_repeat_type":0}
    [{"device_brightness":80,"device_onoff":1,"device_id":134289232,"device_ipv6":"fd04:bd3:80e8:10:15:8d00:53:6649"}]
	"{\"time_repeat_week_value\":0,\"time_repeat_type\":0}"
	{"time_repeat_week_value":0,"time_repeat_type":0}
	"[{\"device_brightness\":80,\"device_onoff\":1,\"device_id\":134289232,\"device_ipv6\":\"fd04:bd3:80e8:10:15:8d00:53:6649\"}]"
	[{"device_brightness":80,"device_onoff":1,"device_id":134289232,"device_ipv6":"fd04:bd3:80e8:10:15:8d00:53:6649"}]
	{ "profile_time_repeat": "{\"time_repeat_week_value\":0,\"time_repeat_type\":0}", "profile_devices": "[{\"device_brightness\":80,\"device_onoff\":1,\"device_id\":134289232,\"device_ipv6\":\"fd04:bd3:80e8:10:15:8d00:53:6649\"}]" }
	**/
#endif
#if 0	
	char * UpdateValue = "0001";
	printf("UpdateValue = %d\n", json_object_get_int(json_tokener_parse(UpdateValue)));
#endif
	
#if 0	
	char * OnOff = "1";
	printf("UpdateValue = %d\n", json_object_get_int(json_tokener_parse(OnOff)));
	printf("Length = %d\n", json_object_object_length(json_object_new_string(OnOff)));
#endif
#if 0	
	char * StrResult = " { \"Nodes\": [ { \"IPv6Address\": \"fd04:bd3:80e8:10:15:8d00:53:662f\", \"IsOtherGroup\": 1, \"DeviceID\": -2147413640, \"MiBs\": [ { \"ID\": 536870657, \"Name\": \"Sensor\", \"Vars\": [ { \"Name\": \"Temperature\", \"Index\": 0, \"Type\": 5, \"AccessType\": 2, \"Security\": 0, \"Value\": 20.000000 } ] } ] } ] }";
	
	struct json_object* JsonArryNodes = json_object_object_get(json_tokener_parse(StrResult),"Nodes");
	struct json_object* JsonArryMibs = json_object_object_get(json_object_array_get_idx(JsonArryNodes,0), "MiBs");
	struct json_object* JsonArryVars = json_object_object_get(json_object_array_get_idx(JsonArryMibs,0), "Vars");
	struct json_object* JsonObjectValue = json_object_object_get(json_object_array_get_idx(JsonArryVars,0), "Value");
	int Value = json_object_get_int(JsonObjectValue);
	printf("%s\n",json_object_get_string(JsonObjectValue));
	printf("%d\n",strlen(json_object_get_string(JsonArryMibs)));

#endif
#if 0	
	char * StrResult = " { \"linkage_device_list\": []}";
	struct json_object* JsonArryNodes = json_tokener_parse(StrResult);
	printf("%s\n",json_object_get_string(JsonArryNodes));
	printf("%d\n",strlen(json_object_get_string(JsonArryNodes)));
#endif

#if 0	
    struct json_object* psJsonBRList = json_object_new_array();
	json_object_array_add(psJsonBRList, json_object_new_string("panchangtao"));
	struct json_object* psJsonResult = json_object_new_object();
	json_object_object_add (psJsonResult,"BRList",psJsonBRList);
	printf("%s\n",json_object_get_string(psJsonResult));
#endif
#if 0
	int a;
	unsigned int b;
	long c;
	char *strcc = "{\"device_id\":2147553656}";
	struct json_object* psJsonCC = json_tokener_parse(strcc);
	printf("%llu\n", json_object_get_int64(json_object_object_get(psJsonCC,"device_id")));
	a = json_object_get_int64(json_object_object_get(psJsonCC,"device_id"));
	b = json_object_get_int64(json_object_object_get(psJsonCC,"device_id"));
	c = json_object_get_int64(json_object_object_get(psJsonCC,"device_id"));
	printf("a = %d, b = %llu, c = %ld\n",a,b,c);
#endif
#if 0
	int a;
	unsigned int b;
	long c;
	char acDevaddr[50]={0};
	char *acResult = 
	"{\"device_lists\": [ { \"device_id\": 134287733, \"device_ipv6\": \"fd04:bd3:80e8:10:15:8d00:53:6644\","
	" \"device_is_online\": 0 }, { \"device_id\": 2147553656, \"device_ipv6\": \"fd04:bd3:80e8:10:15:8d00:53:663b\","
	" \"device_is_online\": 1 }, { \"device_id\": 2147553664, \"device_ipv6\": \"fd04:bd3:80e8:10:15:8d00:53:6636\","
	" \"device_is_online\": 0 }, { \"device_id\": 134287733, \"device_ipv6\": \"fd04:bd3:80e8:10:15:8d00:53:6617\","
	" \"device_is_online\": 0 } ]}";
		unsigned char i,j;
		
	char *testbuf = 
	"{ \"Status\": { \"Value\": 255, \"Description\": \"JIP discover network failed\" } }";
	struct json_object *psResult = json_tokener_parse(testbuf);
	struct json_object *psDeviceList = json_object_object_get(psResult, "device_lists");
	if (psDeviceList == NULL)
	{
		printf("Data Error\n");
		return 0;
	}
	j = json_object_array_length(psDeviceList);
	printf("Device list = %s\n num is [%d]\n", json_object_to_json_string(psDeviceList), j);
	
	for (i = 0; i < j; i++)
	{
		printf("loop [%d]\n",i);
		unsigned int iZGBdeviceId;	
		int ionline;
		
		struct json_object *psDevice = json_object_array_get_idx(psDeviceList, i);
		sprintf(acDevaddr, "%s",json_object_get_string(json_object_object_get(psDevice, "device_ipv6")));	
		
		ionline = json_object_get_int(json_object_object_get(psDevice, "device_is_online"));
		
		iZGBdeviceId = json_object_get_int64(json_object_object_get(psDevice, "device_id"));
		printf("deviceid [%llu], deviceipv6 [%s]\n",iZGBdeviceId,acDevaddr);
	}

#endif
#if 0	
	char *strAlljoyn = "{\"command\":\"about\",\"GUID\":\"123456\",\"name\":\"ms1\",\"description\":\"\",\"language\":\"english\",\"interface\":\"00\"}";
    struct json_object *jsonAlljyonMessage = json_tokener_parse(strAlljoyn);
	if(is_error(jsonAlljyonMessage))
	{
		printf("can't parse msg\n");
		return 1;
	}
	printf("%s\n",json_object_get_string(jsonAlljyonMessage));

	struct json_object *jsonCommand = json_object_object_get(jsonAlljyonMessage, "command");
	printf("%s\n",json_object_get_string(jsonCommand));
	
	const char * sGuid = json_object_get_string(json_object_object_get(jsonAlljyonMessage, "GUID"));
	printf("sGuid = %s,\t\n",sGuid);
	json_object_put(jsonAlljyonMessage);
#endif

#if 0
	DBG_vPrintf(1,"hello\n");
	char *str = "{\"command\":6,\"sequence\":6,\"device_address\":\"6066005655905860\",\"group_id\":0}";
	
	json_object *psJsonMessage, *psJsonTemp = NULL;
	
	/*psJsonMessage = json_object_new_object();
	json_object_object_add(psJsonMessage,"command",json_object_new_int(6));
	json_object_object_add(psJsonMessage,"sequence",json_object_new_int(6));
	json_object_object_add(psJsonMessage,"device_address",json_object_new_int64(6066005655905860));
	json_object_object_add(psJsonMessage,"group_id",json_object_new_int(0));*/
	
	if(NULL != (psJsonMessage = json_tokener_parse(str)))
	{
		printf("%s\n", json_object_get_string(psJsonMessage));
		if(NULL != (psJsonTemp = json_object_object_get(psJsonMessage,"device_address")))
		{
			printf("%s\n", json_object_get_string(psJsonTemp));
			long long u64DeviceAddress = json_object_get_int64(psJsonTemp);
			printf("%016llX\n", u64DeviceAddress);
			printf("%d\n", u64DeviceAddress == 0);
		}
		if(NULL != (psJsonTemp = json_object_object_get(psJsonMessage,"command")))
		{
			printf("%s\n", json_object_get_string(psJsonTemp));
			long long u64DeviceAddress = json_object_get_int64(psJsonTemp);
			printf("%lld\n", u64DeviceAddress);
		}
		DBG_vPrintf(1, "ResponseMessageError error, cant't get device_address\n");
	}
#endif

#if 1
	char *str = "{\"command\":6,\"sequence\":6,\"device_address\":\"6066005655905860\",\"group_id\":\"aaaaaaaaaaaaaaaa\"}";
	
	json_object *pJsonTest = NULL, *pJsonAddr = NULL;

	pJsonTest = json_tokener_parse(str);
	printf("%s\n", json_object_get_string(pJsonTest));
	
	pJsonAddr = json_object_object_get(pJsonTest,"device_address");
	printf("Length = %ld\n", strlen(json_object_get_string(pJsonTest)));
	json_object_put(pJsonTest);
	json_object_put(pJsonAddr);

#endif
	return 0;
}