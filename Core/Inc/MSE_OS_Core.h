/*
 * MSE_OS_Core.h
 *
 *  Created on: 8 jul. 2022
 *      Author: Wels
 */

#ifndef INC_MSE_OS_CORE_H_
#define INC_MSE_OS_CORE_H_

/*
 * LIBRERIA DE STM32
 * */
#include "main.h"


/************************************************************************************
 *
 ***********************************************************************************/

#define STACK_SIZE 256
#define Os_IdleTask_ID 0xFF

// PRIORIDADES
#define MAX_PRIORITY			0
#define MIN_PRIORITY			3

#define COUNT_PRIORITY			(MIN_PRIORITY-MAX_PRIORITY)+1

#define OS_MAX_PRIORITY			4

#define TASK_NAME_SIZE			10


#define QUEUE_HEAP_SIZE			64

/*==================[definicion de datos para el OS]=================================*/

typedef enum
{
	Task_Ready,
	Task_Running,
	Task_Blocked,
	Task_Suspended
}task_state_t;

/*==================[definicion de estados posibles del OS]=================================*/

typedef enum
{
	Os_From_Reset,
	Os_Normal_Run,
	Os_Scheduling,
	Os_Error,
	Os_Running_from_IRQ
}os_state_t;

typedef enum
{
	Os_error_none,
	Os_error_exceeded,
	Os_error_notask,
	Os_error_invalid_state,
	Os_error_maxpriority_exceeded,
	Os_error_daly_from_IRQ
}os_error_t;

/*==================[definicion de estructura para cada tarea]=================================*/

typedef struct
{
	uint32_t stack[STACK_SIZE/4];
	uint32_t stackPointer;
	void *entryPoint;
	uint8_t id;
	task_state_t state;
	uint8_t priority;
	uint8_t taskID;
	uint32_t blockedTicks;
}task_handler_t;

extern uint32_t tarea1;
extern uint32_t tarea2;
extern uint32_t tarea3;

/************************************************************************************
 * 	Posiciones dentro del stack frame de los registros que conforman el stack frame
 ***********************************************************************************/
#define XPSR		1
#define PC_REG		2
#define LR			3
#define R12			4
#define R3			5
#define R2				6
#define R1				7
#define R0				8
#define LR_PREV			9
#define R4				10
#define R5				11
#define R6				12
#define R7				13
#define R8				14
#define R9				15
#define R10 			16
#define R11 			17

//----------------------------------------------------------------------------------

#define INIT_XPSR 	1 << 24				//xPSR.T = 1
#define EXEC_RETURN	0xFFFFFFF9			//retornar a modo thread con MSP, FPU no utilizad

//----------------------------------------------------------------------------------


/************************************************************************************
 * 						Definiciones varias
 ***********************************************************************************/
#define STACK_FRAME_SIZE		8
#define FULL_STACKING_SIZE 		17	//16 core registers + valor previo de LR

/*==================[definicion codigos de error de OS]=================================*/



/*==================[definicion de prototipos]=================================*/

void OS_InitTask(task_handler_t *tareaHandler, void* entryPoint, uint8_t prioridad );
void OS_Init(void);
int32_t os_getError(void);
task_handler_t* Os_GetTareaActual(void);
void os_CPU_YIELD(void);

void Os_Critical_Enter();
void Os_Critical_Exit();

os_state_t Os_GetControlState();

void Os_SetControlState(task_state_t newState);

void Os_SetSchedulingFromIRQ();

void Os_ClearSchedulingFromIRQ();

bool Os_IsSchedulingFromIRQ();

void Os_SetError(os_error_t err, void* caller);

#endif /* INC_MSE_OS_CORE_H_ */
