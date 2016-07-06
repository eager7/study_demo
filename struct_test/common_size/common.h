/*********************************************************************************
*Copyright(C),2010-2020, 西安TCL软件开发有限公司
*FileName:  Common.h
*Author:	Li Peng
*Version:	V0.1
*Date:  //完成日期
*Description: 定义了一些common属性等内容
*History:  
1.Date:		2015.03.24
Author:		Li Peng
Modification:	初始版本
**********************************************************************************/

#ifndef _COMMON_H_  
#define _COMMON_H_  

// system include
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
#include <netdb.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/un.h>
#include <getopt.h>
#include <fcntl.h>

// add include
//#include "json/json.h"

#include "list.h"

#define CC_SOFTWARE_VERSION   "smarthome-v4.1"
//#define CC_SOFTWARE_VERSION   "smarthome-v12230939" 

// enable or disable the controlpoint
#define  CP_TONLYELE_ENABLE     1
#define  CP_BROADLINK_ENABLE    1
#define  CP_NEWGREEN_ENABLE     1
#define  CP_ZIGBBE_ENABLE       1
#define  CP_NETVIEW_ENABLE      1
#define  CP_ZWAVE_ENABLE        1
#define  CP_ALLJOYN_ENABLE      1
#define  CP_BLE_ENABLE          1
#define  CP_HUOHETECH_ENABLE    1
#define  CP_WISE_ENABLE         1

// for Log output 
#define   TL_PRINTF2FILE   0

#if  TL_PRINTF2FILE 
#define TL_DBG_Printf(a,b,ARGS...) do{                                    \
    if(a)                                                               \
    {                                                                   \
        FILE *fp=fopen("/tmp/smarthome_log.txt","a+");                      \
        fprintf(fp,"\n------%s(%s)-------,%d\n",__FUNCTION__,__TIME__,__LINE__);   \
        fprintf(fp,b,## ARGS);                                          \
        fclose(fp);                                                     \
    }                                                                   \
}while(0)
#else
#define TL_UI_BLUE(x) 	    "\e[34;1m"x"\e[0m"
#define TL_UI_GREEN(x) 	    "\e[32;1m"x"\e[0m"
#define TL_UI_YELLOW(x)     "\e[33;40;1m"x"\e[0m"
#define TL_UI_RED(x)	            "\e[31;1m"x"\e[0m"
#define TL_UI_BACK_BLUE(x) "\e[0;37;41m"x"\e[0m" 
//#define TL_DBG_Printf(a,b,ARGS...) do {  if (a) printf(TL_UI_BLUE("[TONLY]") b, ## ARGS); } while(0)
#define TL_DBG_Printf(a,b,ARGS...) do {  if (a) printf(TL_UI_GREEN("%s(%d):") b,__FUNCTION__,__LINE__, ## ARGS); } while(0)
/*
#define TL_DBG_Printf(a,b,ARGS...) do{                                    \
    if(a)                                                               \
    {                                                                   \
        printf("\e[34;1m%s%s\e[0m", __FUNCTION__,"() ");   \
        printf(b, ## ARGS);\
    }                                                                   \
}while(0)
*/
#endif // TL_PRINTF2FILE

#define   TL_NET_PRINTF2FILE    0
#if  TL_NET_PRINTF2FILE 
#define TL_DBG_NET_Printf(a,b,ARGS...) do{                                    \
    if(a)                                                               \
    {                                                                   \
        FILE *fp=fopen("/tmp/smarthome_net_log.txt","a+");                      \
        fprintf(fp,"\n------%s(%s)-------,%d\n",__FUNCTION__,__TIME__,__LINE__);   \
        fprintf(fp,b,## ARGS);                                          \
        fclose(fp);                                                     \
    }                                                                   \
}while(0)
#else
#define TL_DBG_NET_Printf   TL_DBG_Printf
#endif
typedef unsigned char bool;
//begin add by anchungang
enum DBGTYPE
{
	E=1,
	W=2,
	I=4,
	D=8
};
#define BLE_MODE
#ifdef BLE_MODE
#define MODE      "[BLE_MODE]"
#endif
#define DBGSWITCH (E | W | I |D)        //DUG  LEAVE SWITCH
#define ERR   (E & DBGSWITCH)
#define WAR   (W & DBGSWITCH)
#define INFO  (I & DBGSWITCH)
#define DBG   (D & DBGSWITCH)
#define ENTER "\r\n"

#define EDBG(TYPE,FORMAT,...) ((ERR)?  printf (TL_UI_RED("%s[E]-->") "%s:"FORMAT"%s",MODE,TYPE,##__VA_ARGS__,ENTER):0)
#define WDBG(TYPE,FORMAT,...) ((WAR)?  printf (TL_UI_YELLOW("%s[W]-->") "%s:"FORMAT"%s",MODE,TYPE,##__VA_ARGS__,ENTER):0)
#define IDBG(TYPE,FORMAT,...) ((INFO)? printf (TL_UI_GREEN("%s[I]-->") "%s:"FORMAT"%s",MODE,TYPE,##__VA_ARGS__,ENTER):0)
#define DDBG(TYPE,FORMAT,...) ((DBG)?  printf (TL_UI_BACK_BLUE("%s[D]-->") "%s:"FORMAT"%s",MODE,TYPE,##__VA_ARGS__,ENTER):0)

//add by anchungang


#define 	_TL_DEBUG_CONTROLCENTER      0
#define 	_TL_DEBUG_DEVMANAGE      0
#define 	_TL_DEBUG_CONTROLPOINT     0
#define 	_TL_DEBUG_NETWORK               0
#define 	_TL_DEBUG_NETACCMANAGER      0
#define 	_TL_DEBUG_REMOTESERVER      0
#define 	_TL_DEBUG_THIRDSERVER      0
#define 	_TL_DEBUG_IRSTATION      0
#define 	_TL_DEBUG_NETVIEW      0
#define 	_TL_DEBUG_BLE      0

//  for debug 
#define 	_TL_DEBUG_CTROL      0
#define 	_TL_DEBUG_SOCKET    0
#define 	_TL_DEBUG_THREAD    0
#define 	_TL_DEBUG_JSON		0
#define 	_TL_LOG_CTROL          1
#define 	_TL_LOG_JSON          1
#define 	_TL_BLE_DEBUG_ENABLE          1


#define ETH_NAME    "eth0.2" 

//#define ETH_01_NAME        "eth0.1" 

#define ETH_01_NAME        "br-lan"
#define ETH_02_NAME        "eth0.2" 
#define ETH_03_NAME        "apcli0"


#define  SOCKET_AF_UNIX_AF_INET          0    //  0:AF_UNIX     1:AF_INET 
#define  SOCKET_CC_NET_ALONE              1    //  0:old-together    1:new-alone

// FOR Sensor max an min value  define
#define  TL_SENSOR_TP_MAX	100
#define  TL_SENSOR_TP_MIN	(-45)
#define  TL_SENSOR_HUM_MAX	200
#define  TL_SENSOR_HUM_MIN	0

// FOR alarm bit define
// 0:tp,1:hum,2:smork,3:move,4:noise;
#define  TL_ALARM_BIT_TP  	0x0001
#define  TL_ALARM_BIT_HUM	0x0002
#define  TL_ALARM_BIT_SMORK	0x0004
#define  TL_ALARM_BIT_MOVE	    0x0008
#define  TL_ALARM_BIT_NOISE	    0x0010
#define  TL_ALARM_BIT_WLEAK	    0x0020  
#define  TL_ALARM_BIT_BUTTON	0x0040  
#define  TL_ALARM_BIT_PIR	    0x0080  
#define  TL_ALARM_BIT_DOOR_OPEN	0x0100  
#define  TL_ALARM_BIT_DOOR_CLS	0x0200  
#define  TL_ALARM_BIT_PM25	    0x0400  
#define  TL_ALARM_BIT_BABY	    0x0800	

#define ALARM_SCENES_RETRY_TIMES  3
typedef enum enAlarmType
{
    ALARM_TYPE_TP = 1,
    ALARM_TYPE_HUM,
    ALARM_TYPE_SMORK,
    ALARM_TYPE_MOVE,
    ALARM_TYPE_NOISE,
    ALARM_TYPE_WLEAK,
    ALARM_TYPE_BUTTON,
    ALARM_TYPE_PIR,
    ALARM_TYPE_DOOR_OPEN,
    ALARM_TYPE_DOOR_CLS,
    ALARM_TYPE_PM25,
    ALARM_TYPE_BABY,
    ALARM_TYPE_BUTT
}ALARM_TYPE_EN;

#define BUTTON_SHORT_PRESS      1
#define BUTTON_LONG_PRESS       2

#define  TL_ALARM_MASK_WISE (TL_ALARM_BIT_WLEAK|TL_ALARM_BIT_BUTTON|TL_ALARM_BIT_PIR|TL_ALARM_BIT_DOOR_OPEN|TL_ALARM_BIT_DOOR_CLS)  


/* 以下两个宏定义了alarm时，对APP和SPEAKER发送失败的记录位 */
#define  TL_ALARM_TO_APP_FAILED	    0x0001
#define  TL_ALARM_TO_SPK_FAILED	    0x0100

// Socket
#define CC_OM_SERVER_PORT    6666  
#define CC_OM_LISTEN_QUEUE   20 
#define  SOCKET_CONNECT_MAX   60
#define CC_UNIX_SOCKET_FILE    "/tmp/smarthome_socket_file"  
//#define  SCENCETESTMODE

/* DMC和CC通信的端口 */
#define DMC_SERVER_PORT             6688  
#define DMC_COMMU_DATA_LEN          64  
#define DMC_SERVER_LISTEN_QUEUE     20 

#define HTTP_PORT   80

// String Max len 
#define MAC_ADDR_LENGTH	   64  // mac =18、uuid = 64
#define IP_ADDR_LENGTH	          INET_ADDRSTRLEN
#define IPV6_ADDR_LENGTH	   INET6_ADDRSTRLEN
#define BT_DEVICE_UUID_LENGTH 33//the length of uuid is 16 bytes, + 1
#define DEV_ID_LENGTH	       37
#define DEV_TYPE_LENGTH	      32
#define CONTROL_POINT_LENGTH	      32
#define DEVICE_NAME_LENGTH	64
#define KEY_LENGTH_MAX	 32
#define NICK_NAME_LENGTH	64
// Device List, Json string
#define DEV_CNT_MAX    50

#define RC_IRKEY_LENGTH	       37

#define JSON_STRING_LEN_MAX    (1024*25)

#define  SOCKET_BUFF_MAX   1024
#define  MESSAGES_BUFF_MAX   (2048)
#define  KEYCODES_BUFF_MAX   (4096)

#define BRAND_ID_LENGTH	       256

/* 按照协议文档给出的字符串的结果，timespan字段表示的时间段为24小时制，格式为XX:XX-XX:XX */    
#define TIMESPAN_TOTAL_LEN  30
#define TIMESPAN_HOUR_LEN   2
#define TIMESPAN_MINUTE_LEN 2

#define CONTROL_DEV_ID_LEN  32

#define BOOL_TURE       (bool)1 
#define BOOL_FALSE      (bool)0 

#define SYSTEM_TM_LENGTH          32   // YYYY-MM-DD:hh-mm-ss:ms

// Json Packet key defined 
#define  TL_JSON_KEY_COMMOND		"command"
#define  TL_JSON_KEY_COMMONDID		"commandId"
#define  TL_JSON_KEY_CONRTOLPOINT	"controlPoint"
#define  TL_JSON_KEY_TYPE		"type"
#define  TL_JSON_KEY_RESULT	"result"
#define  TL_JSON_KEY_VOLUME	"volume"
#define  TL_JSON_KEY_TEMP		"temperature"
#define  TL_JSON_KEY_HUMIDITY	"humidity"
#define  TL_JSON_KEY_DEVICEPROPERTIES		"deviceProperties"
#define  TL_JSON_KEY_DEVICES_LIST	"device-list"
#define  TL_JSON_KEY_DEVICENAME	"deviceName"
#define  TL_JSON_KEY_DBLE_EVICENAME	"device_name"
#define  TL_JSON_KEY_NICKENAME	"nickName"
#define  TL_JSON_KEY_ACTION		"action"
#define  TL_JSON_KEY_SCENCE		"scene"
#define  TL_JSON_KEY_DEVICEID		"deviceId"
#define  TL_JSON_KEY_BLE_DEVICEID		"device_ID"
#define  TL_JSON_KEY_BLE_DEVICE_ID		"device_id"
#define  TL_JSON_KEY_DEVICETYPE	"deviceType"
#define  TL_JSON_KEY_DEVICEIP		"deviceIp"
#define  TL_JSON_KEY_DEVICEMAC		"deviceMac"
#define  TL_JSON_KEY_BLE_DEVICEMAC		"device_address"
#define  TL_JSON_KEY_DEVICEIPV6	"deviceIpv6"
#define  TL_JSON_KEY_DEVICEONOFF	"deviceOnOff"
#define  TL_JSON_KEY_DEVICEONLINE	"deviceOnline"
#define  TL_JSON_KEY_CONTROLLERIP		"controllerIp"
#define  TL_JSON_KEY_CONTROLLERMAC	"controllerMac"
#define  TL_JSON_KEY_CONTROLLERIPV6		"controllerIpv6"
#define  TL_JSON_KEY_CONTROLLERONLINE	"controllerOnline"
#define  TL_JSON_KEY_SOFTWARE_VERSION	"software-version"
#define  TL_JSON_KEY_MESH_TYPE	"deviceAssociateType"
#define  TL_JSON_KEY_MESH_DEV_STATUS_TYPE	"device_status"
#define  TL_JSON_KEY_STATUS_SCAN_END	"scan_end"
#define  TL_JSON_KEY_IS_NET   "is_net"
#define  TL_JSON_KEY_DEV_UUID   "uuid"
#define  TL_JSON_KEY_GET_INNER_BT_MAC "getInnerBleMac"
#define  TL_JSON_KEY_GET_INNER_BT_STATUS "ble_usage"
#define  TL_JSON_KEY_INNER_MAC "inner_mac"

#define  TL_JSON_KEY_DATA		"data"
#define  TL_JSON_KEY_MESSAGES	               "message"
#define  TL_JSON_KEY_IRCODES	               "ircode"
#define  TL_JSON_KEY_RCKEY	                     "rckeys"
#define  TL_JSON_KEY_OWNERDEVICEID       "ownerdeviceId"
#define  TL_JSON_KEY_BRANDNAME       "brandname"
#define  TL_JSON_KEY_DEVTYPENO       "devicetypeno"
#define  TL_JSON_KEY_DEVICEVIRTUALMAC  "virtualMAC"
#define  TL_JSON_KEY_DEVICEIRKEYCODES  "deviceIrKeyCodes"
#define  TL_JSON_KEY_SCENCENAME  "sceneName"

#define  TL_JSON_KEY_DEVTRIGGERVAL  "devicetriggerval"
#define  TL_JSON_KEY_TRIGGERTIME      "triggertime"
#define  TL_JSON_KEY_TRIGGERTIMEREPEATMODE  "triggertimerepeatmode"

#define  TL_JSON_KEY_SCENCETYPE   "scenetype"
#define  TL_JSON_KEY_DEVICEVALUE	"deviceValue"

#define  TL_JSON_KEY_SCENE_LIST	"scene-list"
#define  TL_JSON_KEY_CONTROL_CENTER	"controlcenter"
#define  TL_JSON_KEY_STATUS "status"

#define  TL_JSON_KEY_SCENE_DISENABLE	"disenable"
#define  TL_JSON_KEY_LIGHT_LUM	   "lightlum"
#define  TL_JSON_KEY_LIGHT_CLRR  "lightclrR"
#define  TL_JSON_KEY_LIGHT_CLRG  "lightclrG"
#define  TL_JSON_KEY_LIGHT_CLRB  "lightclrB"
#define  TL_JSON_KEY_LIGHT_HSV  "lightHSV"
#define  TL_JSON_KEY_LIGHT_TONE  "lightTone"
#define  TL_JSON_KEY_PROPERTIES_TYPE		"PropertiesType"

#define  TL_JSON_KEY_CONTROLPOINTID	   "controlpointId"
#define  TL_JSON_KEY_USERNAME	   "username"
#define  TL_JSON_KEY_PASSWORD	   "password"
#define  TL_JSON_KEY_USERMOBILE	   "usermobile"
#define  TL_JSON_KEY_OTHER	   "other"

#define  TL_JSON_BLE_DEV_TYPE_WB	   "warmcoldbulb"
#define  TL_JSON_BLE_DEV_TYPE_DS	   "doorsensor"
#define  TL_JSON_BLE_DEV_TYPE_LS	   "leaksensor"
#define  TL_JSON_BLE_DEV_TYPE_SS	   "switchsensor"
#define  TL_JSON_BLE_DEV_TYPE_PS	   "pm25sensor"
#define  TL_JSON_BLE_DEV_TYPE_GS	   "gassensor"
#define  TL_JSON_BLE_DEV_TYPE_PIRS	   "pirsensor"
#define  TL_JSON_BLE_DEV_TYPE_TS	   "tempsensor"
#define  TL_JSON_BLE_DEV_TYPE_BS	   "babysensor"
#define  TL_JSON_BLE_DEV_TYPE_BRI	   "bridge"



#define  TL_JSON_KEY_MASTERCONTROLERID "mastercontrolerid"

#define  TL_JSON_KEY_DEVICEUUID "deviceUuid"

#define  MASTER_CONTROLER_ID_LEN      64


// Scence not NULL:       controlcenter  scene  sceneName
// ScenceDev not NULL:  controlcenter  scene  deviceId
#define  BACKUP_TO_HTTP_SERVER_ENABLE      0


// ################################
#pragma pack(1)

typedef enum _TL_ControlPointType
{
	CP_TONLYELE = 0,
	CP_BROADLINK,
	CP_NEWGREEN,	
	CP_ZIGBBE,
	CP_ZWAVE,
	CP_ALLJOYN,
	CP_NETVIEW,
	CP_BLE,
	CP_HUOHE,
    #if  CP_WISE_ENABLE
    CP_WISE,
    #endif /* #if CP_WISE_ENABLE */
    
	CONTROL_POINT_CNT,	
}TL_ControlPointType;

typedef enum _TL_DeviceType
{
    DEV_UNKOWN = 0,
    DEV_LAMP,
    DEV_LAMPRGB,	
    DEV_OUTLET,
    DEV_CAMERA,
    DEV_THSENSOR,	
    DEV_THSENSOR_NEW,   //PCT
    DEV_SMOKEDETECTOR,
    DEV_SMARTPLUG,
    DEV_LIGHTSENSOR, 
    DEV_LAMPBICOLOUR,  
    DEV_PLAYER,
    DEV_IRSTATION, 
    DEV_TV,   
    DEV_DVD, 
    DEV_AC, 
    DEV_PROJECTOR, 
    DEV_BOX,  
    DEV_CONTROLPANEL,
    #if CP_BLE_ENABLE
    DEV_BLE_MESH_SWITCH,
    DEV_BLE_MESH_LIGHT,
    DEV_BLE_PIR_SENSOR,
    DEV_BLE_MAGNET_SENSOR,
    DEV_BLE_HOURGLASS_SENSOR,
    DEV_BLE_PM_SENSOR,
    DEV_BLE_TEMP_SENSOR,
    DEV_BLE_SWITCH_SENSOR,

    DEV_BLE_DOOR_SENSOR, 
    DEV_BLE_WATER_SENSOR,
    DEV_BLE_WARM_COLD_BULB,
    DEV_BLE_LEAK_SENSOR,
    DEV_BLE_GAS_SENSOR,
    DEV_BLE_BABYS_SENSOR,
    DEV_BLE_BRIDGE,
    #endif
    DEV_GATWAY,  
    DEV_LOCK, 
    DEV_AIRCLEAN, 
    #if  CP_WISE_ENABLE
    DEV_AIR_DETECTOR,  
    DEV_PIR_SENSOR, 
    DEV_DOOR_SENSOR, 
    DEV_WATER_SENSOR,
    DEV_BUTTON,             /* 按键开关 */
    DEV_BABY_TH,		//baby Thermometer
    DEV_REPEATER,        //中继器
    #endif /* #if CP_WISE_ENABLE */
    DEVICE_TYPE_CNT,		
}TL_DeviceType;

#if  CP_WISE_ENABLE
#define DEVICE_TYPE_NUM DEVICE_TYPE_CNT
#else
#define DEVICE_TYPE_NUM (24) // 20+ DEV_THSENSOR_NEW + DEV_GATWAY + DEV_LOCK + DEV_AIRCLEAN
#endif /* #if CP_WISE_ENABLE */

typedef enum _TL_CommandByte
{
	CMD_NET_HEARTBEAT = 0,// "heart-beat"
	CMD_GET_SOFTWARE_VERSION,// "get-cc-software-version"	
	CMD_DEVICE_CONTROL,// DeviceManage	
	CMD_CONTROL_NOTIFY,
	CMD_VOICE_CONTROL,
	CMD_ALARM_NOTIFY,	
	CMD_GET_TEMPERATURE_HUMIDITY,
	CMD_GET_BRIEF_INFO,	
	CMD_DEVICE_OFFLINE,		
	CMD_GET_ALL_DEVICE_LIST,
	CMD_SCENE_CONTROL,// ScencesEngine		
	CMD_GET_CURRENT_SCENE,	
	CMD_SCENE_NOTIFY,
	CMD_SETTING_SCENE_DEVICE,
	CMD_GET_DEVICE_LIST,
	CMD_CONTROLLER_REGISTER,// netAccessManager	
	CMD_GET_CONTROLLER_INFO,
	CMD_IR_STUDY, //irstation
	CMD_IR_CONTROL, 
	CMD_ADDTO_IRSTATION,
	CMD_IR_KEYCODES, 
	CMD_SETTING_SCENE,
	CMD_SETTING_DEVICE,
	CMD_GET_SCENE_LIST,
	CMD_BACKUP_SCENE_INFO,	
	CMD_BACKUP_SCENE_DEVICE_INFO,
	CMD_ROUTER_CFG,
	CMD_FILE_BROWSING,
    #if CP_BLE_ENABLE
    CMD_ADD_BLE,//0x1c
    CMD_DEL_BLE,//add by anchungang
    CMD_SAVE_BLE_INFORMATION,
    CMD_SCAN_BLE_STATUS,
    CMD_SCAN_BLE_STATUS_END,
    CMD_RESET_BLE_BIND,
    CMD_GET_BLE_SWITCH_MAC,
    CMD_GET_BLE_SENSOR_DATA,//add by anchungang
    CMD_SEND_BLE_BRIDGE_DEVID_TO_SENSOR,//add by anchungang
    #endif
	CMD_PERMIT_JIONING,    
	CMD_GATEWAY_UPGRADE,  
	CMD_GATEWAY_RESTART,  
	CMD_SETTING_CP_USERID, 
	CMD_SETTING_DEV_PWD,  
    #if  CP_WISE_ENABLE
	CMD_ADDTO_COULD,  
    CMD_GET_LOGIN_INFO,  
    #endif /* #if CP_WISE_ENABLE */
    
	COMMAND_BYTE_CNT, 
}TL_CommandByte;


#define TL_SCENCE_ADD_DEV_ONE_BY_ONE     1  //  scene add device one by one 
#define  TL_SCENCE_MODE_DEFAULT      "Scence_default_alldev" // for save all device info
#define  TL_SCENCE_MODE_ATHOME      "Scence_at-home"
#define  TL_SCENCE_MODE_LEAVEHOME "Scence_leave-home"
typedef enum _TL_ScenceType
{
	SCENCE_TYPE_ALL = 0,
	#if TL_SCENCE_ADD_DEV_ONE_BY_ONE   // the Scence_default_alldev must save all device info,for others scence sycn irdevice -keycodes
	SCENCE_TYPE_DEFAULT,  
	#endif //  end of #if TL_SCENCE_ADD_DEV_ONE_BY_ONE
	SCENCE_TYPE_AT_HOME,  
	SCENCE_TYPE_LEAVE_HOME,
	SCENCE_TYPE_CNT,		
}TL_ScenceType;

typedef enum _TL_Action
{
	ACTION_ADD = 0,
	ACTION_UPDATE,
	ACTION_REMOVE,	
	ACTION_DELETE,
	ACTION_RENAME,
	ACTION_BING,
	ACTION_UNBING,
	ACTION_STATUS,	
	ACTION_CNT,			
}TL_Action;

typedef enum _TL_DevStatus
{
	DEVSTATUS_OFFLINE = 0,
    DEVSTATUS_ONLINE,
	DEVSTATUS_CNT,			
}TL_DevStatus;

typedef enum _TL_PROPVAL_TYPE
{
	PROPVAL_STATE = 0,
	PROPVAL_ONLINE,
	PROPVAL_VOL,
	PROPVAL_LIGHT,	
	PROPVAL_TP,
	PROPVAL_HUM,	
	PROPVAL_TYPE_CNT,	
}TL_PROPVAL_TYPE;

typedef enum stTrigger_Method
{
	TRIGGER_MANUAL = 0,
	TRIGGER_TIME,
	TRIGGER_CAMER,
	TRIGGER_LIGHT,	
	TRIGGER_TEMP,
	TRIGGER_HUM,
    TRIGGER_SMOKE,   
    #if  CP_WISE_ENABLE
	TRIGGER_DOOR_SENSOR,	
    TRIGGER_BUTTON,  
    TRIGGER_WATER_SENSOR,
    TRIGGER_PIR,
    TRIGGER_THMETER,    //宝宝体温计
    TRIGGER_PM25,
    #endif
	TRIGGER_BUTT	
}TRIGGER_METHOD_ST;


typedef enum stTrigger_Case
{
	TRIGGER_CASE_1 = 1,
	TRIGGER_CASE_2,
	TRIGGER_CASE_3,
    TRIGGER_CASE_4,
    TRIGGER_CASE_5,
    TRIGGER_CASE_6,
    TRIGGER_CASE_7,
    
	TRIGGER_CASE_BUTT	
}TRIGGER_CASE_ST;

typedef struct _TL_JsonKeyString
{
	int	keyByte;
	char	*pkeyStr;
}TL_JsonKeyString;


typedef struct _TL_TIME
{
	struct tm		time; 
	int	ms;
} TL_TIME;

typedef struct _TL_DevicesID
{     
	// cptype-devtype-sqliteindex
	TL_ControlPointType	cp;
	TL_DeviceType	devtype;
	int	iDeviceId;
}TL_DevicesID;

typedef struct _TL_CLR_RGB
{     
	float uiClr_R;
	float uiClr_G;
	float uiClr_B;
}TL_CLR_RGB;
typedef struct _TL_Sensor
{
	int pir_sensor_value;
	int magnet_sensor_value;
	int hourglass_sensor_value;
	int pm25_sensor_value;
	int pm10_sensor_value;
	int temp_sensor_value;
	int switch_sensor_value;
}_TL_Sensor_t;

typedef struct _TL_PropVal_Val
{
    int propType;   
    int status;
    int online;
    int volume;
    int light;		
    unsigned int light_hsv;		
    int temperature;
    int humidity;
    bool smoke;
    TL_CLR_RGB color;
    int battery;/* 传感器电量 */
    int mute; //0:mute, 1: unmute   
    int value;  //设备的一些其他信息
    _TL_Sensor_t bleSenser;
}TL_PropVal_Val; 

typedef struct _TL_PropVal
{     
  	//TL_PROPVAL_TYPE  type;
  	TL_DeviceType       type;
  	TL_PropVal_Val	 val;
}TL_PropVal;


#define  TL_PROP_TYPE_STATUE   0x00000001
#define  TL_PROP_TYPE_LIGHT      0x00000002
#define  TL_PROP_TYPE_HSV          0x00000004
#define  TL_PROP_TYPE_RGB         0x00000008
#define  TL_PROP_TYPE_HUM        0x00000010
#define  TL_PROP_TYPE_TP           0x00000020
#define  TL_PROP_TYPE_VOLUME   0x00000040	
#define  TL_PROP_TYPE_SMORK     0x00000080 
#define  TL_PROP_TYPE_PWD       0x00000100
#define  TL_PROP_TYPE_TONE      0x00000200

#if  CP_WISE_ENABLE
typedef enum enCmd_Statue
{
	CMD_STATUE_UNUSE = 0,
    CMD_STATUE_SUCCESS = 1,
    CMD_STATUE_FAILED = 2,
    CMD_STATUE_OPING = 3,   /* 正在处理 */
    CMD_STATUE_FEEDBACK = 4,   /* 正在处理 */
    CMD_STATUE_BUTT
}CMD_STATUE_EN;
#endif 

typedef struct _TL_Properties
{
    //  for device 
    TL_ControlPointType	cptype;
    TL_DeviceType	devtype;		
    char	devname[DEVICE_NAME_LENGTH];
    char	nickname[DEVICE_NAME_LENGTH];    
    char	devId[DEV_ID_LENGTH];
    char	devownerId[DEV_ID_LENGTH]; 
    char    brandname[BRAND_ID_LENGTH];
    char    devtypeno[DEV_TYPE_LENGTH];
    char	mac[MAC_ADDR_LENGTH];
    char	addr[IP_ADDR_LENGTH];
    char	addr6[IPV6_ADDR_LENGTH];
    int     net_id;
    TL_CommandByte     response_cmd;
	int		ble_association_status;
	char    DeviceUUID[BT_DEVICE_UUID_LENGTH];
    /************
    char *pstr_sin_addr;   
    inet_pton(AF_INET,"127.0.0.1",&address.sin_addr); // @@"char  to  sin_add"
    address.sin_addr.s_addr= inet_addr("127.0.0.1");   // @@"char  to  sin_add"
    pstr_sin_addr = inet_ntoa(address.sin_addr); //"127.0.0.1"     // @@@"sin_add  to  char"

    // 	struct in_addr ipv4addr = {INADDR_ANY};
    // 	struct in6_addr ipv6addr = IN6ADDR_ANY_INIT; 
    // 	char ipv4addrbuf[INET_ADDRSTRLEN];
    // 	char ipv6addrbuf[INET6_ADDRSTRLEN];
    //  inet_ntop(AF_INET, &ipv4addr, ipv4addrbuf, sizeof(ipv4addrbuf));// @@ "sin_add  to  char"
    //  inet_ntop(AF_INET6, &ipv6addr, ipv6addrbuf, sizeof(ipv6addrbuf));// @@ "sin_add  to  char"
    *************/   
    int propType;    
    int status;
    int online;
    int volume;
    int light;		
    uint32_t light_hsv;		
    int temperature; 
    int humidity;
    bool smoke;
    TL_CLR_RGB color;
    int battery;/* 传感器电量 */
    int mute; //0:mute, 1: unmute      

    //  for Scence
    int  value;
    int   scence;	
    char scenceName[DEVICE_NAME_LENGTH];
    int scenceType; 
    char acTimespan[30];
    int  uiRepeatmode;       //每一bit表示一个星期的一天 
    int  mTriggerVal;    
    int itime;
	_TL_Sensor_t bleSensor;
}TL_Properties;


typedef struct _TL_Properties2
{
    //  for device 
    TL_ControlPointType	cptype;
    TL_DeviceType	devtype;		
    char	devname[DEVICE_NAME_LENGTH];
    char	nickname[DEVICE_NAME_LENGTH];     
    char	devId[DEV_ID_LENGTH];
    char	devownerId[DEV_ID_LENGTH]; 
    char    brandname[BRAND_ID_LENGTH];
    char    devtypeno[DEV_TYPE_LENGTH];
    char	mac[MAC_ADDR_LENGTH];
    char	addr[IP_ADDR_LENGTH];
    char	addr6[IPV6_ADDR_LENGTH]; 
    /************
    char *pstr_sin_addr;   
    inet_pton(AF_INET,"127.0.0.1",&address.sin_addr); // @@"char  to  sin_add"
    address.sin_addr.s_addr= inet_addr("127.0.0.1");   // @@"char  to  sin_add"
    pstr_sin_addr = inet_ntoa(address.sin_addr); //"127.0.0.1"     // @@@"sin_add  to  char"

    // 	struct in_addr ipv4addr = {INADDR_ANY};
    // 	struct in6_addr ipv6addr = IN6ADDR_ANY_INIT; 
    // 	char ipv4addrbuf[INET_ADDRSTRLEN];
    // 	char ipv6addrbuf[INET6_ADDRSTRLEN];
    //  inet_ntop(AF_INET, &ipv4addr, ipv4addrbuf, sizeof(ipv4addrbuf));// @@ "sin_add  to  char"
    //  inet_ntop(AF_INET6, &ipv6addr, ipv6addrbuf, sizeof(ipv6addrbuf));// @@ "sin_add  to  char"
    *************/ 
    int propType;   
    int status;
    int online;
    int volume;
    int light;
    unsigned int light_hsv;
    int temperature;
    int humidity;
    bool smoke;
    TL_CLR_RGB color;
    int battery;/* 传感器电量 */
    int mute; //0:mute, 1: unmute    

    //  for Scence    
    char    rckey[RC_IRKEY_LENGTH];     
    char    keycodes[KEYCODES_BUFF_MAX]; 
    int  value;
    int   scence;
    char scenceName[DEVICE_NAME_LENGTH];
    int scenceType; 
    char acTimespan[30];
    int  uiRepeatmode;       //每一bit表示一个星期的一天 
    int  mTriggerVal;
}TL_Properties2;


typedef struct _TL_CmdDataS
{
	TL_CommandByte	cmd;
	int   socket;
	char remoteClient[256]; 
	char masterCtrlID[MASTER_CONTROLER_ID_LEN];  
	int	type;
	TL_Action   action;
	int   scence;	
	int	result;
	char ccId[128];  
} TL_CmdDataS;

typedef struct _TL_UintPropDataS
{
    TL_CommandByte	cmd;
    int   socket;
    char remoteClient[256];
    char masterCtrlID[MASTER_CONTROLER_ID_LEN]; 
    int	type;
    TL_Action   action;
    int   scence;	
    int	result;
    char ccId[128];  
    TL_Properties  prop;	
} TL_UintPropDataS;

typedef struct _TL_UintPropDataS2
{
    TL_CommandByte	cmd;
    int   socket;
    char remoteClient[256];
    char masterCtrlID[MASTER_CONTROLER_ID_LEN]; 
    int	type;
    TL_Action   action;
    int   scence;	
    int	result;
    char ccId[128];  
    TL_Properties2  prop2;	
} TL_UintPropDataS2;

typedef struct _TL_GroupPropDataS
{
    TL_CommandByte	cmd;
    int   socket;
    char remoteClient[256];
    char masterCtrlID[MASTER_CONTROLER_ID_LEN]; 
    int	type;
    TL_Action   action;
    int   scence;	
    int	result;
    char ccId[128];  
    TL_Properties  prop[DEV_CNT_MAX];
    int   propCnt;	
} TL_GroupPropDataS;


typedef union _TL_CommuPacketDatas 
{  
	TL_CmdDataS            CmdDataS;
	TL_UintPropDataS      UintPropDataS;
	TL_UintPropDataS2      UintPropDataS2;
	TL_GroupPropDataS    GroupPropDataS;
}TL_CommuPacketDatas; 


typedef struct _TL_CommuPacketDatas_node
{
	TL_CommuPacketDatas	data;
	unsigned short	index;
	struct list	list;
} TL_CommuPacketDatas_node;


#if _TL_DEBUG_SOCKET
// for socket struct
typedef struct _TL_TEST_Data1
{
	int	d1;
	char		str[10]; 
	float		fd;	
	char		ch;
	int	d2;	
} TL_TEST_Data1;

typedef struct _TL_TEST_Data
{
	int	data;
	TL_TEST_Data1		data1;
	int	data2;	   
	char	strstr[10]; 	   
	float		fdata;  
	char	chch;	
} TL_TEST_Data;

// TL_TEST_Data  data={0x20000000,{1,"data1",0.100,'a',0x10000000},2,"data2",0.200,'b'};
//%d, { %d,  %s,  %f,   %c,   %d  } ; %d ,  %s  , %f  ,%c;
//data.data,data.data1.d1,data.data1.str,data.data1.fd,data.data1.ch,data.data1.d2,data.data2,data.strstr,data.fdata,data.chch
#endif// _TL_DEBUG_SOCKET

typedef struct Alarm_Data
{
	bool smork_alarm;
	bool temperature_alarm;
	bool humidity_alarm;
}ALARM_DATA;

typedef struct Alarm_Node
{
    char acDevIPv6[IPV6_ADDR_LENGTH];
    TL_DeviceType devtype;
    ALARM_DATA data;
    struct list list;
}ALARM_NODE;

/* 所有设备的cmd都在此定义，后续如果有新的设备或者新的操作方式可以直接在其中添加的,该类型直接存储在DB中的scene_device_table中 */
typedef enum Device_CmdType
{
    /* BL设备的cmd类型 */
	BL_CMD_ST= 0,
	BL_CMD_CONTR_OFF= BL_CMD_ST,
	BL_CMD_CONTR_ON,
	BL_CMD_REFRESH ,
	BL_CMD_BUTT,

    /* zigbee设备的cmd类型 */
    ZB_CMD_ST = BL_CMD_BUTT,
    ZB_CMD_CONTR_OFF = ZB_CMD_ST,
    ZB_CMD_CONTR_ON,
    ZB_CMD_CONTR_LEVEL,//PCT
    ZB_CMD_CONTR_RGB,//PCT
    ZB_CMD_STATUS_GET,
    ZB_CMD_LUM_SET,
    ZB_CMD_BUTT,

    /* 新绿智科设备的cmd类型 */
    NEWG_CMD_ST = ZB_CMD_BUTT,
    NEWG_CMD_CONTR_OFF = NEWG_CMD_ST,
    NEWG_CMD_CONTR_ON,
    NEWG_CMD_BUTT,

    /* BLE设备的cmd类型 */
    BLE_CMD_ST = NEWG_CMD_BUTT,
    BLE_CMD_CONTR_OFF = BLE_CMD_ST,
    BLE_CMD_CONTR_ON,
    BLE_CMD_CONTR_GET_MAC,
    BLE_CMD_CONTR_RESET_BIND,
    BLE_CMD_SENSOR_STATUS_GET,//begin add by anchungang for ble sensor
    BLE_CMD_SENSOR_SEND_TAG_DEVID,//begin add by anchungang for ble sensor
    BLE_CMD_BUTT,

    /* DMR相关设备的命令 */
    DMR_CMD_ST = BLE_CMD_BUTT,
    DMR_CMD_ALARM_OFF = DMR_CMD_ST,
    DMR_CMD_ALARM_ON,
    DMR_CMD_BUTT
}DEVICE_CMD_TYPE;



typedef struct _NetIpPort
{
    int    port;
    char  ip[IP_ADDR_LENGTH];
} NetIpPort;

typedef struct _DataBufNode
{
    int      port;
    char    ip[IP_ADDR_LENGTH];
    char    *pbuf;
    int       buflen;
    struct list list;
} DataBufNode;


typedef struct _DataBufNode2
{
    int mType;
    int mIdex;
    int mStatu;
    char cDevMac[MAC_ADDR_LENGTH];
    char cDevName[DEVICE_NAME_LENGTH];
    char cOwerDevMac[MAC_ADDR_LENGTH];
    struct list list;
} DataBufNode2;


typedef struct stTime
{
	int hour_begin; 
	int minute_begin;
	int second_begin;
	int hour_end; 
	int minute_end;
	int second_end;
}TIME_ST;

typedef struct stTime_Minute
{
	int begin_minute; 
    int end_minute;
}TIME_MINUTE_ST;

#pragma pack()

// ################################


#endif  

