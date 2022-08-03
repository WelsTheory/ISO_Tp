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
 * 			Tamaño del stack predefinido para cada tarea expresado en bytes
 ***********************************************************************************/

#define STACK_SIZE 256
#define Os_IdleTask_ID 0xFF

typedef enum
{
	Task_State_Running,
	Task_State_Ready,
	Task_State_Blocked,
	Task_State_Suspended

} task_state_t;

typedef struct
{
	uint32_t stack[STACK_SIZE/4];
	uint32_t stackPointer;
	void *entryPoint;
	task_state_t state;
	uint8_t priority;
	uint8_t taskID;
	uint32_t blockedTicks;
} task_handler_t;

//----------------------------------------------------------------------------------

extern uint32_t sp_tarea1;
extern uint32_t sp_tarea2;
extern uint32_t sp_tarea3;

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
#define STACK_FRAME_SIZE	8
#define FULL_STACKING_SIZE 			17	//16 core registers + valor previo de LR


/*==================[definicion codigos de error de OS]=================================*/


/*==================[definicion de datos para el OS]=================================*/


/*==================[definicion de prototipos]=================================*/

void OS_InitTask(task_handler_t *task_handler, void* entryPoint);
void OS_Init(void);


#endif /* INC_MSE_OS_CORE_H_ */
