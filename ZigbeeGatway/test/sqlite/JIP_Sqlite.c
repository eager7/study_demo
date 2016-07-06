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
 函 数 名  : OpenSqliteDB
 功能描述  : 打开数据库
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 正确返回0，错误返回1
 修改历史      :
  1.日    期   : 2014年10月29日
    作    者   : pct
    修改内容   : 新生成函数

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
** 函 数 名  : DeviceTableAddRecord
** 功能描述  : 在设备列表中添加一个新的记录
** 输入参数  : Device_Table_t *DeviceData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus DeviceTableAddRecord( Device_Table_t *DeviceData)
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*错误码*/
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
** 函 数 名  : DeviceTableDelete
** 功能描述  : 从设备列表中删除一个设备
** 输入参数  : Device_Table_t *DeviceData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus DeviceTableDelete( Device_Table_t *DeviceData )
{
    char SqlCommand[1024] = {0};
 
    char *pErrMsg = NULL;/*错误码*/
    sprintf(SqlCommand,"Delete From Device_Table where deviceipv6='%s'", DeviceData->DeviceIpv6);

    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, SqlCommand, NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    
    return SQLITE_SUCCESS;
}
/*******************************************************************************
** 函 数 名  : DeviceTableUpdate
** 功能描述  : 更新设备列表，根据参数maskcode进行更新，maskcode通过宏的或运
               算得到，必须传入设备的默认名称作为索引
** 输入参数  : Device_Table_t *DeviceData  
             : uint8_t maskcode            
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus DeviceTableUpdate( Device_Table_t *DeviceData, uint8_t maskcode )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*错误码*/

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
** 函 数 名  : DeviceTableClear
** 功能描述  : 清空设备列表
** 输入参数  : 无
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus DeviceTableClear()
{
    char *pErrMsg = NULL;/*错误码*/

    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, "Delete From Device_Table", NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}

///////////////////////////////////////Scene Table Function////////////////////////////////////
/*******************************************************************************
** 函 数 名  : SceneTableAddRecord
** 功能描述  : 在场景的列表中增加记录
** 输入参数  : Scene_Table_t *SceneData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus SceneTableAddRecord( Scene_Table_t *SceneData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*错误码*/
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
** 函 数 名  : SceneTableDelete
** 功能描述  : 从场景列表中删除一个记录
** 输入参数  : Scene_Table_t *SceneData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus SceneTableDelete( Scene_Table_t *SceneData ) 
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*错误码*/
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
** 函 数 名  : SceneTableUpdate
** 功能描述  : 更新场景列表，根据maskcode进行更新，必须传入Scene id作为索引
** 输入参数  : Scene_Table_t *SceneData  
             : uint8_t maskcode          
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pc
*******************************************************************************/
t_SqliteStatus SceneTableUpdate( Scene_Table_t *SceneData, uint8_t maskcode )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*错误码*/

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
** 函 数 名  : SceneTableClear
** 功能描述  : 清空场景列表
** 输入参数  : 无
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus SceneTableClear()
{
    char *pErrMsg = NULL;/*错误码*/

    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, "Delete From Scene_Table", NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}
///////////////////////////////////////Scene Device Table Function//////////////////////////////////
/*******************************************************************************
** 函 数 名  : SceneDeviceTableAddRecord
** 功能描述  : 在Scene表中插入新的记录，将会返回主键值到结构体中
** 输入参数  : Scene_Device_Table_t *SceneDeviceData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus SceneDeviceTableAddRecord( Scene_Device_Table_t *SceneDeviceData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*错误码*/
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
** 函 数 名  : SceneDeviceTableDelete
** 功能描述  : 从Scene表中删除一个记录，根据Scene的Id进行删除
** 输入参数  : Scene_Device_Table_t *SceneDeviceData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus SceneDeviceTableDelete( Scene_Device_Table_t *SceneDeviceData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*错误码*/
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
** 函 数 名  : SceneDeviceTableClear
** 功能描述  : 清空Scene表
** 输入参数  : 无
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus SceneDeviceTableClear()
{
    char *pErrMsg = NULL;/*错误码*/

    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, "Delete From Scene_Device_Table", NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error resion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}


///////////////////////////////////////Profile Table Function//////////////////////////////////
/*******************************************************************************
** 函 数 名  : ProfileTableAddRecord
** 功能描述  : 在Profile列表中增加一个Record，将返回索引值到结构体
** 输入参数  : Profile_Table_t *ProfileData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pc
*******************************************************************************/
t_SqliteStatus ProfileTableAddRecord( Profile_Table_t *ProfileData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*错误码*/
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
** 函 数 名  : ProfileTableDelete
** 功能描述  : 在Profile列表中删除一个记录
** 输入参数  : Profile_Table_t *ProfileData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus ProfileTableDelete( Profile_Table_t *ProfileData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*错误码*/
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
** 函 数 名  : ProfileTableUpdate
** 功能描述  : 更新Profile列表，根据参数maskcode更新，必须传入Profile的id作
               为索引
** 输入参数  : Profile_Table_t *ProfileData  
             : uint8_t maskcode              
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus ProfileTableUpdate( Profile_Table_t *ProfileData, uint8_t maskcode )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*错误码*/
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
** 函 数 名  : ProfileTableClear
** 功能描述  : 清空Profile列表
** 输入参数  : 无
** 返 回 值  : 
       
** 日    期  : 2014年10月29日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus ProfileTableClear()
{
    char *pErrMsg = NULL;/*错误码*/

    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, "Delete From Profile_Table", NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}

///////////////////////////////////////Room Table Function/////////////////////////////////////
/*******************************************************************************
** 函 数 名  : RoomTableAddRecord
** 功能描述  : 在房间列表中增加一个记录，将返回主键索引到结构体
** 输入参数  : Room_Table_t *RoomData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus RoomTableAddRecord( Room_Table_t *RoomData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*错误码*/
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
    char *pErrMsg = NULL;/*错误码*/
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
    char *pErrMsg = NULL;/*错误码*/
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
    char *pErrMsg = NULL;/*错误码*/

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
** 函 数 名  : RoomTableDelete
** 功能描述  : 从房间列表中删除一个记录，根据房间ID进行删除
** 输入参数  : Room_Table_t *RoomData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pc
*******************************************************************************/
t_SqliteStatus RoomTableDelete( Room_Table_t *RoomData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*错误码*/
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
** 函 数 名  : RoomTableUpdate
** 功能描述  : 更新房间列表，根据掩码来更新，掩码通过宏运算而来，必须传入room
               的组id作为索引
** 输入参数  : Room_Table_t *RoomData  
             : uint8_t maskcode        
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus RoomTableUpdate( Room_Table_t *RoomData, uint8_t maskcode )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*错误码*/
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
** 函 数 名  : RoomTableClear
** 功能描述  : 清空房间列表
** 输入参数  : 无
** 返 回 值  : 
       
** 日    期  : 2014年10月29日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus RoomTableClear()
{
    char *pErrMsg = NULL;/*错误码*/

    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, "Delete From Room_Table", NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}


///////////////////////////////////////Profile List Table Function/////////////////////////////
/*******************************************************************************
** 函 数 名  : ProfileListTableAddRecord
** 功能描述  : 为ProfileList表增加一个记录，增加前需要Profile和Scene存在，如
               果不知道Profile和Scene的索引值，需使用函数SceneTableSearchOne
               以及ProfileTableSearchOne更新结构体，找到索引，然后将两个结构
               体赋值给Profile_List_Table_t类型的结构体，再调用此函数添加
** 输入参数  : Profile_List_Table_t *ProfileListData  
** 返 回 值  : 
       
** 日    期  : 2014年10月30日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus ProfileListTableAddRecord( Profile_List_Table_t *ProfileListData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*错误码*/
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
** 函 数 名  : ProfileListTableDelete
** 功能描述  : 删除一个记录，根据Profile的id进行删除，所以必须创建一个Profile
               的结构体，并设置Profile id的值，然后将结构体赋值给ProfileList
               的结构体再调用此函数进行删除
** 输入参数  : Profile_List_Table_t *ProfileListData  
** 返 回 值  : 
       
** 日    期  : 2014年10月30日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus ProfileListTableDelete( Profile_List_Table_t *ProfileListData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*错误码*/
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
** 函 数 名  : ProfileListTableUpdate
** 功能描述  : 更新Profile列表，必须传入Profile的id作为索引，同时根据掩码进
               行更新
** 输入参数  : Profile_List_Table_t *ProfileListData  
             : uint8_t maskcode                      
** 返 回 值  : 
       
** 日    期  : 2014年10月29日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus ProfileListTableUpdate( Profile_List_Table_t *ProfileListData, uint8_t maskcode )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*错误码*/
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
    char *pErrMsg = NULL;/*错误码*/

    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, "Delete From Profile_List_Table", NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}


///////////////////////////////////////List Table Function/////////////////////////////////////
/*******************************************************************************
** 函 数 名  : RoomListTableAddRecord
** 功能描述  : 为房间设备列表增加记录，使用方法同ProfileListTableAddRecord
** 输入参数  : Room_List_Table_t *RoomListData  
** 返 回 值  : 
       
** 日    期  : 2014年10月30日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus RoomListTableAddRecord( Room_List_Table_t *RoomListData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*错误码*/
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
** 函 数 名  : RoomListTableDelete
** 功能描述  : 使用方法同ProfileList
** 输入参数  : Room_List_Table_t *RoomListData  
** 返 回 值  : 
       
** 日    期  : 2014年10月30日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus RoomListTableDelete( Room_List_Table_t *RoomListData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*错误码*/
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
** 函 数 名  : RoomListTableDeleteUnderDevice
** 功能描述  : 删除房间里的设备
** 输入参数  : Room_List_Table_t *RoomListData  
** 返 回 值  : 
       
** 日    期  : 2014年12月5日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus RoomListTableDeleteUnderDevice( Room_List_Table_t *RoomListData )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*错误码*/
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
** 函 数 名  : RoomListTableUpdate
** 功能描述  : 更新房间设备列表，即更新设备所在的组
** 输入参数  : Room_List_Table_t *RoomListData  
             : uint8_t maskcode                 
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus RoomListTableUpdate( Room_List_Table_t *RoomListData, uint8_t maskcode )
{
    char SqlCommand[1024] = {0};
    char *pErrMsg = NULL;/*错误码*/
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
    char *pErrMsg = NULL;/*错误码*/

    if(SQLITE_OK != sqlite3_exec(SQLITE_DB, "Delete From Room_List_Table", NULL, NULL, &pErrMsg))
    {
        PCT_vPrintf(DBG_SQLITE, "Error reasion is %s\n", pErrMsg);
        return SQLITE_ERR;
    }
    return SQLITE_SUCCESS;
}
/////////////////////////////////////查询/////////////////////////////////////////////
/*******************************************************************************
** 函 数 名  : SceneTableSearch
** 功能描述  : 查询场景的记录，用于时间管理，调用时传入一个结构体，将返回一
               个链表。查询时按照增序查询，方便添加新的场景
** 输入参数  : Scene_Table_t *SceneData  
** 返 回 值  : 
       
** 日    期  : 2014年10月29日
** 作    者  : pct
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
    dl_list_init ( &SceneData->list ); /*初始化链表*/
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
** 函 数 名  : SceneTableSearchFree
** 功能描述  : 释放查询时分配的内存
** 输入参数  : Scene_Table_t *SceneData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
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
** 函 数 名  : DeviceTableSearch
** 功能描述  : 输入设备结构体的指针，然后使用指针中的链表遍历所有设备
** 输入参数  : Device_Table_t *DeviceData  
** 返 回 值  : 
       
** 日    期  : 2014年10月30日
** 作    者  : pct
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
    dl_list_init ( &DeviceData->list ); /*初始化链表*/
    
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
** 函 数 名  : DeviceTableSearchFree
** 功能描述  : 释放查询时分配的内存
** 输入参数  : Device_Table_t *DeviceData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
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
** 函 数 名  : RoomTableSearch
** 功能描述  : 查询房间列表，即有多少个房间以及房间的具体信息
** 输入参数  : Room_Table_t *RoomData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
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
    dl_list_init ( &RoomData->list ); /*初始化链表*/
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
** 函 数 名  : RoomTableSearchFree
** 功能描述  : 释放查询房间时分配的内存空间
** 输入参数  : Room_Table_t *RoomData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
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
** 函 数 名  : ProfileTableSearch
** 功能描述  : 查询Profile个数以及详细信息
** 输入参数  : Profile_Table_t *ProfileData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
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
    dl_list_init ( &ProfileData->list ); /*初始化链表*/
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
** 函 数 名  : ProfileTableOnOff
** 功能描述  : 设置Profile的ONOFF属性，并设置和Profile相关的Scened的ONOFF属
               性
** 输入参数  : Profile_Table_t *ProfileData  
             : uint8_t onoff                 
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
*******************************************************************************/
t_SqliteStatus  ProfileTableOnOff( Profile_Table_t *ProfileData, uint8_t onoff)
{
    char SqlCommand[1024] = {0};
    sqlite3_stmt * stmtProfile = NULL; 
    char *pErrMsg = NULL;/*错误码*/
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
** 函 数 名  : ProfileTableSearchFree
** 功能描述  : 释放查询Profile时分配的内存空间
** 输入参数  : Profile_Table_t *ProfileData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
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
** 函 数 名  : DeviceTableSearchOne
** 功能描述  : 在只知道设备IPV6地址的情况下，获取设备的其他信息
** 输入参数  : Device_Table_t *DeviceData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
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
** 函 数 名  : SceneTableSearchOne
** 功能描述  : 在只知道场景id的情况下获取当前id场景的所有信息
** 输入参数  : Scene_Table_t *SceneData  
** 返 回 值  : 
       
** 日    期  : 2014年10月30日
** 作    者  : pct
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
** 函 数 名  : RoomTableSearchOne
** 功能描述  : 在只知道房间id的情况下获取房间所有信息
** 输入参数  : Room_Table_t *RoomData  
** 返 回 值  : 
       
** 日    期  : 2014年10月30日
** 作    者  : pct
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
** 函 数 名  : ProfileTableSearchOne
** 功能描述  : 在只知道profile id的情况下获取其他信息
** 输入参数  : Profile_Table_t *ProfileData  
** 返 回 值  : 
       
** 日    期  : 2014年10月30日
** 作    者  : pct
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
** 函 数 名  : ProfileTableSearchOneFree
** 功能描述  : 释放Profile查询一个记录时分配的设备字符串空间
** 输入参数  : Profile_Table_t *ProfileData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
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
** 函 数 名  : ProfileDeviceListSearch
** 功能描述  : 查询基于Profile的设备，传入需要查询的Profile ID，以及一个结构
               体，通过结构体链表获取查询数据
** 输入参数  : uint16_t ProfileId          
             : Device_Table_t *DeviceData  
** 返 回 值  : 
       
** 日    期  : 2014年10月29日
** 作    者  : pct
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
    dl_list_init ( &DeviceData->list ); /*初始化链表*/

    sprintf(SqlCommand,"Select * From Profile_Table where profileid=%d", ProfileId);
    PCT_vPrintf(DBG_SQLITE, "SqlCommand--->%s\n", SqlCommand);
    
    if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, SqlCommand, -1, &stmtprofile, NULL))
    {
        PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
        return SQLITE_FIND_ERR;
    }
    else
    {
        while(sqlite3_step(stmtprofile) == SQLITE_ROW)//从Profile表中查出Profile Id
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
                while(sqlite3_step(stmtprofilelist) == SQLITE_ROW)//从Profile_List中查出Scene Index
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
                        while(sqlite3_step(stmtscene) == SQLITE_ROW)//从Scene Device表中查出Device Index
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
** 函 数 名  : ProfileSceneListSearch
** 功能描述  : 根据Profile的ID查询和此Profile相关的Scene
** 输入参数  : uint16_t ProfileId        
             : Scene_Table_t *SceneData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
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
    
    dl_list_init ( &SceneData->list ); /*初始化链表*/
    if(SQLITE_OK != sqlite3_prepare_v2(SQLITE_DB, SqlCommand, -1, &stmtprofile, NULL))
    {
        PCT_vPrintf(DBG_SQLITE,"Error----%s",sqlite3_errmsg(SQLITE_DB));
        return SQLITE_FIND_ERR;
    }
    else
    {
        while(sqlite3_step(stmtprofile) == SQLITE_ROW)//从Profile List表中查出Profile Id
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
                while(sqlite3_step(stmtscene) == SQLITE_ROW)//从Scene表中查出Scene
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
** 函 数 名  : RoomDeviceListSearch
** 功能描述  : 根据房间查找设备列表，roomid即房间的组ID，函数查找完成返回结
               构体链表
** 输入参数  : uint16_t RoomId             
             : Device_Table_t *DeviceData  
** 返 回 值  : 
       
** 日    期  : 2014年10月30日
** 作    者  : pc
*******************************************************************************/
t_SqliteStatus RoomDeviceListSearch( uint32_t RoomId, Device_Table_t *DeviceData )
{
    char SqlCommand[1024] = {0};
    sqlite3_stmt * room = NULL; 
    sqlite3_stmt * device = NULL; 
    sqlite3_stmt * roomlist = NULL; 
    Device_Table_t *Temp = NULL;
    dl_list_init ( &DeviceData->list ); /*初始化链表*/

    sprintf(SqlCommand,"Select * From Room_Table where groupid=%d", RoomId);
    PCT_vPrintf(DBG_SQLITE, "SqlCommand--->%s\n", SqlCommand);
    /*查找房间列表的ID*/
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
            /*查找房间设备列表的设备索引*/
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
                    /*根据设备索引查找设备信息*/
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
    dl_list_init ( &DeviceData->list ); /*初始化链表*/

    sprintf(SqlCommand,"Select * From Scene_Device_Table where sceneindex=%d", SceneId);
    PCT_vPrintf(DBG_SQLITE, "SqlCommand--->%s\n", SqlCommand);
    /*查找房间列表的ID*/
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
            /*查找房间设备列表的设备索引*/
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
** 函 数 名  : DBUnUsedSceneId
** 功能描述  : 查找一个未使用的SceneId
** 输入参数  : 无
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
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
** 函 数 名  : DBUnUsedRoomId
** 功能描述  : 返回一个未使用的Room Id
** 输入参数  : 无
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
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
** 函 数 名  : DBUnUsedProfileId
** 功能描述  : 返回一个未使用的场景Id
** 输入参数  : 无
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
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
** 函 数 名  : ProfileTableDataIsExist
** 功能描述  : 查询一个Profile记录是否存在
** 输入参数  : Profile_Table_t *ProfileData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
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
** 函 数 名  : DeviceTableDataIsExist
** 功能描述  : 查询一个设备记录是否存在
** 输入参数  : Device_Table_t *DeviceData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
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
** 函 数 名  : SceneTableDataIsExist
** 功能描述  : 查询一个Scene记录是否存在
** 输入参数  : Scene_Table_t *SceneData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
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
** 函 数 名  : RoomTableDataIsExist
** 功能描述  : 查询一个Room记录是否存在
** 输入参数  : Room_Table_t *RoomData  
** 返 回 值  : 
       
** 日    期  : 2014年11月12日
** 作    者  : pct
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
