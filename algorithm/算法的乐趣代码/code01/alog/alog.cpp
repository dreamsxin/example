// alog.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#if 0
#define SYNC_LOG_CNT             1000
#define SYNC_LOG_MEMOVER_CNT     50

typedef struct 
{
	INT32U logCnt;
	EPON_SYNC_LOG_DATA syncLogs[SYNC_LOG_CNT];
}EPON_SYNC_LOG;

EPON_SYNC_LOG s_EponSyncLog;

void Epon_Sync_Log_Add(EPON_SYNC_LOG_DATA*pLogData)
{
   INT32U i = 0;
   INT32U syncLogCnt = 0;
   
	syncLogCnt = s_EponSyncLog.logCnt;
	if(syncLogCnt>=SYNC_LOG_CNT)
	{
        /*缓冲区已满，向前移动50条记录，为新纪录腾出空间*/
		memmove(s_EponSyncLog.syncLogs,
                s_EponSyncLog.syncLogs + SYNC_LOG_MEMOVER_CNT,
                (SYNC_LOG_CNT-SYNC_LOG_MEMOVER_CNT) * sizeof(EPON_SYNC_LOG_DATA));
        /*清空新腾出来的空间*/
		memset(s_EponSyncLog.syncLogs + (SYNC_LOG_CNT - SYNC_LOG_MEMOVER_CNT),
               0, SYNC_LOG_MEMOVER_CNT * sizeof(EPON_SYNC_LOG_DATA));
        /*写入当前一条日志*/
		memmove(s_EponSyncLog.syncLogs + (SYNC_LOG_CNT - SYNC_LOG_MEMOVER_CNT),
                pLogData, sizeof(EPON_SYNC_LOG_DATA));
        s_EponSyncLog.logCnt = SYNC_LOG_CNT - SYNC_LOG_MEMOVER_CNT + 1;

		return;
	}
	/*如果缓冲区有空间，则直接写入当前一条记录*/
    memmove(s_EponSyncLog.syncLogs + syncLogCnt, pLogData, sizeof(EPON_SYNC_LOG_DATA));
	s_EponSyncLog.logCnt++;
}
#endif

#define MAX_DIRTY_POINT   4
#define MAX_BMP_WIDTH      1600
#define MAX_BMP_HEIGHT     1200

typedef struct tagPOINT
{
    int x,y;
}POINT;

typedef struct tagRESULT
{
    POINT pts[MAX_DIRTY_POINT];/*记录搜索过的前4个点的位置*/
    int count;
}RESULT;

/*8个方向*/
POINT dir[] = { {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1} };

void SearchDirty(char bmp[MAX_BMP_WIDTH][MAX_BMP_HEIGHT], 
                 int x, int y, RESULT *result)
{
    for(int i = 0; i < sizeof(dir)/sizeof(dir[0]); i++)
    {
        int nx = x + dir[i].x;
        int ny = y + dir[i].y;
        if( (nx >= 0 && nx < MAX_BMP_WIDTH) 
            && (ny >= 0 && nx < MAX_BMP_HEIGHT) 
            && (bmp[nx][ny] == 1) )
        {
            if(result->count < MAX_DIRTY_POINT)
            {
                /*记录前MAX_DIRTY_POINT个点的位置*/
                result->pts[result->count].x = nx;
                result->pts[result->count].x = ny;
            }
            result->count++;
            if(result->count > MAX_DIRTY_POINT)
                break;

            SearchDirty(bmp, nx, ny, result);
        }
    }
}

int main(int argc, char* argv[])
{
	return 0;
}

