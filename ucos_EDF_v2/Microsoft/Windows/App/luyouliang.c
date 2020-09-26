#include <os.h>
#include "luyouliang.h"

BOOLEAN g_FlagEn = 0;		//增加一個全域變數，作為是否要時鐘調度的標誌

OS_EVENT  *MyEventSem;

int a, b, c;
char * buffer;

int FirstTask(void *pParam)
{
	int i, j = 0;
	int abc = 123;
	OSTimeDly(100);//不影响统计任务初始化时获取最大的空闲计数值
	printf("welcome to embeded system\n");
	printf("welcome to ucos\n");
	printf("This is a simple task for experiment 1!\n");

	for (;;)
	{
		// for (i = 0; i<99999999; i++);
		OSTimeDly(100);
		printf("任務延時，j=%d\n", j++);
	}
	return(0);
}
