#include <os.h>
#include "luyouliang.h"
#include "usercode.h"

#define TASK_STK_SIZE   512
OS_STK	TaskStk[OS_MAX_TASKS][TASK_STK_SIZE];

int MenuLoader(void)
{
	int Experiment;

	printf("1.任務1\n");

	scanf_s("%d", &Experiment);

	OSInit();
	switch (Experiment)
	{
	case 1: //任務1
		OSTaskCreate(FirstTask, 0, &TaskStk[5][TASK_STK_SIZE - 1], 5);
		break;
	default:
		;
	}
	return 0;
}
