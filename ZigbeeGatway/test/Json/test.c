#include <stdio.h>
#include "json_object.h"
struct json_object* json_tokener_parse(const char *str);

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

#if 1	
    struct json_object* psJsonBRList = json_object_new_array();
	json_object_array_add(psJsonBRList, json_object_new_string("panchangtao"));
	struct json_object* psJsonResult = json_object_new_object();
	json_object_object_add (psJsonResult,"BRList",psJsonBRList);
	printf("%s\n",json_object_get_string(psJsonResult));
#endif
	
	
	return 0;
}