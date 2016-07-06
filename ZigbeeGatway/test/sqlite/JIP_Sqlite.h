/****************************************************************************
 *
 * MODULE:             Linux Zigbee - IFTT
 *
 * COMPONENT:          Interface to JCI
 *
 * REVISION:           $Revision: 1.0 $
 *
 * DATED:              $Date: 2014-10-22 11:16:28 +0000 (Fri, 02 Dec 2011) $
 *
 * AUTHOR:             PCT
 *
 ***************************************************************************

 * Copyright Tonly B.V. 2014. All rights reserved
 *
 ***************************************************************************/


#ifndef  JIPSQLITE_H_INCLUDED
#define  JIPSQLITE_H_INCLUDED

#include <stdint.h>
#include <netinet/in.h>
#include "list.h"//PCT

//#define DBG_vPrintf(a,b,ARGS...) do {  if (a) printf(b, ## ARGS); } while(0)

/****************************************************************************/
/***        Include files                                                 ***/
/****************************************************************************/
#include "sqlite3.h"
#define VER_DB    "0.3"


/****************************************************************************/
/***        Macro Definitions                                             ***/
/****************************************************************************/
//DeviceTable Mask Code
#define DEVICEID 0x01
#define DEVICEUSERNAME 0x02
#define DEVICEADDRESS 0x04
#define DEVICEMACADDRESS 0x08
#define DEVICEISONLINE 0x10
#define DEVICEONOFF 0x20
#define DEVICEVALUE1 0x40
#define DEVICEVALUE2 0x80
#define DEVICETYPE 0x100
#define DEVICEAPPEND 0x03
//SceneTable Mask Code
#define SCENETIMESPAN 0x01
#define SCENEONOFF 0x02
#define SCENEREPEATMODE 0x04
//Profile Table Mask Code
#define PROFILENAME 0x01
#define PROFILESTARTTIME   0x02
#define PROFILEENDTIME 0x04
#define PROFILEREPEAT   0x08
#define PROFILEDEVICES  0x10
#define PROFILEONOFF    0x20
//Room Table Mask
#define ROOMUSERNAME 0x01
//Profile List Table Mask Code
#define PROFILELISTSCENEID 0x01
//Room List Table Mask Code
#define ROOMLISTSCENEID 0x01


/****************************************************************************/
/***        Type Definitions                                              ***/
/****************************************************************************/
typedef enum
{
    SQLITE_SUCCESS = 0,
    SQLITE_ERR,
    SQLITE_FIND_ERR,
    SQLITE_MALLOC_ERR,
    SQLITE_DBERROR,
    SQLITE_NOT_FOUND,
    
    SQLITE_DEVICETABLE_DEVICEID_ERROR,
    SQLITE_DEVICETABLE_SCENEID_ERROR,
    SQLITE_DEVICETABLE_USERNAME_ERROR,
    SQLITE_DEVICETABLE_ADDRESS_ERROR,
    SQLITE_DEVICETABLE_MACADDRESS_ERROR,
    SQLITE_DEVICETABLE_DEVICETYPE_ERROR,

    SQLITE_SCENETABLE_TIMESPAN_ERROR,
    SQLITE_SCENETABLE_ONOFF_ERROR,
    SQLITE_SCENETABLE_REPEATMODE_ERROR,

    SQLITE_PROFILE_NAME_ERROR,
    SQLITE_PROFILE_ONOFF_ERROR,
    SQLITE_PROFILE_ADDSTR_ERROR,
    SQLITE_PROFILE_ID_ERROR,

    SQLITE_ROOMTABLE_NAME_ERROR,
    SQLITE_ROOMLISTTABLE_NAME_ERROR,
    SQLITE_PROFILELISTTABLE_NAME_ERROR,
} t_SqliteStatus;

typedef struct _Device_Table{
    uint16_t Index;
    uint32_t DeviceId;
    //uint16_t SceneId;
    char DeviceName[1024];
    char UserName[1024];
    uint16_t Address;
    char DeviceIpv6[1024];
    uint64_t MacAddress;
    uint8_t DeviceType;
    uint8_t DeviceOnOff;
    uint16_t DeviceValue1;
    uint16_t DeviceValue2;
    uint8_t DeviceIsOnline;
    char DeviceAppendstr[2048];//Append info
    struct dl_list list;
}Device_Table_t;

typedef struct _Profile_Table{//APK端的场景
    uint16_t Index;
    uint16_t ProfileId;
    char ProfileName[1024];
    char ProfileOnOff[5];
    char ProfileStartTime[10];
    char ProfileEndTime[10];
    char ProfileTimeRepeat[1024];
    char * ProfileTableDevices;
    struct dl_list list;
}Profile_Table_t;

typedef struct _Linkage_Table{//Device Linkage Table
    uint16_t Index;
    uint16_t LinkageId;
    char LinkageSensorAddress[255];
    char *LinkageDevicesList;
}Linkage_Table_t;

typedef struct _Scene_Table{
    uint16_t Index;
    uint16_t SceneId;//Zigbee场景的id
    char TimeSpan[30];//时间跨度
    uint8_t OnOff;//是否启动此Scene
    uint8_t RepeatMode;//按照比特位从周一到周七
    struct dl_list list;
}Scene_Table_t;

typedef struct _Scene_Device_Table{
    uint16_t Index;
    Scene_Table_t SceneData;
    Device_Table_t DeviceData;
}Scene_Device_Table_t;

typedef struct _Room_Table{
    uint16_t Index;
    char RoomName[1024];
    uint32_t GroupId;
    struct dl_list list;
}Room_Table_t;

typedef struct _Profile_List_Table{
    uint16_t Index;
    Scene_Table_t SceneId;
    Profile_Table_t ProfileId;
    struct dl_list list;
}Profile_List_Table_t;

typedef struct _Room_List_Table{
    uint16_t Index;
    Device_Table_t DeviceId;
    Room_Table_t RoomId;
    struct dl_list list;
}Room_List_Table_t;

/****************************************************************************/
/***        Local Function Prototypes                                     ***/
/****************************************************************************/

/****************************************************************************/
/***        Exported Variables                                            ***/
/****************************************************************************/

//Sqlite Operate
t_SqliteStatus OpenSqliteDB(char *db);
t_SqliteStatus CloseSqliteDB();
//Device Table Operate
t_SqliteStatus DeviceTableAddRecord( Device_Table_t *DeviceData);
t_SqliteStatus DeviceTableDelete( Device_Table_t *DeviceData );
t_SqliteStatus DeviceTableUpdate( Device_Table_t *DeviceData, uint8_t maskcode );
t_SqliteStatus DeviceTableClear();
//Scene Table Operate
t_SqliteStatus SceneTableAddRecord( Scene_Table_t *SceneData );
t_SqliteStatus SceneTableDelete( Scene_Table_t *SceneData ) ;
t_SqliteStatus SceneTableUpdate( Scene_Table_t *SceneData, uint8_t maskcode );
t_SqliteStatus SceneTableClear();
//Scene Device Table Operate
t_SqliteStatus SceneDeviceTableAddRecord( Scene_Device_Table_t *SceneDeviceData );
t_SqliteStatus SceneDeviceTableDelete( Scene_Device_Table_t *SceneDeviceData );
t_SqliteStatus SceneDeviceTableClear();
//Profile Table Operate
t_SqliteStatus ProfileTableAddRecord( Profile_Table_t *ProfileData );
t_SqliteStatus ProfileTableDelete( Profile_Table_t *ProfileData );
t_SqliteStatus ProfileTableUpdate( Profile_Table_t *ProfileData, uint8_t maskcode );
t_SqliteStatus ProfileTableClear();
//Linkage Table Operate
void LinkageTableSearchOneFree( Linkage_Table_t *LinkageData );
t_SqliteStatus LinkageTableSearchOne( Linkage_Table_t *LinkageData );
t_SqliteStatus LinkageTableClear();
t_SqliteStatus LinkageTableDelete( Linkage_Table_t *LinkageData );
t_SqliteStatus LinkageTableAddRecord( Linkage_Table_t *LinkageData );

//Room Talbe Operate
t_SqliteStatus RoomTableAddRecord( Room_Table_t *RoomData );
t_SqliteStatus RoomTableDelete( Room_Table_t *RoomData );
t_SqliteStatus RoomTableUpdate( Room_Table_t *RoomData, uint8_t maskcode );
t_SqliteStatus RoomTableClear();
//Profile List Table Operate
t_SqliteStatus ProfileListTableAddRecord( Profile_List_Table_t *ProfileListData );
t_SqliteStatus ProfileListTableDelete( Profile_List_Table_t *ProfileListData );
t_SqliteStatus ProfileListTableUpdate( Profile_List_Table_t *ProfileListData, uint8_t maskcode );
t_SqliteStatus ProfileListTableClear();
//Room List Table Operate
t_SqliteStatus RoomListTableAddRecord( Room_List_Table_t *RoomListData );
t_SqliteStatus RoomListTableDelete( Room_List_Table_t *RoomListData );
t_SqliteStatus RoomListTableDeleteUnderDevice( Room_List_Table_t *RoomListData );
t_SqliteStatus RoomListTableUpdate( Room_List_Table_t *RoomListData, uint8_t maskcode );
t_SqliteStatus RoomListTableClear();
//Search Operate
t_SqliteStatus RoomTableSearchOne( Room_Table_t *RoomData );
t_SqliteStatus SceneTableSearchOne( Scene_Table_t *SceneData );
t_SqliteStatus DeviceTableSearchOne( Device_Table_t *DeviceData );
t_SqliteStatus ProfileTableSearchOne( Profile_Table_t *ProfileData );

t_SqliteStatus SceneDeviceListSearch( uint16_t SceneId, Device_Table_t *DeviceData );
t_SqliteStatus SceneTableSearch( Scene_Table_t *SceneData);
t_SqliteStatus RoomTableSearch( Room_Table_t *RoomData);
t_SqliteStatus DeviceTableSearch( Device_Table_t *DeviceData);
t_SqliteStatus ProfileDeviceListSearch( uint16_t ProfileId, Device_Table_t *DeviceData );
t_SqliteStatus ProfileSceneListSearch( uint16_t ProfileId, Scene_Table_t *SceneData );
t_SqliteStatus RoomDeviceListSearch( uint32_t RoomId, Device_Table_t *DeviceData );
//Search Unused Id
uint16_t DBUnUsedProfileId();
uint32_t DBUnUsedRoomId();
uint16_t DBUnUsedSceneId();
//Free Memory
void ProfileTableSearchOneFree( Profile_Table_t *ProfileData );
void RoomTableSearchFree( Room_Table_t *RoomData);
void DeviceTableSearchFree( Device_Table_t *DeviceData);
void SceneTableSearchFree( Scene_Table_t *SceneData);
void ProfileTableSearchFree( Profile_Table_t *ProfileData);
//Check a Record is Exist
t_SqliteStatus ProfileTableDataIsExist( Profile_Table_t *ProfileData );
t_SqliteStatus RoomTableDataIsExist( Room_Table_t *RoomData );
t_SqliteStatus SceneTableDataIsExist( Scene_Table_t *SceneData );
t_SqliteStatus DeviceTableDataIsExist( Device_Table_t *DeviceData );
//On Off Profile Record and Scene Record
t_SqliteStatus  ProfileTableOnOff( Profile_Table_t *ProfileData, uint8_t onoff);

#endif  /* JIPSQLITE_H_INCLUDED */

/****************************************************************************/
/***        END OF FILE                                                   ***/
/****************************************************************************/

