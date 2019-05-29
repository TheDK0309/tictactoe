

#include "includes.h"

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  TASK_STK_SIZE                 512       /* Size of each task's stacks (# of WORDs)            */
#define  N_TASKS                        10       /* Number of identical tasks                          */

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

OS_STK        TaskStk[N_TASKS][TASK_STK_SIZE];        /* Tasks stacks                                  */
OS_STK        TaskStartStk[TASK_STK_SIZE];
char          TaskData[N_TASKS];                      /* Parameters to pass to each task               */
OS_EVENT	 *DispSem;




/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

void TaskStart(void *data);        /* Function prototype of Startup task           */
void draw(void *data); /*Function prototypes of the grid drawing*/
void playerA(int x, int y);
void playerB(int x, int y);
INT8U check(INT8U matrix[3][3]);



/*$PAGE*/
/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/

int  main (void)
{
    PC_DispClrScr(DISP_FGND_WHITE + DISP_BGND_BLACK);      /* Clear the screen                         */

    OSInit();                                              /* Initialize uC/OS-II                      */

	DispSem = OSSemCreate(1);
    
    OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
    OSStart();                                             /* Start multitasking                       */
    return 0;
}


/*
*********************************************************************************************************
*                                              STARTUP TASK
*********************************************************************************************************
*/
void  TaskStart (void *pdata)
{
    INT16S key;
	INT8U symbol;


    pdata = pdata;                                         /* Prevent compiler warning                 */


	OSTaskCreate(draw, (void *)&symbol, (void *)&TaskStk[0][TASK_STK_SIZE - 1], 1);

//    OSStatInit();                                          /* Initialize uC/OS-II's statistics         */
	//OSSemPend(DispSem, 0, &err);

    for (;;) {
  
        if (PC_GetKey(&key) == TRUE) {                     /* See if key has been pressed              */
            if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
                exit(0);  	                           /* End program                              */
            }
        }
        OSTimeDlyHMSM(0, 0, 1, 0);                         /* Wait one second                          */


    }
}

/*$PAGE*/

/*
*********************************************************************************************************
*                                                  FUNCTIONS
*********************************************************************************************************
*/


void draw(void *data)
{
	INT8U i;
	INT8U j;
	INT8U xa, ya;
	INT8U xb, yb;
	INT8U a, b;
	INT8U err;
	INT8U matrix[3][3];
	INT8U count = 0;

	
	for (a = 0; a<3; a++)
		for (b = 0; b<3; b++) matrix[a][b] = 0;

	for (i = 0; i < 6; i++) //draw the grid
	{
		PC_DispStr(33, 15+i, "|", DISP_FGND_WHITE);
		PC_DispStr(37, 15+i, "|", DISP_FGND_WHITE);
		if (i % 2 == 1 && i >0 && i<5) {
		PC_DispStr(30, 15 + i, "_", DISP_FGND_WHITE);
		PC_DispStr(31, 15 + i, "_", DISP_FGND_WHITE);
		PC_DispStr(32, 15 + i, "_", DISP_FGND_WHITE);
		PC_DispStr(34, 15 + i, "_", DISP_FGND_WHITE);
		PC_DispStr(35, 15 + i, "_", DISP_FGND_WHITE);
		PC_DispStr(36, 15 + i, "_", DISP_FGND_WHITE);
		PC_DispStr(38, 15 + i, "_", DISP_FGND_WHITE);
		PC_DispStr(39, 15 + i, "_", DISP_FGND_WHITE);
		PC_DispStr(40, 15 + i, "_", DISP_FGND_WHITE);
		}
	}

	OSTimeDly(1);
	srand(time(NULL));

	for (j = 0; j < 5; j++) {
			xa = rand() % (2 + 1 - 0) + 0;
			ya = rand() % (2 + 1 - 0) + 0;
			while (matrix[xa][ya] != 0) {
				xa = rand() % (2 + 1 - 0) + 0;
				ya = rand() % (2 + 1 - 0) + 0;
					}
			matrix[xa][ya] = 1;
			playerA(xa, ya);
			count++;
			if (check(matrix) == 1) break;

			if (count < 9) {
				OSTimeDlyHMSM(0, 0, 1, 0);
				xb = rand() % (2 + 1 - 0) + 0;
				yb = rand() % (2 + 1 - 0) + 0;
				while (matrix[xb][yb] != 0) {
					xb = rand() % (2 + 1 - 0) + 0;
					yb = rand() % (2 + 1 - 0) + 0;
				}
			matrix[xb][yb] = 2;
			playerB(xb, yb);
			if (check(matrix) == 1) break;
			OSTimeDlyHMSM(0, 0, 1, 0);
			count++;
			}
	}

	if (check(matrix) == 0) {
		PC_DispStr(30, 23, "No player wins", DISP_FGND_WHITE);
	}

	for (;;)
	{
		OSSemPend(DispSem, 0, &err);
		OSSemPost(DispSem);
	}
	
}/*function code*/

void playerA(int x, int y)
{
	PC_DispStr(31 + x*4, 15 + y*2, "X", DISP_FGND_WHITE);
}

void playerB(int x, int y)
{
	PC_DispStr(31 + x * 4, 15 + y * 2, "O", DISP_FGND_WHITE);
}

INT8U check(INT8U matrix[3][3]) {
	INT8U report = 0; 
	
	INT8U i;
	INT8U res;

		for (i = 0; i < 3; i++)  // check rows 
			if (matrix[i][0] == matrix[i][1] && 
				matrix[i][0] != 0 &&
				matrix[i][0] == matrix[i][2])
			{
				res = matrix[i][0];
				report = 1;
				break;
			}

		for (i = 0; i < 3; i++)  // check columns
			if (matrix[0][i] == matrix[1][i] &&
				matrix[0][i] != 0 &&
				matrix[0][i] == matrix[2][i]) 
			{
				res = matrix[0][i];
				report = 1;
				break;
			}

		// check diagonals
		if (matrix[0][0] == matrix[1][1] &&
			matrix[0][0] != 0 &&
			matrix[1][1] == matrix[2][2]) 
		{
			res = matrix[0][0];
			report = 1;
		}

		if (matrix[0][2] == matrix[1][1] &&
			matrix[0][2] != 0 &&
			matrix[1][1] == matrix[2][0])
		{
			res = matrix[0][2];
			report = 1;
		}

		if (res == 1) {
			PC_DispStr(30, 23, "Player 1 wins", DISP_FGND_WHITE);
		}
		else if (res == 2) {
			PC_DispStr(30, 23, "Player 2 wins", DISP_FGND_WHITE);
		}
		
	return report;
}