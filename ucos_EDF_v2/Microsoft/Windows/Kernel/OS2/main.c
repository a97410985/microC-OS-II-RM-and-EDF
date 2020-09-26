#include  <cpu.h>
#include  <lib_mem.h>
#include  <os.h>

#include  "app_cfg.h"
#include  "loader.h"
#include <stdio.h>


#define  APP_TASK_START_PRIO                               4u
#define  APP_TASK_START_STK_SIZE                         256u
#define  TASK_STK_SIZE     512                /* Size of each task's stacks (# of WORDs)       */

OS_STK TaskStk11[APP_TASK_START_STK_SIZE];
OS_STK TaskStk12[APP_TASK_START_STK_SIZE];
OS_STK TaskStk[8][TASK_STK_SIZE];

// haha_turtle 定義數字巨集
#define			 PERIOSIC_TASK_START_ID  20
#define			 PERIODIC_TASK_START_PRIO 20

INT32U task11Time[3];
INT32U task12Time[3];
INT32U taskTimeInfos[8][3];
INT8U NumberOfTasks;


void Task11(void* p_arg);
void Task12(void* p_arg);
void createTaskSet1();
void periodTask(void* p_arg);

int  main(void)
{
	CPU_IntInit();

	Mem_Init();                                                 /* Initialize Memory Managment Module                   */
	CPU_IntDis();                                               /* Disable all Interrupts                               */
	CPU_Init();                                                 /* Initialize the uC/CPU services                       */

	OSInit();                                                   /* Initialize uC/OS-II                                  */

	// 建立任務
	createTaskSet1();

	OSStart();                                                  /* Start multitasking (i.e. give control to uC/OS-II)   */

	while (DEF_ON) {                                            /* Should Never Get Here.                               */
		;
	}
	return 0;
}


static void createTaskSet1() {

	FILE* InputFile;
	FILE* testOutputFile;
	testOutputFile = fopen("Output.txt", "w");
	InputFile = fopen("Input.txt", "r");
	if (InputFile == NULL) {
		printf("can't not open file.\n");
	}

	fscanf(InputFile, "%d", &NumberOfTasks);
	fprintf(testOutputFile, "%d\n", NumberOfTasks);


	for (int i = 0; i < NumberOfTasks; i++)
	{
		int tempCompTime;
		int tempPeriod;
		fscanf(InputFile, "%d %d", &tempCompTime, &tempPeriod);
		fprintf(testOutputFile, "%d %d\n", tempCompTime, tempPeriod);
		taskTimeInfos[i][0] = tempCompTime;
		taskTimeInfos[i][1] = 0;
		taskTimeInfos[i][2] = tempPeriod;
	}
	fclose(testOutputFile);
	fclose(InputFile);

	int hasAssignedTask[8];
	for (int i = 0; i < NumberOfTasks; i++)
	{
		hasAssignedTask[i] = -1;
	}


	for (int i = 0; i < NumberOfTasks; i++)
	{
		int minDeadline = 100000000;
		int minDeadlineTaskIndex = 0;
		for (int j = 0; j < NumberOfTasks; j++)
		{
			if (taskTimeInfos[i][2] < minDeadline) {
				int hasIn = 0;
				for (int k = 0; k <= i; k++)
				{
					if (hasAssignedTask[k] == j) {
						hasIn = 1;
					}
				}
				if (hasIn == 0) {
					minDeadline = taskTimeInfos[j][2];
					minDeadlineTaskIndex = j;
					hasAssignedTask[i] = j;
				}
			}
		}
		OSTaskCreateExt((void(*)(void*))periodTask,              /* Create the start task                                */
			(void*)0,
			(OS_STK*)&TaskStk[minDeadlineTaskIndex][TASK_STK_SIZE - 1],
			(INT8U)PERIODIC_TASK_START_PRIO+i,
			(INT16U)PERIOSIC_TASK_START_ID+minDeadlineTaskIndex,
			(OS_STK*)&TaskStk[minDeadlineTaskIndex][0],
			(INT32U)TASK_STK_SIZE,
			(void*)taskTimeInfos[minDeadlineTaskIndex],
			(INT16U)(OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR));
	}
}

void periodTask(void* p_arg) {
	int toDelay;
	FILE* testOutputFile;

	int T = taskTimeInfos[OSTCBCur->OSTCBId - PERIOSIC_TASK_START_ID][2] - taskTimeInfos[OSTCBCur->OSTCBId - PERIOSIC_TASK_START_ID][1]; // 週期
	while (1) {
		printf("task%d     start:%d  deadline:%d\n", OSTCBCur->OSTCBId, OSTimeGet(), OSTCBCur->Deadline);
		//testOutputFile = fopen("Output.txt", "a");
		//fprintf("task%d     start:%d  deadline:%d\n", OSTCBCur->OSTCBId, OSTimeGet(), OSTCBCur->Deadline);
		//fclose(testOutputFile);

		while (OSTCBCur->CompTime > 0) {

		}
		toDelay = taskTimeInfos[OSTCBCur->OSTCBId-PERIOSIC_TASK_START_ID][2] - OSTimeGet(); // task11Time[2] is deadline
		taskTimeInfos[OSTCBCur->OSTCBId - PERIOSIC_TASK_START_ID][1] = taskTimeInfos[OSTCBCur->OSTCBId - PERIOSIC_TASK_START_ID][2]; // task11Time[1] is start time ; 開始時間變成之前的deadline
		taskTimeInfos[OSTCBCur->OSTCBId - PERIOSIC_TASK_START_ID][2] = taskTimeInfos[OSTCBCur->OSTCBId - PERIOSIC_TASK_START_ID][1] + T;
		OSTCBCur->CompTime = taskTimeInfos[OSTCBCur->OSTCBId - PERIOSIC_TASK_START_ID][0];
		OSTCBCur->StartTime = taskTimeInfos[OSTCBCur->OSTCBId - PERIOSIC_TASK_START_ID][1];
		OSTCBCur->Deadline = taskTimeInfos[OSTCBCur->OSTCBId - PERIOSIC_TASK_START_ID][2];

		printf("task%d     end:%d  deadline:%d\n", OSTCBCur->OSTCBId, OSTimeGet(), OSTCBCur->Deadline);
		//testOutputFile = fopen("output.txt", "a");
		//fprintf("task%d     end:%d  deadline:%d\n", OSTCBCur->OSTCBId, OSTimeGet(), OSTCBCur->Deadline);
		//fclose(testOutputFile);
		OSTimeDly(toDelay);
		printf("task%d     end:%d  deadline:%d\n", OSTCBCur->OSTCBId, OSTimeGet(), OSTCBCur->Deadline);
		//testOutputFile = fopen("output.txt", "a");
		//fprintf("task%d     ready:%d  deadline:%d\n", OSTCBCur->OSTCBId, OSTimeGet(), OSTCBCur->Deadline);
		//fclose(testOutputFile);

	}
}
