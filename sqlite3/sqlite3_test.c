#include <stdio.h>
#include <sqlite3.h>

#define UI_BLUE(x) 	    "\e[34;1m"x"\e[0m"
#define UI_GREEN(x) 	"\e[32;1m"x"\e[0m"
#define UI_YELLOW(x)    "\e[33;40;1m"x"\e[0m"
#define UI_RED(x)	    "\e[31;1m"x"\e[0m"
#define UI_BACK_BLUE(x) "\e[0;37;41m"x"\e[0m" //white font,red background
#define DBG_vPrintf(a,b,ARGS...) do {  if (a) {printf(UI_BLUE("[DBG_%d]") b, __LINE__, ## ARGS);} } while(0)
#define ERR_vPrintf(a,b,ARGS...) do {  if (a) {printf(UI_RED("[ERR_%d]") b, __LINE__, ## ARGS);} } while(0)
#define LOG_vPrintf(a,b,ARGS...) do {  if (a) {printf(UI_GREEN("[LOG_%d]") b, __LINE__, ## ARGS);} } while(0)
#define INF_vPrintf(a,b,ARGS...) do {  if (a) {printf(UI_YELLOW("[INF_%d]") b, __LINE__, ## ARGS);} } while(0)
#define LED_vPrintf(a,b,ARGS...) do {  if (a) {printf(UI_BACK_BLUE("[RED_%d]") b, __LINE__, ## ARGS);} } while(0)

#define DBG_SQLITE	1
	
/**************************************************************************************************************/
typedef enum
{
	E_SQ_OK,
	E_SQ_ERROR,
	E_SQ_NO_DADA,
	E_SQ_OPEN_ERROR,
}teSqliteStatus;

/**************************************************************************************************************/

sqlite3 *psZigbeeNodesDB;

/**************************************************************************************************************/
teSqliteStatus SqliteDBOpen(char *pSqlite_DB);


/**************************************************************************************************************/
int main(int argc, char *argv[])
{
	printf("This is a sqlite3 test program\n");
	
	SqliteDBOpen("ZigbeeDB");
	return 0;
}

teSqliteStatus SqliteDBOpen(char *pSqlite_DB)
{
	if (!access(pSqlite_DB, 0))//Datebase does not existed
    {
        DBG_vPrintf(DBG_SQLITE, "Open %s \n",pSqlite_DB);
        if( SQLITE_OK != sqlite3_open( pSqlite_DB, &psZigbeeNodesDB) )
        {
            ERR_vPrintf(DBG_SQLITE, "Failed to Open SQLITE_DB \n");
            return E_SQ_OPEN_ERROR;
        }
    }
	else
	{
        DBG_vPrintf(DBG_SQLITE, "Create %s \n",pSqlite_DB);
		
	}

	return E_SQ_OK;
}
