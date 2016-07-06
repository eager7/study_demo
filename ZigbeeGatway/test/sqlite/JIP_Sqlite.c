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

//#include "JIP.h"
#include "JIP_Sqlite.h"

#define PCT_vPrintf(a,b,ARGS...) do{                                    \
    if(a)                                                               \
    {                                                                   \
        FILE *fp=fopen("/tmp/sqliteout.txt","a+");                      \
        fprintf(fp,"\n------%s-------,%d\n\n",__FUNCTION__,__LINE__);   \   
        fprintf(fp,b,## ARGS);                                          \
        fclose(fp);                                                     \
    }                                                                   \
}while(0)

#define DBG_SQLITE 0
sqlite3 *SQLITE_DB;

uint16_t DBUnUsedProfileId();
uint32_t DBUnUsedRoomId();
uint16_t DBUnUsedSceneId();

/*****************************************************************************
 �� �� ��  : OpenSqliteDB
 ��������  : �����ݿ�
 �������  : ��
 �������  : ��
 �� �� ֵ  : ��ȷ����0�����󷵻�1
 �޸���ʷ      :
  1.��    ��   : 2014��10��29��
    ��    ��   : pct
    �޸�����   : �����ɺ���

*****************************************************************************/
t_SqliteStatus OpenSqliteDB(char *db)
{   
    PCT_vPrintf(DBG_SQLITE, "Open %s \n",db);
    if( SQLITE_OK != sqlite3_open( db, &SQLITE_DB) )
    {
        PCT_vPrintf(DBG_SQLITE, "Failed to Open SQLITE_DB \n");
        return SQLITE_DBERROR;
    }

    return SQLITE_SUCCESS;
}

t_SqliteStatus CloseSqliteDB()
{
    PCT_vPrintf(DBG_SQLITE, "===>CloseSqliteDB<===\n");
    
    sqlite3_close( SQLITE_DB );
    return SQLITE_SUCCESS;
}

///////////////////////////////////////Device Table Function///////////////////////////////////////////////
/*******************************************************************************
** �� �� ��  : DeviceTableAddRecord
** ��������  : ���豸�б������һ���µļ�¼
** �������  : Device_Table_t *DeviceData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus DeviceTableAddRecord( Device_Table_t *DeviceData)
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*������*/
    PCT_vPrintf(DBG_SQLITE, "%d\n",DeviceData->DeviceIsOnline);
    sprintf(SqlCommand,
        "Insert Into Device_Table("
        "deviceid,devicename,username,"
        "address,deviceipv6,macaddress,"
        "devicetype,deviceonoff,devicevalue1,"
        "devicevalue2,deviceisonline,deviceappend) "
        "values(%lu,'%s','%s',"
        "%d,'%s',%llu,"
        "%d,%d,%d,"
        "%d,%d,'%s')", 
        DeviceData->DeviceId,DeviceData->DeviceName,DeviceData->UserName,
        DeviceData->Address,DeviceData->DeviceIpv6,DeviceData->MacAddress,
        DeviceData->DeviceType,DeviceData->DeviceOnOff,DeviceData->DeviceValue1,
        DeviceData->DeviceValue2,DeviceData->DeviceIsOnline,DeviceData->DeviceAppendstr);

    PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    DeviceData->Index = sqlite3_last_insert_rowid(SQLITE_DB);
    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : DeviceTableDelete
** ��������  : ���豸�б���ɾ��һ���豸
** �������  : Device_Table_t *DeviceData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus DeviceTableDelete( Device_Table_t *DeviceData )
{
    char SqlCommand[1024] = {0};
 
    char *pErrMsg = NULL;/*������*/
    sprintf(SqlCommand,"Delete From Device_Table where deviceipv6='%s'", DeviceData->DeviceIpv6);

    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    
    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : DeviceTableUpdate
** ��������  : �����豸�б����ݲ���maskcode���и��£�maskcodeͨ����Ļ���
               ��õ������봫���豸��Ĭ��������Ϊ����
** �������  : Device_Table_t *DeviceData  
             : uint8_t maskcode            
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus DeviceTableUpdate( Device_Table_t *DeviceData, uint8_t maskcode )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*������*/

    if( maskcode & DEVICEID )
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand,"Update Device_Table Set deviceid=%lu Where deviceipv6='%s'", 
            DeviceData->DeviceId, DeviceData->DeviceIpv6);
        PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
            return SQLITE_DEVICETABLE_DEVICEID_ERROR;
        }
    }

    if( maskcode & DEVICEUSERNAME )
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand,"Update Device_Table Set username='%s' Where deviceipv6='%s'", 
            DeviceData->UserName, DeviceData->DeviceIpv6);
        PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
            return SQLITE_DEVICETABLE_USERNAME_ERROR;
        }
    }
    
    if( maskcode & DEVICEADDRESS )
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand,"Update Device_Table Set address=%d Where deviceipv6='%s'", 
            DeviceData->Address, DeviceData->DeviceIpv6);
        PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
            return SQLITE_DEVICETABLE_ADDRESS_ERROR;
        }
    }

    if( maskcode & DEVICEMACADDRESS )
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand,"Update Device_Table Set macaddress=%llu Where deviceipv6='%s'", 
            DeviceData->MacAddress, DeviceData->DeviceIpv6);
        PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
            return SQLITE_DEVICETABLE_MACADDRESS_ERROR;
        }
    }

    if( maskcode & DEVICETYPE )
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand,"Update Device_Table Set devicetype=%d Where deviceipv6='%s'", 
            DeviceData->DeviceType, DeviceData->DeviceIpv6);
        PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
            return SQLITE_DEVICETABLE_DEVICETYPE_ERROR;
        }
    }
    
    if( maskcode & DEVICEONOFF )
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand,"Update Device_Table Set deviceonoff=%d Where deviceipv6='%s'", 
            DeviceData->DeviceOnOff, DeviceData->DeviceIpv6);
        PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
            return SQLITE_DEVICETABLE_DEVICETYPE_ERROR;
        }
    }
    
    if( maskcode & DEVICEVALUE1 )
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand,"Update Device_Table Set devicevalue1=%d Where deviceipv6='%s'", 
            DeviceData->DeviceValue1, DeviceData->DeviceIpv6);
        PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
            return SQLITE_DEVICETABLE_DEVICETYPE_ERROR;
        }
    }
    if( maskcode & DEVICEVALUE2 )
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand,"Update Device_Table Set devicevalue2=%d Where deviceipv6='%s'", 
            DeviceData->DeviceValue2, DeviceData->DeviceIpv6);
        PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
            return SQLITE_DEVICETABLE_DEVICETYPE_ERROR;
        }
    }

    if( maskcode & DEVICEISONLINE )
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand,"Update Device_Table Set deviceisonline=%d Where deviceipv6='%s'", 
            DeviceData->DeviceIsOnline, DeviceData->DeviceIpv6);
        PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
            return SQLITE_DEVICETABLE_DEVICETYPE_ERROR;
        }
    }
    
    if( maskcode == DEVICEAPPEND )
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand,"Update Device_Table Set deviceappend='%s' Where deviceipv6='%s'", 
            DeviceData->DeviceAppendstr, DeviceData->DeviceIpv6);
        PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
            return SQLITE_DEVICETABLE_DEVICETYPE_ERROR;
        }
    }
    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : DeviceTableClear
** ��������  : ����豸�б�
** �������  : ��
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus DeviceTableClear()
{
    char *pErrMsg = NULL;/*������*/

    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, "Delete From Device_Table", NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}

///////////////////////////////////////Scene Table Function////////////////////////////////////
/*******************************************************************************
** �� �� ��  : SceneTableAddRecord
** ��������  : �ڳ������б������Ӽ�¼
** �������  : Scene_Table_t *SceneData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus SceneTableAddRecord( Scene_Table_t *SceneData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*������*/
    sprintf(SqlCommand,
        "Insert Into Scene_Table(sceneid,timespan,onoff,repeatmode) values(%d,'%s',%d,%d)",
        SceneData->SceneId,SceneData->TimeSpan, SceneData->OnOff, SceneData->RepeatMode);
    PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    SceneData->Index = sqlite3_last_insert_rowid(SQLITE_DB);
    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : SceneTableDelete
** ��������  : �ӳ����б���ɾ��һ����¼
** �������  : Scene_Table_t *SceneData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus SceneTableDelete( Scene_Table_t *SceneData ) 
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*������*/
    sprintf(SqlCommand,
        "Delete From Scene_Table where sceneid=%d",SceneData->SceneId);
    PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : SceneTableUpdate
** ��������  : ���³����б�����maskcode���и��£����봫��Scene id��Ϊ����
** �������  : Scene_Table_t *SceneData  
             : uint8_t maskcode          
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pc
*******************************************************************************/
t_SqliteStatus SceneTableUpdate( Scene_Table_t *SceneData, uint8_t maskcode )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*������*/

    if(maskcode&SCENETIMESPAN)
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand,"Update Scene_Table Set timespan='%s' Where sceneid=%d", 
            SceneData->TimeSpan, SceneData->SceneId);
        PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
            return SQLITE_SCENETABLE_TIMESPAN_ERROR;
        }       
    }
    
    if(maskcode&SCENEONOFF)
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand,"Update Scene_Table Set onoff=%d Where sceneid=%d", 
            SceneData->OnOff, SceneData->SceneId);
        PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
            return SQLITE_SCENETABLE_TIMESPAN_ERROR;
        }       
    }
    
    if(maskcode&SCENEREPEATMODE)
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand,"Update Scene_Table Set repeatmode=%d Where sceneid=%d", 
            SceneData->RepeatMode, SceneData->SceneId);
        PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
            return SQLITE_SCENETABLE_TIMESPAN_ERROR;
        }       
    }
    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : SceneTableClear
** ��������  : ��ճ����б�
** �������  : ��
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus SceneTableClear()
{
    char *pErrMsg = NULL;/*������*/

    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, "Delete From Scene_Table", NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}
///////////////////////////////////////Scene Device Table Function//////////////////////////////////
/*******************************************************************************
** �� �� ��  : SceneDeviceTableAddRecord
** ��������  : ��Scene���в����µļ�¼�����᷵������ֵ���ṹ����
** �������  : Scene_Device_Table_t *SceneDeviceData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus SceneDeviceTableAddRecord( Scene_Device_Table_t *SceneDeviceData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*������*/
    sprintf(SqlCommand,
        "Insert Into Scene_Device_Table(sceneindex,deviceindex) values(%d,%d)", 
        SceneDeviceData->SceneData.Index, SceneDeviceData->DeviceData.Index);
    PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    SceneDeviceData->Index = sqlite3_last_insert_rowid(SQLITE_DB);
    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : SceneDeviceTableDelete
** ��������  : ��Scene����ɾ��һ����¼������Scene��Id����ɾ��
** �������  : Scene_Device_Table_t *SceneDeviceData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus SceneDeviceTableDelete( Scene_Device_Table_t *SceneDeviceData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*������*/
    sprintf(SqlCommand,
        "Delete From Scene_Device_Table Where sceneindex=%d", SceneDeviceData->SceneData.Index);
    PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : SceneDeviceTableClear
** ��������  : ���Scene��
** �������  : ��
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus SceneDeviceTableClear()
{
    char *pErrMsg = NULL;/*������*/

    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, "Delete From Scene_Device_Table", NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}


///////////////////////////////////////Profile Table Function//////////////////////////////////
/*******************************************************************************
** �� �� ��  : ProfileTableAddRecord
** ��������  : ��Profile�б�������һ��Record������������ֵ���ṹ��
** �������  : Profile_Table_t *ProfileData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pc
*******************************************************************************/
t_SqliteStatus ProfileTableAddRecord( Profile_Table_t *ProfileData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*������*/
    //PCT_vPrintf(DBG_SQLITE,"%d\n",ProfileData);
    sprintf(SqlCommand,
        "Insert Into Profile_Table(profileid,profilename,profileonoff,profilestarttime,profileendtime,profiletimerepeat,profiledevices)" 
        "values(%d,'%s','%s','%s','%s','%s','%s')", 
        ProfileData->ProfileId, ProfileData->ProfileName, ProfileData->ProfileOnOff, ProfileData->ProfileStartTime,
        ProfileData->ProfileEndTime,ProfileData->ProfileTimeRepeat,ProfileData->ProfileTableDevices);
    PCT_vPrintf(DBG_SQLITE,"SqlCommand = %s\n",SqlCommand);
    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    ProfileData->Index = sqlite3_last_insert_rowid(SQLITE_DB);
    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : ProfileTableDelete
** ��������  : ��Profile�б���ɾ��һ����¼
** �������  : Profile_Table_t *ProfileData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus ProfileTableDelete( Profile_Table_t *ProfileData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*������*/
    sprintf(SqlCommand,
        "Delete From Profile_Table Where profileid=%d", ProfileData->ProfileId);
    PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : ProfileTableUpdate
** ��������  : ����Profile�б����ݲ���maskcode���£����봫��Profile��id��
               Ϊ����
** �������  : Profile_Table_t *ProfileData  
             : uint8_t maskcode              
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus ProfileTableUpdate( Profile_Table_t *ProfileData, uint8_t maskcode )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*������*/
    if(maskcode&PROFILENAME)
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand,"Update Profile_Table Set profilename='%s' Where profileid=%d", 
            ProfileData->ProfileName, ProfileData->ProfileId);
        PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
            return SQLITE_PROFILE_NAME_ERROR;
        }       
    }
    if(maskcode&PROFILEONOFF)
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand,"Update Profile_Table Set profileonoff='%s' Where profileid=%d", 
            ProfileData->ProfileOnOff, ProfileData->ProfileId);
        PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
            return SQLITE_PROFILE_ONOFF_ERROR;
        }       
    }

    if(maskcode&PROFILESTARTTIME)
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand,"Update Profile_Table Set profilestarttime='%s' Where profileid=%d", 
            ProfileData->ProfileStartTime, ProfileData->ProfileId);
        PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
            return SQLITE_PROFILE_ADDSTR_ERROR;
        }       
    }
    if(maskcode&PROFILEENDTIME)
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand,"Update Profile_Table Set profileendtime='%s' Where profileid=%d", 
            ProfileData->ProfileEndTime, ProfileData->ProfileId);
        PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
            return SQLITE_PROFILE_ADDSTR_ERROR;
        }       
    }
    if(maskcode&PROFILEREPEAT)
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand,"Update Profile_Table Set profiletimerepeat='%s' Where profileid=%d", 
            ProfileData->ProfileTimeRepeat, ProfileData->ProfileId);
        PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
            return SQLITE_PROFILE_ADDSTR_ERROR;
        }       
    }
    if(maskcode&PROFILEDEVICES)
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand,"Update Profile_Table Set profiledevices='%s' Where profileid=%d", 
            ProfileData->ProfileTableDevices, ProfileData->ProfileId);
        PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
            return SQLITE_PROFILE_ADDSTR_ERROR;
        }       
    }

    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : ProfileTableClear
** ��������  : ���Profile�б�
** �������  : ��
** �� �� ֵ  : 
       
** ��    ��  : 2014��10��29��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus ProfileTableClear()
{
    char *pErrMsg = NULL;/*������*/

    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, "Delete From Profile_Table", NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}

///////////////////////////////////////Room Table Function/////////////////////////////////////
/*******************************************************************************
** �� �� ��  : RoomTableAddRecord
** ��������  : �ڷ����б�������һ����¼�������������������ṹ��
** �������  : Room_Table_t *RoomData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus RoomTableAddRecord( Room_Table_t *RoomData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*������*/
    RoomData->GroupId = DBUnUsedRoomId();
    sprintf(SqlCommand,
        "Insert Into Room_Table(roomname,groupid) values('%s',%lu)",RoomData->RoomName,RoomData->GroupId);
    PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    RoomData->Index = sqlite3_last_insert_rowid(SQLITE_DB);
    return SQLITE_SUCCESS;
}

t_SqliteStatus LinkageTableAddRecord( Linkage_Table_t *LinkageData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*������*/
    sprintf(SqlCommand,
        "Insert Into linkage_Table(linkageid,linkagesensoripv6,linkagedeviceslist)" 
        "values(%d,'%s','%s')", 
        LinkageData->LinkageId, LinkageData->LinkageSensorAddress, LinkageData->LinkageDevicesList);
    PCT_vPrintf(DBG_SQLITE,"SqlCommand = %s\n",SqlCommand);
    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    LinkageData->Index = sqlite3_last_insert_rowid(SQLITE_DB);
    return SQLITE_SUCCESS;
}

t_SqliteStatus LinkageTableDelete( Linkage_Table_t *LinkageData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*������*/
    sprintf(SqlCommand,
        "Delete From Linkage_Table Where linkagesensoripv6='%s'", LinkageData->LinkageSensorAddress);
    PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}

t_SqliteStatus LinkageTableClear()
{
    char *pErrMsg = NULL;/*������*/

    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, "Delete From Linkage_Table", NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}

t_SqliteStatus LinkageTableSearchOne( Linkage_Table_t *LinkageData )
{
    char SqlCommand[1024] = {0};
    uint8_t ret = 0;
    sqlite3_stmt * stmt = NULL; 
    
    sprintf(SqlCommand,
        "Select * From Linkage_Table where linkagesensoripv6='%s'",
        LinkageData->LinkageSensorAddress);
    PCT_vPrintf(DBG_SQLITE, "SqlCommand--->%s\n", SqlCommand);

    if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, SqlCommand, -1, &stmt, NULL))
    {
        PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
        return SQLITE_FIND_ERR;
    }
    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        ret ++;
        LinkageData->Index = sqlite3_column_int(stmt, 0);
        LinkageData->LinkageId = sqlite3_column_int(stmt, 1);
        memcpy(LinkageData->LinkageSensorAddress,(char*)sqlite3_column_text(stmt, 2), sqlite3_column_bytes(stmt,2));
        LinkageData->LinkageDevicesList = (char *)malloc(sqlite3_column_bytes(stmt, 3)+1);
        if ( LinkageData->LinkageDevicesList == NULL )
        {
            return SQLITE_MALLOC_ERR;
        }
        memset(LinkageData->LinkageDevicesList, 0, sqlite3_column_bytes(stmt, 3)+1);
        memcpy(LinkageData->LinkageDevicesList, (char*)sqlite3_column_text(stmt, 3), sqlite3_column_bytes(stmt, 3));
        //PCT_vPrintf(DBG_SQLITE, "%d,%s\n%s\n", sqlite3_column_bytes(stmt,7),ProfileData->ProfileTableDevices,(char*)sqlite3_column_text(stmt, 7));
    }
    sqlite3_finalize(stmt);

    if(!ret)
        return SQLITE_NOT_FOUND;
    return SQLITE_SUCCESS;
}

void LinkageTableSearchOneFree( Linkage_Table_t *LinkageData )
{
    if(LinkageData->LinkageDevicesList != NULL)
    {
        free(LinkageData->LinkageDevicesList);
        LinkageData->LinkageDevicesList = NULL;
    }   
}

/*******************************************************************************
** �� �� ��  : RoomTableDelete
** ��������  : �ӷ����б���ɾ��һ����¼�����ݷ���ID����ɾ��
** �������  : Room_Table_t *RoomData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pc
*******************************************************************************/
t_SqliteStatus RoomTableDelete( Room_Table_t *RoomData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*������*/
    sprintf(SqlCommand,
        "Delete From Room_Table Where groupid=%d", RoomData->GroupId);
    PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}

/*******************************************************************************
** �� �� ��  : RoomTableUpdate
** ��������  : ���·����б��������������£�����ͨ����������������봫��room
               ����id��Ϊ����
** �������  : Room_Table_t *RoomData  
             : uint8_t maskcode        
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus RoomTableUpdate( Room_Table_t *RoomData, uint8_t maskcode )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*������*/
    if(maskcode&ROOMUSERNAME)
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand,"Update Room_Table Set roomname='%s' Where groupid=%d", 
            RoomData->RoomName,RoomData->GroupId);
        PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
            return SQLITE_ROOMTABLE_NAME_ERROR;
        }       
    }
    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : RoomTableClear
** ��������  : ��շ����б�
** �������  : ��
** �� �� ֵ  : 
       
** ��    ��  : 2014��10��29��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus RoomTableClear()
{
    char *pErrMsg = NULL;/*������*/

    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, "Delete From Room_Table", NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}


///////////////////////////////////////Profile List Table Function/////////////////////////////
/*******************************************************************************
** �� �� ��  : ProfileListTableAddRecord
** ��������  : ΪProfileList������һ����¼������ǰ��ҪProfile��Scene���ڣ���
               ����֪��Profile��Scene������ֵ����ʹ�ú���SceneTableSearchOne
               �Լ�ProfileTableSearchOne���½ṹ�壬�ҵ�������Ȼ�������ṹ
               �帳ֵ��Profile_List_Table_t���͵Ľṹ�壬�ٵ��ô˺������
** �������  : Profile_List_Table_t *ProfileListData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��10��30��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus ProfileListTableAddRecord( Profile_List_Table_t *ProfileListData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*������*/
    sprintf(SqlCommand,
        "Insert Into Profile_List_Table(sceneid,profileid) values(%d,%d)",
        ProfileListData->SceneId.Index, ProfileListData->ProfileId.Index);
    PCT_vPrintf(DBG_SQLITE,"SqlCommand = %s\n",SqlCommand);
    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    ProfileListData->Index = sqlite3_last_insert_rowid(SQLITE_DB);
    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : ProfileListTableDelete
** ��������  : ɾ��һ����¼������Profile��id����ɾ�������Ա��봴��һ��Profile
               �Ľṹ�壬������Profile id��ֵ��Ȼ�󽫽ṹ�帳ֵ��ProfileList
               �Ľṹ���ٵ��ô˺�������ɾ��
** �������  : Profile_List_Table_t *ProfileListData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��10��30��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus ProfileListTableDelete( Profile_List_Table_t *ProfileListData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*������*/
    sprintf(SqlCommand,
        "Delete From Profile_List_Table Where profileid=%d", ProfileListData->ProfileId.Index);
    PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : ProfileListTableUpdate
** ��������  : ����Profile�б����봫��Profile��id��Ϊ������ͬʱ���������
               �и���
** �������  : Profile_List_Table_t *ProfileListData  
             : uint8_t maskcode                      
** �� �� ֵ  : 
       
** ��    ��  : 2014��10��29��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus ProfileListTableUpdate( Profile_List_Table_t *ProfileListData, uint8_t maskcode )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*������*/
    if(maskcode&PROFILELISTSCENEID)
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand,"Update Profile_List_Table Set sceneid=%d Where profileid=%d", 
            ProfileListData->SceneId.Index, ProfileListData->ProfileId.Index);
        PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
            return SQLITE_PROFILELISTTABLE_NAME_ERROR;
        }       
    }
    return SQLITE_SUCCESS;
}

t_SqliteStatus ProfileListTableClear()
{
    char *pErrMsg = NULL;/*������*/

    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, "Delete From Profile_List_Table", NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}


///////////////////////////////////////List Table Function/////////////////////////////////////
/*******************************************************************************
** �� �� ��  : RoomListTableAddRecord
** ��������  : Ϊ�����豸�б����Ӽ�¼��ʹ�÷���ͬProfileListTableAddRecord
** �������  : Room_List_Table_t *RoomListData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��10��30��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus RoomListTableAddRecord( Room_List_Table_t *RoomListData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*������*/
    sprintf(SqlCommand,
        "Insert Into Room_List_Table(deviceid,roomid) values(%d,%d)",
        RoomListData->DeviceId.Index, RoomListData->RoomId.Index);
    PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    RoomListData->Index = sqlite3_last_insert_rowid(SQLITE_DB);
    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : RoomListTableDelete
** ��������  : ʹ�÷���ͬProfileList
** �������  : Room_List_Table_t *RoomListData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��10��30��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus RoomListTableDelete( Room_List_Table_t *RoomListData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*������*/
    sprintf(SqlCommand,
        "Delete From Room_List_Table Where roomid=%d", RoomListData->RoomId.Index);
    PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : RoomListTableDeleteUnderDevice
** ��������  : ɾ����������豸
** �������  : Room_List_Table_t *RoomListData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��12��5��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus RoomListTableDeleteUnderDevice( Room_List_Table_t *RoomListData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*������*/
    sprintf(SqlCommand,
        "Delete From Room_List_Table Where deviceid=%d", RoomListData->DeviceId.Index);
    PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}

/*******************************************************************************
** �� �� ��  : RoomListTableUpdate
** ��������  : ���·����豸�б��������豸���ڵ���
** �������  : Room_List_Table_t *RoomListData  
             : uint8_t maskcode                 
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus RoomListTableUpdate( Room_List_Table_t *RoomListData, uint8_t maskcode )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*������*/
    if(maskcode&ROOMLISTSCENEID)
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand,"Update Room_List_Table Set roomid=%d Where deviceid=%d", 
            RoomListData->RoomId.Index, RoomListData->DeviceId.Index );
        PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
            return SQLITE_ROOMLISTTABLE_NAME_ERROR;
        }       
    }
    return SQLITE_SUCCESS;
}

t_SqliteStatus RoomListTableClear()
{
    char *pErrMsg = NULL;/*������*/

    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, "Delete From Room_List_Table", NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}
/////////////////////////////////////��ѯ/////////////////////////////////////////////
/*******************************************************************************
** �� �� ��  : SceneTableSearch
** ��������  : ��ѯ�����ļ�¼������ʱ���������ʱ����һ���ṹ�壬������һ
               ��������ѯʱ���������ѯ����������µĳ���
** �������  : Scene_Table_t *SceneData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��10��29��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus SceneTableSearch( Scene_Table_t *SceneData)
{
    sqlite3_stmt * stmt = NULL; 
    Scene_Table_t *Temp = NULL;
    PCT_vPrintf(DBG_SQLITE, "SceneTableSearch\n");
    
    if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, "Select * From Scene_Table Order By sceneid asc", -1, &stmt, NULL))
    {
        PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
        return SQLITE_FIND_ERR;
    }
    dl_list_init ( &SceneData->list ); /*��ʼ������*/
    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        Temp = ( Scene_Table_t * ) malloc ( sizeof ( Scene_Table_t ) );
        if ( Temp == NULL )
        {
            return SQLITE_MALLOC_ERR;
        }
        memset(Temp,0,sizeof(Scene_Table_t));
        Temp->Index = sqlite3_column_int(stmt, 0);
        Temp->SceneId = sqlite3_column_int(stmt, 1);
        //printf("Temp->Index=%d\n",Temp->Index);
        memcpy( Temp->TimeSpan, (char*)sqlite3_column_text(stmt, 2), sqlite3_column_bytes(stmt,2));
        Temp->OnOff= sqlite3_column_int(stmt, 3);
        Temp->RepeatMode = sqlite3_column_int(stmt, 4);

        dl_list_add_tail(&SceneData->list, &Temp->list);
    }
    sqlite3_finalize(stmt);

    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : SceneTableSearchFree
** ��������  : �ͷŲ�ѯʱ������ڴ�
** �������  : Scene_Table_t *SceneData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
void SceneTableSearchFree( Scene_Table_t *SceneData)
{
    Scene_Table_t *Temp = NULL;
    Scene_Table_t *Temp2 = NULL;
    PCT_vPrintf(DBG_SQLITE, "SceneTableSearchFree\n");
    dl_list_for_each_safe(Temp,Temp2,&SceneData->list,Scene_Table_t,list)
    {
        dl_list_del ( &Temp->list );
        free(Temp);
        Temp = NULL;
    }
    Temp2 = NULL;
}
/*******************************************************************************
** �� �� ��  : DeviceTableSearch
** ��������  : �����豸�ṹ���ָ�룬Ȼ��ʹ��ָ���е�������������豸
** �������  : Device_Table_t *DeviceData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��10��30��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus  DeviceTableSearch( Device_Table_t *DeviceData)
{
    sqlite3_stmt * stmt = NULL; 
    Device_Table_t *Temp = NULL;
    
    if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, "Select * From Device_Table", -1, &stmt, NULL))
    {
        PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
        return SQLITE_FIND_ERR;
    }
    PCT_vPrintf(DBG_SQLITE,"Enter DeviceTableSearch Loop\n");
    dl_list_init ( &DeviceData->list ); /*��ʼ������*/
    
    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        Temp = ( Device_Table_t * ) malloc ( sizeof ( Device_Table_t ) );
        if ( Temp == NULL )
        {
            PCT_vPrintf(DBG_SQLITE,"Malloc Error\n");
            return SQLITE_MALLOC_ERR;
        }
        memset(Temp,0,sizeof(Device_Table_t));
        Temp->Index = sqlite3_column_int(stmt, 0);
        Temp->DeviceId= sqlite3_column_int(stmt, 1);
        //printf("Temp->Index=%d\n",Temp->Index);
        memcpy( Temp->DeviceName, (char*)sqlite3_column_text(stmt, 2), sqlite3_column_bytes(stmt,2));
        memcpy( Temp->UserName, (char*)sqlite3_column_text(stmt, 3), sqlite3_column_bytes(stmt,3));
        Temp->Address = sqlite3_column_int(stmt, 4);
        memcpy( Temp->DeviceIpv6, (char*)sqlite3_column_text(stmt, 5), sqlite3_column_bytes(stmt,5));
        Temp->MacAddress = sqlite3_column_int64(stmt, 6);
        Temp->DeviceType = sqlite3_column_int(stmt, 7);
        Temp->DeviceOnOff = sqlite3_column_int(stmt, 8);
        Temp->DeviceValue1 = sqlite3_column_int(stmt, 9);
        Temp->DeviceValue2 = sqlite3_column_int(stmt, 10);
        Temp->DeviceIsOnline = sqlite3_column_int(stmt, 11);
        memcpy( Temp->DeviceAppendstr, (char*)sqlite3_column_text(stmt, 12), sqlite3_column_bytes(stmt,12));
        //PCT_vPrintf(DBG_SQLITE,"dl_list_add_tail\n");

        dl_list_add_tail(&DeviceData->list, &Temp->list);
    }
    sqlite3_finalize(stmt);

    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : DeviceTableSearchFree
** ��������  : �ͷŲ�ѯʱ������ڴ�
** �������  : Device_Table_t *DeviceData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
void DeviceTableSearchFree( Device_Table_t *DeviceData)
{
    Device_Table_t *Temp = NULL;
    Device_Table_t *Temp2 = NULL;
    PCT_vPrintf(DBG_SQLITE, "DeviceTableSearchFree\n");
    dl_list_for_each_safe(Temp,Temp2,&DeviceData->list,Device_Table_t,list)
    {
        dl_list_del ( &Temp->list );
        free(Temp);
        Temp = NULL;
    }
    Temp2 = NULL;       
}
/*******************************************************************************
** �� �� ��  : RoomTableSearch
** ��������  : ��ѯ�����б����ж��ٸ������Լ�����ľ�����Ϣ
** �������  : Room_Table_t *RoomData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus  RoomTableSearch( Room_Table_t *RoomData)
{
    sqlite3_stmt * stmt = NULL; 
    Room_Table_t *Temp = NULL;

    if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, "Select * From Room_Table Order By groupid asc", -1, &stmt, NULL))
    {
        PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
        return SQLITE_FIND_ERR;
    }

    PCT_vPrintf(DBG_SQLITE,"Enter RoomTableSearch Loop\n");
    dl_list_init ( &RoomData->list ); /*��ʼ������*/
    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        Temp = ( Room_Table_t * ) malloc ( sizeof ( Room_Table_t ) );
        if ( Temp == NULL )
        {
            return SQLITE_MALLOC_ERR;
        }
        memset(Temp,0,sizeof(Room_Table_t));
        Temp->Index = sqlite3_column_int(stmt, 0);
        memcpy( Temp->RoomName, (char*)sqlite3_column_text(stmt, 1), sqlite3_column_bytes(stmt,1));
        Temp->GroupId= sqlite3_column_int(stmt, 2);

        dl_list_add_tail(&RoomData->list, &Temp->list);
    }
    sqlite3_finalize(stmt);

    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : RoomTableSearchFree
** ��������  : �ͷŲ�ѯ����ʱ������ڴ�ռ�
** �������  : Room_Table_t *RoomData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
void RoomTableSearchFree( Room_Table_t *RoomData)
{
    Room_Table_t *Temp = NULL;
    Room_Table_t *Temp2 = NULL;
    PCT_vPrintf(DBG_SQLITE, "RoomTableSearchFree\n");
    dl_list_for_each_safe(Temp,Temp2,&RoomData->list,Room_Table_t,list)
    {
        dl_list_del ( &Temp->list );
        free(Temp);
        Temp = NULL;
    }
    Temp2 = NULL;
}
/*******************************************************************************
** �� �� ��  : ProfileTableSearch
** ��������  : ��ѯProfile�����Լ���ϸ��Ϣ
** �������  : Profile_Table_t *ProfileData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus  ProfileTableSearch( Profile_Table_t *ProfileData)
{
    sqlite3_stmt * stmt = NULL; 
    Profile_Table_t *Temp = NULL;
    
    if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, "Select * From Profile_Table Order By profileid asc", -1, &stmt, NULL))
    {
        PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
        return SQLITE_FIND_ERR;
    }

    PCT_vPrintf(DBG_SQLITE,"Enter ProfileTableSearch Loop\n");
    dl_list_init ( &ProfileData->list ); /*��ʼ������*/
    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        Temp = ( Profile_Table_t * ) malloc ( sizeof ( Profile_Table_t ) );
        if ( Temp == NULL )
        {
            return SQLITE_MALLOC_ERR;
        }
        memset(Temp,0,sizeof(Profile_Table_t));
        Temp->Index = sqlite3_column_int(stmt, 0);
        Temp->ProfileId = sqlite3_column_int(stmt, 1);
        memcpy( Temp->ProfileName, (char*)sqlite3_column_text(stmt, 2), sqlite3_column_bytes(stmt,2));
        memcpy( Temp->ProfileOnOff, (char*)sqlite3_column_text(stmt, 3), sqlite3_column_bytes(stmt,3));
        memcpy( Temp->ProfileStartTime, (char*)sqlite3_column_text(stmt, 4), sqlite3_column_bytes(stmt,4));
        memcpy( Temp->ProfileEndTime, (char*)sqlite3_column_text(stmt, 5), sqlite3_column_bytes(stmt,5));
        memcpy( Temp->ProfileTimeRepeat, (char*)sqlite3_column_text(stmt, 6), sqlite3_column_bytes(stmt,6));
        Temp->ProfileTableDevices= malloc(sqlite3_column_bytes(stmt, 7)+1);
        //PCT_vPrintf(DBG_SQLITE,"leave ProfileTableSearch\n");
        if ( Temp->ProfileTableDevices == NULL )
        {
            return SQLITE_MALLOC_ERR;
        }
        memset(Temp->ProfileTableDevices, 0, sqlite3_column_bytes(stmt, 7)+1);
        memcpy(Temp->ProfileTableDevices, (char*)sqlite3_column_text(stmt, 7), sqlite3_column_bytes(stmt, 7));
        //printf("%s\n%s\n",(char*)sqlite3_column_text(stmt, 7),Temp->ProfileTableDevices);
        dl_list_add_tail(&ProfileData->list, &Temp->list);
    }
    sqlite3_finalize(stmt);
    //PCT_vPrintf(DBG_SQLITE,"leave ProfileTableSearch\n");
    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : ProfileTableOnOff
** ��������  : ����Profile��ONOFF���ԣ������ú�Profile��ص�Scened��ONOFF��
               ��
** �������  : Profile_Table_t *ProfileData  
             : uint8_t onoff                 
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus  ProfileTableOnOff( Profile_Table_t *ProfileData, uint8_t onoff)
{
    char SqlCommand[1024] = {0};
    sqlite3_stmt * stmtProfile = NULL; 
    char *pErrMsg = NULL;/*������*/
    t_SqliteStatus Status = 0;
    if(Status = ProfileTableSearchOne(ProfileData))
    {
        return Status;
    }
    sprintf(SqlCommand,"Select * From Profile_List_Table where profileid=%d",ProfileData->Index);
    PCT_vPrintf(DBG_SQLITE,"SqlCommand = %s\n", SqlCommand);
    ProfileTableSearchOneFree(ProfileData);
    
    if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, SqlCommand, -1, &stmtProfile, NULL))
    {
        PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
        return SQLITE_FIND_ERR;
    }
    while(sqlite3_step(stmtProfile) == SQLITE_ROW)
    {
        memset(SqlCommand, 0, sizeof(SqlCommand));
        sprintf(SqlCommand, "Update Scene_Table set onoff=%d where indexnum=%d", onoff, sqlite3_column_int(stmtProfile, 1));    
        PCT_vPrintf(DBG_SQLITE,"SqlCommand = %s\n", SqlCommand);
        if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
        {
            PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
            return SQLITE_SCENETABLE_ONOFF_ERROR;
        }       
    }
    sqlite3_finalize(stmtProfile);

    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : ProfileTableSearchFree
** ��������  : �ͷŲ�ѯProfileʱ������ڴ�ռ�
** �������  : Profile_Table_t *ProfileData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
void  ProfileTableSearchFree( Profile_Table_t *ProfileData)
{
    Profile_Table_t *Temp = NULL;
    Profile_Table_t *Temp2 = NULL;
    PCT_vPrintf(DBG_SQLITE, "ProfileTableSearchFree\n");
    dl_list_for_each_safe(Temp,Temp2,&ProfileData->list,Profile_Table_t,list)
    {
        free(Temp->ProfileTableDevices);
        Temp->ProfileTableDevices = NULL;
        dl_list_del( &Temp->list );
        free(Temp);
        Temp = NULL;
    }
    Temp2 = NULL;
}
/*******************************************************************************
** �� �� ��  : DeviceTableSearchOne
** ��������  : ��ֻ֪���豸IPV6��ַ������£���ȡ�豸��������Ϣ
** �������  : Device_Table_t *DeviceData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus DeviceTableSearchOne( Device_Table_t *DeviceData )
{
    char SqlCommand[1024] = {0};
    sqlite3_stmt * stmt = NULL; 
    uint8_t ret = 0;
    sprintf(SqlCommand,
        "Select * From Device_Table where deviceipv6='%s'",
        DeviceData->DeviceIpv6);
    PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
    if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, SqlCommand, -1, &stmt, NULL))
    {
        PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
        return SQLITE_FIND_ERR;
    }
    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        ret ++;
        DeviceData->Index = sqlite3_column_int(stmt, 0);
        DeviceData->DeviceId = sqlite3_column_int(stmt, 1);
        memcpy(DeviceData->DeviceName,(char*)sqlite3_column_text(stmt, 2), sqlite3_column_bytes(stmt,2));
        memcpy(DeviceData->UserName,(char*)sqlite3_column_text(stmt, 3), sqlite3_column_bytes(stmt,3));
        DeviceData->Address= sqlite3_column_int(stmt, 4);
        memcpy(DeviceData->DeviceIpv6,(char*)sqlite3_column_text(stmt, 5), sqlite3_column_bytes(stmt,5));
        DeviceData->MacAddress = sqlite3_column_int64(stmt, 6);
        PCT_vPrintf(DBG_SQLITE,"%llu\n%llu\n",sqlite3_column_int64(stmt, 6),DeviceData->MacAddress);
        DeviceData->DeviceType = sqlite3_column_int(stmt, 7);
        DeviceData->DeviceOnOff = sqlite3_column_int(stmt, 8);
        DeviceData->DeviceValue1 = sqlite3_column_int(stmt, 9);
        DeviceData->DeviceValue2 = sqlite3_column_int(stmt, 10);
        DeviceData->DeviceIsOnline = sqlite3_column_int(stmt, 11);
        memcpy(DeviceData->DeviceAppendstr,(char*)sqlite3_column_text(stmt, 12), sqlite3_column_bytes(stmt,12));
    }
    sqlite3_finalize(stmt);

    if(!ret)
        return SQLITE_NOT_FOUND;
    
    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : SceneTableSearchOne
** ��������  : ��ֻ֪������id������»�ȡ��ǰid������������Ϣ
** �������  : Scene_Table_t *SceneData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��10��30��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus SceneTableSearchOne( Scene_Table_t *SceneData )
{
    char SqlCommand[1024] = {0};
    sqlite3_stmt * stmt = NULL; 
    uint8_t ret = 0;
    
    sprintf(SqlCommand,
        "Select * From Scene_Table where sceneid=%d",
        SceneData->SceneId);
    PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
    if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, SqlCommand, -1, &stmt, NULL))
    {
        PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
        return SQLITE_FIND_ERR;
    }
    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        ret ++;
        SceneData->Index = sqlite3_column_int(stmt, 0);
        SceneData->SceneId = sqlite3_column_int(stmt, 1);
        memcpy(SceneData->TimeSpan,(char*)sqlite3_column_text(stmt, 2), sqlite3_column_bytes(stmt,2));
        SceneData->OnOff= sqlite3_column_int(stmt, 3);
        SceneData->RepeatMode= sqlite3_column_int(stmt, 4);
    }
    sqlite3_finalize(stmt);

    if(!ret)
        return SQLITE_NOT_FOUND;
    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : RoomTableSearchOne
** ��������  : ��ֻ֪������id������»�ȡ����������Ϣ
** �������  : Room_Table_t *RoomData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��10��30��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus RoomTableSearchOne( Room_Table_t *RoomData )
{
    char SqlCommand[1024] = {0};
    sqlite3_stmt * stmt = NULL; 
    uint8_t ret = 0;
    
    sprintf(SqlCommand,
        "Select * From Room_Table where groupid=%d",
        RoomData->GroupId);
    PCT_vPrintf(DBG_SQLITE, "%s\n",SqlCommand);
    if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, SqlCommand, -1, &stmt, NULL))
    {
        PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
        return SQLITE_FIND_ERR;
    }
    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        ret ++;
        RoomData->Index = sqlite3_column_int(stmt, 0);
        memcpy(RoomData->RoomName,(char*)sqlite3_column_text(stmt, 1), sqlite3_column_bytes(stmt,1));
        RoomData->GroupId= sqlite3_column_int(stmt, 2);
    }
    sqlite3_finalize(stmt);

    if(!ret)
        return SQLITE_NOT_FOUND;
    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : ProfileTableSearchOne
** ��������  : ��ֻ֪��profile id������»�ȡ������Ϣ
** �������  : Profile_Table_t *ProfileData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��10��30��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus ProfileTableSearchOne( Profile_Table_t *ProfileData )
{
    char SqlCommand[1024] = {0};
    uint8_t ret = 0;
    sqlite3_stmt * stmt = NULL; 
    
    sprintf(SqlCommand,
        "Select * From Profile_Table where profileid=%d",
        ProfileData->ProfileId);
    PCT_vPrintf(DBG_SQLITE, "SqlCommand--->%s\n", SqlCommand);

    if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, SqlCommand, -1, &stmt, NULL))
    {
        PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
        return SQLITE_FIND_ERR;
    }
    while(sqlite3_step(stmt) == SQLITE_ROW)
    {
        ret ++;
        ProfileData->Index = sqlite3_column_int(stmt, 0);
        ProfileData->ProfileId= sqlite3_column_int(stmt, 1);
        memcpy(ProfileData->ProfileName,(char*)sqlite3_column_text(stmt, 2), sqlite3_column_bytes(stmt,2));
        memcpy(ProfileData->ProfileOnOff,(char*)sqlite3_column_text(stmt, 3), sqlite3_column_bytes(stmt,3));
        memcpy( ProfileData->ProfileStartTime, (char*)sqlite3_column_text(stmt, 4), sqlite3_column_bytes(stmt,4));
        memcpy( ProfileData->ProfileEndTime, (char*)sqlite3_column_text(stmt, 5), sqlite3_column_bytes(stmt,5));
        memcpy( ProfileData->ProfileTimeRepeat, (char*)sqlite3_column_text(stmt, 6), sqlite3_column_bytes(stmt,6));
        ProfileData->ProfileTableDevices = (char *)malloc(sqlite3_column_bytes(stmt, 7)+1);
        if ( ProfileData->ProfileTableDevices == NULL )
        {
            return SQLITE_MALLOC_ERR;
        }
        memset(ProfileData->ProfileTableDevices, 0, sqlite3_column_bytes(stmt, 7)+1);
        memcpy(ProfileData->ProfileTableDevices, (char*)sqlite3_column_text(stmt, 7), sqlite3_column_bytes(stmt, 7));
        
        //printf( "%d,%s\n%s\n", sqlite3_column_bytes(stmt,7),ProfileData->ProfileTableDevices,(char*)sqlite3_column_text(stmt, 7));
    }
    sqlite3_finalize(stmt);

    if(!ret)
        return SQLITE_NOT_FOUND;
    return SQLITE_SUCCESS;
}

/*******************************************************************************
** �� �� ��  : ProfileTableSearchOneFree
** ��������  : �ͷ�Profile��ѯһ����¼ʱ������豸�ַ����ռ�
** �������  : Profile_Table_t *ProfileData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
void ProfileTableSearchOneFree( Profile_Table_t *ProfileData )
{
    if(ProfileData->ProfileTableDevices!= NULL)
    {
        free(ProfileData->ProfileTableDevices);
        ProfileData->ProfileTableDevices = NULL;
    }
}
/*******************************************************************************
** �� �� ��  : ProfileDeviceListSearch
** ��������  : ��ѯ����Profile���豸��������Ҫ��ѯ��Profile ID���Լ�һ���ṹ
               �壬ͨ���ṹ�������ȡ��ѯ����
** �������  : uint16_t ProfileId          
             : Device_Table_t *DeviceData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��10��29��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus ProfileDeviceListSearch( uint16_t ProfileId, Device_Table_t *DeviceData )
{
    char SqlCommand[1024] = {0};
    sqlite3_stmt * stmtprofile; 
    sqlite3_stmt * stmtprofilelist; 
    sqlite3_stmt * stmtdevice; 
    sqlite3_stmt * stmtscene; 
    
    Device_Table_t *Temp = NULL;
    Device_Table_t *TempLoop = NULL;
    dl_list_init ( &DeviceData->list ); /*��ʼ������*/

    sprintf(SqlCommand,"Select * From Profile_Table where profileid=%d", ProfileId);
    PCT_vPrintf(DBG_SQLITE, "SqlCommand--->%s\n", SqlCommand);
    
    if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, SqlCommand, -1, &stmtprofile, NULL))
    {
        PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
        return SQLITE_FIND_ERR;
    }
    else
    {
        while(sqlite3_step(stmtprofile) == SQLITE_ROW)//��Profile���в��Profile Id
        {
            memset(SqlCommand,0,sizeof(SqlCommand));
            sprintf(SqlCommand,"Select * From Profile_List_Table where profileid=%d", sqlite3_column_int(stmtprofile, 0));
            PCT_vPrintf(DBG_SQLITE, "SqlCommand--->%s\n", SqlCommand);

            if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, SqlCommand, -1, &stmtprofilelist, NULL))
            {
                PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
                return SQLITE_FIND_ERR;
            }
            else
            {
                while(sqlite3_step(stmtprofilelist) == SQLITE_ROW)//��Profile_List�в��Scene Index
                {
                    memset(SqlCommand,0,sizeof(SqlCommand));
                    sprintf(SqlCommand,"Select * From Scene_Device_Table where indexnum=%d", sqlite3_column_int(stmtprofilelist, 1));
                    PCT_vPrintf(DBG_SQLITE, "SqlCommand--->%s\n", SqlCommand);
                    
                    if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, SqlCommand, -1, &stmtscene, NULL))
                    {
                        PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
                        return SQLITE_FIND_ERR;
                    }
                    else
                    {
                        while(sqlite3_step(stmtscene) == SQLITE_ROW)//��Scene Device���в��Device Index
                        {
                            memset(SqlCommand,0,sizeof(SqlCommand));
                            sprintf(SqlCommand,"Select * From Device_Table where indexnum=%d", sqlite3_column_int(stmtscene, 2));
                            PCT_vPrintf(DBG_SQLITE, "SqlCommand--->%s\n", SqlCommand);
                            
                            if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, SqlCommand, -1, &stmtdevice, NULL))
                            {
                                PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
                                return SQLITE_FIND_ERR;
                            }
                            else
                            {
LOOP:
                                while(sqlite3_step(stmtdevice) == SQLITE_ROW)
                                {
                                    Temp = ( Device_Table_t * ) malloc ( sizeof ( Device_Table_t ) );           
                                    if ( Temp == NULL )
                                    {
                                        return SQLITE_MALLOC_ERR;
                                    }
                                    memset(Temp,0,sizeof(Device_Table_t));
                                    Temp->Index = sqlite3_column_int(stmtdevice, 0);
                                    Temp->DeviceId = sqlite3_column_int(stmtdevice, 1);
                                    memcpy(Temp->DeviceName, (char*)sqlite3_column_text(stmtdevice,2), sqlite3_column_bytes(stmtdevice,2));
                                    memcpy(Temp->UserName, (char*)sqlite3_column_text(stmtdevice,3), sqlite3_column_bytes(stmtdevice,3));
                                    Temp->Address = sqlite3_column_int(stmtdevice, 4);
                                    memcpy(Temp->DeviceIpv6, (char*)sqlite3_column_text(stmtdevice,5), sqlite3_column_bytes(stmtdevice,5));
                                    Temp->MacAddress = sqlite3_column_int64(stmtdevice, 6);
                                    Temp->DeviceType = sqlite3_column_int(stmtdevice, 7);
                                    Temp->DeviceOnOff = sqlite3_column_int(stmtdevice, 8);
                                    Temp->DeviceValue1 = sqlite3_column_int(stmtdevice, 9);
                                    Temp->DeviceValue2 = sqlite3_column_int(stmtdevice, 10);
                                    Temp->DeviceIsOnline = sqlite3_column_int(stmtdevice, 11);
                                    memcpy(Temp->DeviceAppendstr, (char*)sqlite3_column_text(stmtdevice,12), sqlite3_column_bytes(stmtdevice,12));
                                    //Filtered Same Profile Devices
                                    dl_list_for_each(TempLoop,&DeviceData->list,Device_Table_t,list)
                                    {
                                        if(!strcmp(TempLoop->DeviceIpv6, Temp->DeviceIpv6))
                                        {
                                            free(Temp);
                                            goto LOOP;
                                        }
                                    }
                                    dl_list_add_tail(&DeviceData->list, &Temp->list);
                                }
                                sqlite3_finalize(stmtdevice);
                            }
                        }
                        sqlite3_finalize(stmtscene);
                    }
                }
                sqlite3_finalize(stmtprofilelist); 
            }
        }
        sqlite3_finalize(stmtprofile); 
    }
    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : ProfileSceneListSearch
** ��������  : ����Profile��ID��ѯ�ʹ�Profile��ص�Scene
** �������  : uint16_t ProfileId        
             : Scene_Table_t *SceneData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus ProfileSceneListSearch( uint16_t ProfileId, Scene_Table_t *SceneData )
{
    char SqlCommand[1024] = {0};
    sqlite3_stmt * stmtprofile;  
    sqlite3_stmt * stmtscene; 
    
    Scene_Table_t *Temp = NULL;
    Profile_Table_t ProfileData;
    memset(&ProfileData, 0, sizeof(Profile_Table_t));
    ProfileData.ProfileId = ProfileId;
    ProfileTableSearchOne(&ProfileData);
    sprintf(SqlCommand,"Select * From Profile_List_Table where profileid=%d", ProfileData.Index);
    PCT_vPrintf(DBG_SQLITE, "SqlCommand--->%s\n", SqlCommand);
    
    dl_list_init ( &SceneData->list ); /*��ʼ������*/
    if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, SqlCommand, -1, &stmtprofile, NULL))
    {
        PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
        return SQLITE_FIND_ERR;
    }
    else
    {
        while(sqlite3_step(stmtprofile) == SQLITE_ROW)//��Profile List���в��Profile Id
        {
            memset(SqlCommand,0,sizeof(SqlCommand));
            sprintf(SqlCommand,"Select * From Scene_Table where indexnum=%d", sqlite3_column_int(stmtprofile, 1));
            PCT_vPrintf(DBG_SQLITE, "SqlCommand--->%s\n", SqlCommand);
            if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, SqlCommand, -1, &stmtscene, NULL))
            {
                PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
                return SQLITE_FIND_ERR;
            }
            else
            {
                while(sqlite3_step(stmtscene) == SQLITE_ROW)//��Scene���в��Scene
                {
                    Temp = ( Scene_Table_t * ) malloc ( sizeof ( Scene_Table_t ) );         
                    if ( Temp == NULL )
                    {
                        return SQLITE_MALLOC_ERR;
                    }
                    memset(Temp,0,sizeof(Scene_Table_t));
                    Temp->Index = sqlite3_column_int(stmtscene, 0);
                    Temp->SceneId = sqlite3_column_int(stmtscene, 1);
                    memcpy(Temp->TimeSpan, (char*)sqlite3_column_text(stmtscene,2), sqlite3_column_bytes(stmtscene,2));
                    Temp->OnOff = sqlite3_column_int(stmtscene, 3);
                    Temp->RepeatMode = sqlite3_column_int(stmtscene, 4);
                    dl_list_add_tail(&SceneData->list, &Temp->list);
                }
                sqlite3_finalize(stmtscene); 
            }
            
        }
    }
    ProfileTableSearchOneFree(&ProfileData);
    return SQLITE_SUCCESS;
}
/*******************************************************************************
** �� �� ��  : RoomDeviceListSearch
** ��������  : ���ݷ�������豸�б�roomid���������ID������������ɷ��ؽ�
               ��������
** �������  : uint16_t RoomId             
             : Device_Table_t *DeviceData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��10��30��
** ��    ��  : pc
*******************************************************************************/
t_SqliteStatus RoomDeviceListSearch( uint32_t RoomId, Device_Table_t *DeviceData )
{
    char SqlCommand[1024] = {0};
    sqlite3_stmt * room = NULL; 
    sqlite3_stmt * device = NULL; 
    sqlite3_stmt * roomlist = NULL; 
    Device_Table_t *Temp = NULL;
    dl_list_init ( &DeviceData->list ); /*��ʼ������*/

    sprintf(SqlCommand,"Select * From Room_Table where groupid=%d", RoomId);
    PCT_vPrintf(DBG_SQLITE, "SqlCommand--->%s\n", SqlCommand);
    /*���ҷ����б��ID*/
    if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, SqlCommand, -1, &room, NULL))
    {
        PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
        return SQLITE_FIND_ERR;
    }
    else
    {
        while(sqlite3_step(room) == SQLITE_ROW)
        {
            memset(SqlCommand,0,sizeof(SqlCommand));
            sprintf(SqlCommand,"Select * From Room_List_Table where roomid=%d", sqlite3_column_int(room, 0));
            PCT_vPrintf(DBG_SQLITE, "SqlCommand--->%s\n", SqlCommand);
            /*���ҷ����豸�б���豸����*/
            if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, SqlCommand, -1, &roomlist, NULL))
            {
                PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
                return SQLITE_FIND_ERR;
            }
            else
            {
                while(sqlite3_step(roomlist) == SQLITE_ROW)
                {
                    memset(SqlCommand,0,sizeof(SqlCommand));
                    sprintf(SqlCommand,"Select * From Device_Table where indexnum=%d", sqlite3_column_int(roomlist, 1));
                    PCT_vPrintf(DBG_SQLITE, "SqlCommand--->%s\n", SqlCommand);
                    /*�����豸���������豸��Ϣ*/
                    if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, SqlCommand, -1, &device, NULL))
                    {
                        PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
                        return SQLITE_FIND_ERR;
                    }
                    else
                    {
                        while(sqlite3_step(device) == SQLITE_ROW)
                        {
                            Temp = ( Device_Table_t * ) malloc ( sizeof ( Device_Table_t ) );           
                            if ( Temp == NULL )
                            {
                                return SQLITE_MALLOC_ERR;
                            }
                            memset(Temp,0,sizeof(Device_Table_t));
                            Temp->Index = sqlite3_column_int(device, 0);
                            Temp->DeviceId = sqlite3_column_int(device, 1);
                            memcpy(Temp->DeviceName, (char*)sqlite3_column_text(device,2), sqlite3_column_bytes(device,2));
                            memcpy(Temp->UserName, (char*)sqlite3_column_text(device,3), sqlite3_column_bytes(device,3));
                            Temp->Address = sqlite3_column_int(device, 4);
                            memcpy(Temp->DeviceIpv6, (char*)sqlite3_column_text(device,5), sqlite3_column_bytes(device,5));
                            Temp->MacAddress = sqlite3_column_int64(device, 6);
                            Temp->DeviceType = sqlite3_column_int(device, 7);
                            Temp->DeviceOnOff = sqlite3_column_int(device, 8);
                            Temp->DeviceValue1 = sqlite3_column_int(device, 9);
                            Temp->DeviceValue2 = sqlite3_column_int(device, 10);
                            Temp->DeviceIsOnline = sqlite3_column_int(device, 11);
                            memcpy(Temp->DeviceAppendstr, (char*)sqlite3_column_text(device,12), sqlite3_column_bytes(device,12));
                            dl_list_add_tail(&DeviceData->list, &Temp->list);
                        }
                        sqlite3_finalize(device); 
                    }
                }
                sqlite3_finalize(roomlist);
            }
        }
        sqlite3_finalize(room); 
    }

    return SQLITE_SUCCESS;
}

t_SqliteStatus SceneDeviceListSearch( uint16_t SceneId, Device_Table_t *DeviceData )
{
    char SqlCommand[1024] = {0};
    sqlite3_stmt * device = NULL; 
    sqlite3_stmt * scenelist = NULL; 
    Device_Table_t *Temp = NULL;
    dl_list_init ( &DeviceData->list ); /*��ʼ������*/

    sprintf(SqlCommand,"Select * From Scene_Device_Table where sceneindex=%d", SceneId);
    PCT_vPrintf(DBG_SQLITE, "SqlCommand--->%s\n", SqlCommand);
    /*���ҷ����б��ID*/
    if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, SqlCommand, -1, &scenelist, NULL))
    {
        PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
        return SQLITE_FIND_ERR;
    }
    else
    {
        while(sqlite3_step(scenelist) == SQLITE_ROW)
        {
            memset(SqlCommand,0,sizeof(SqlCommand));
            sprintf(SqlCommand,"Select * From Device_Table where indexnum=%d", sqlite3_column_int(scenelist, 2));
            PCT_vPrintf(DBG_SQLITE, "SqlCommand--->%s\n", SqlCommand);
            /*���ҷ����豸�б���豸����*/
            if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, SqlCommand, -1, &device, NULL))
            {
                PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
                return SQLITE_FIND_ERR;
            }
            else
            {
                while(sqlite3_step(device) == SQLITE_ROW)
                {
                    Temp = ( Device_Table_t * ) malloc ( sizeof ( Device_Table_t ) );           
                    if ( Temp == NULL )
                    {
                        return SQLITE_MALLOC_ERR;
                    }
                    memset(Temp,0,sizeof(Device_Table_t));
                    Temp->Index = sqlite3_column_int(device, 0);
                    Temp->DeviceId = sqlite3_column_int(device, 1);
                    memcpy(Temp->DeviceName, (char*)sqlite3_column_text(device,2), sqlite3_column_bytes(device,2));
                    memcpy(Temp->UserName, (char*)sqlite3_column_text(device,3), sqlite3_column_bytes(device,3));
                    Temp->Address = sqlite3_column_int(device, 4);
                    memcpy(Temp->DeviceIpv6, (char*)sqlite3_column_text(device,5), sqlite3_column_bytes(device,5));
                    Temp->MacAddress = sqlite3_column_int64(device, 6);
                    Temp->DeviceType = sqlite3_column_int(device, 7);
                    Temp->DeviceOnOff = sqlite3_column_int(device, 8);
                    Temp->DeviceValue1 = sqlite3_column_int(device, 9);
                    Temp->DeviceValue2 = sqlite3_column_int(device, 10);
                    Temp->DeviceIsOnline = sqlite3_column_int(device, 11);
                    memcpy(Temp->DeviceAppendstr, (char*)sqlite3_column_text(device,12), sqlite3_column_bytes(device,12));
                    dl_list_add_tail(&DeviceData->list, &Temp->list);
                }
                sqlite3_finalize(device);
            }
        }
        sqlite3_finalize(scenelist); 
    }

    return SQLITE_SUCCESS;
}

/*******************************************************************************
** �� �� ��  : DBUnUsedSceneId
** ��������  : ����һ��δʹ�õ�SceneId
** �������  : ��
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
uint16_t DBUnUsedSceneId()
{
    uint16_t SceneId = 1;
    Scene_Table_t SceneTableData,*Temp = NULL;
    SceneTableSearch(&SceneTableData);
    dl_list_for_each(Temp,&SceneTableData.list,Scene_Table_t,list)
    {
        if(SceneId != Temp->SceneId)
        {
            break;
        }
        SceneId ++; 
    }
    SceneTableSearchFree(&SceneTableData);
    Temp = NULL;
    return SceneId;
}
/*******************************************************************************
** �� �� ��  : DBUnUsedRoomId
** ��������  : ����һ��δʹ�õ�Room Id
** �������  : ��
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
uint32_t DBUnUsedRoomId()
{
    uint32_t RoomId = 0x15a00a;
    Room_Table_t RoomTableData,*Temp = NULL;
    memset(&RoomTableData, 0, sizeof(Room_Table_t));
    RoomTableSearch(&RoomTableData);
    dl_list_for_each(Temp,&RoomTableData.list,Room_Table_t,list)
    {
        if(RoomId != Temp->GroupId)
        {
            break;
        }
        RoomId ++;  
    }
    RoomTableSearchFree(&RoomTableData);
    Temp = NULL;
    return RoomId;
}
/*******************************************************************************
** �� �� ��  : DBUnUsedProfileId
** ��������  : ����һ��δʹ�õĳ���Id
** �������  : ��
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
uint16_t DBUnUsedProfileId()
{
    uint16_t ProfileId = 1;
    Profile_Table_t ProfileTableData,*Temp = NULL;
    memset(&ProfileTableData, 0, sizeof(Profile_Table_t));
    ProfileTableSearch(&ProfileTableData);
    dl_list_for_each(Temp,&ProfileTableData.list,Profile_Table_t,list)
    {
        if(ProfileId != Temp->ProfileId)
        {
            break;
        }
        ProfileId ++;   
    }
    ProfileTableSearchFree(&ProfileTableData);
    Temp = NULL;
    return ProfileId;
}
/*******************************************************************************
** �� �� ��  : ProfileTableDataIsExist
** ��������  : ��ѯһ��Profile��¼�Ƿ����
** �������  : Profile_Table_t *ProfileData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus ProfileTableDataIsExist( Profile_Table_t *ProfileData )
{
    Profile_Table_t ProfileTableData,*Temp = NULL;
    memset(&ProfileTableData, 0, sizeof(Profile_Table_t));
    ProfileTableSearch(&ProfileTableData);
    dl_list_for_each(Temp,&ProfileTableData.list,Profile_Table_t,list)
    {
        if(ProfileData->ProfileId == Temp->ProfileId)
        {
            ProfileTableSearchFree(&ProfileTableData);
            Temp = NULL;
            return SQLITE_SUCCESS;
        }
    }
    ProfileTableSearchFree(&ProfileTableData);
    Temp = NULL;
    return SQLITE_FIND_ERR;
}
/*******************************************************************************
** �� �� ��  : DeviceTableDataIsExist
** ��������  : ��ѯһ���豸��¼�Ƿ����
** �������  : Device_Table_t *DeviceData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus DeviceTableDataIsExist( Device_Table_t *DeviceData )
{
    Device_Table_t DeviceTableData,*Temp = NULL;
    memset(&DeviceTableData, 0, sizeof(Device_Table_t));
    DeviceTableSearch(&DeviceTableData);
    dl_list_for_each(Temp,&DeviceTableData.list,Device_Table_t,list)
    {
        if( !strcmp(DeviceData->DeviceIpv6, Temp->DeviceIpv6) )
        {
            DeviceTableSearchFree(&DeviceTableData);
            Temp = NULL;
            return SQLITE_SUCCESS;
        }
    }
    DeviceTableSearchFree(&DeviceTableData);
    Temp = NULL;
    
    return SQLITE_FIND_ERR;
}
/*******************************************************************************
** �� �� ��  : SceneTableDataIsExist
** ��������  : ��ѯһ��Scene��¼�Ƿ����
** �������  : Scene_Table_t *SceneData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus SceneTableDataIsExist( Scene_Table_t *SceneData )
{
    Scene_Table_t SceneTableData,*Temp = NULL;
    memset(&SceneTableData, 0, sizeof(Scene_Table_t));
    SceneTableSearch(&SceneTableData);
    dl_list_for_each(Temp,&SceneTableData.list,Scene_Table_t,list)
    {
        if( SceneData->SceneId == Temp->SceneId )
        {
            SceneTableSearchFree(&SceneTableData);
            Temp = NULL;
            return SQLITE_SUCCESS;
        }
    }
    SceneTableSearchFree(&SceneTableData);
    Temp = NULL;
    
    return SQLITE_FIND_ERR;
}
/*******************************************************************************
** �� �� ��  : RoomTableDataIsExist
** ��������  : ��ѯһ��Room��¼�Ƿ����
** �������  : Room_Table_t *RoomData  
** �� �� ֵ  : 
       
** ��    ��  : 2014��11��12��
** ��    ��  : pct
*******************************************************************************/
t_SqliteStatus RoomTableDataIsExist( Room_Table_t *RoomData )
{
    Room_Table_t RoomTableData,*Temp = NULL;
    memset(&RoomTableData, 0, sizeof(Room_Table_t));
    RoomTableSearch(&RoomTableData);
    dl_list_for_each(Temp,&RoomTableData.list,Room_Table_t,list)
    {
        if( RoomData->GroupId == Temp->GroupId )
        {
            RoomTableSearchFree(&RoomTableData);
            Temp = NULL;
            return SQLITE_SUCCESS;
        }
    }
    RoomTableSearchFree(&RoomTableData);
    Temp = NULL;   
    return SQLITE_FIND_ERR;
}
