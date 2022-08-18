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

#define Os_IdleTask_ID 0xFF

#define TASK_NAME_SIZE			10
#define TASK_MAX_ALLOWED		8

// PRIORIDADES
#define MAX_PRIORITY			0
#define MIN_PRIORITY			3

#define COUNT_PRIORITY			(MIN_PRIORITY-MAX_PRIORITY)+1
/*==================[definicion codigos de error de OS]=================================*/

#define OS_ERROR_TAREAS			-1
#define OS_ERROR_SCHEDULING		-2

/*==================[definicion de datos para el OS]=================================*/

enum task_state_t
{
	Task_Ready,
	Task_Running,
	Task_Blocked//,
	//Task_Suspended

} ;

typedef enum task_state_t estadoTarea;

/*==================[definicion de estados posibles del OS]=================================*/

enum os_state_t
{
	Os_From_Reset,
	Os_Normal_Run,
	Os_Scheduling
};

typedef enum os_state_t estadoOS;

enum os_error_t
{
	Os_error_none,
	Os_error_exceeded,
	Os_error_notask,
	Os_error_invalid_state
};

typedef enum os_error_t errorOS;

/*==================[definicion de estructura para cada tarea]=================================*/

struct task_handler_t
{
	uint32_t stack[STACK_SIZE/4];
	uint32_t stackPointer;
	void *entryPoint;
	uint8_t id;
	estadoTarea state;
	uint8_t priority;
	uint8_t taskID;
	uint32_t blockedTicks;
};

typedef struct task_handler_t tarea;
//typedef struct task_handler_t tareaIdle;
//task_handler_t os_idleTask;

/*==================[definicion de estructura para el SO]=================================*/

struct os_control_t
{
	void *listaTareas[TASK_MAX_ALLOWED];
	int32_t error;
	uint8_t cantidad_Tareas;
	uint8_t cantTareas_prioridad[COUNT_PRIORITY];

	estadoOS estado_sistema;
	bool cambioContextoNecesario;

	tarea * actualTask;
	tarea * nextTask;
};

typedef struct os_control_t os_control;



/*==================[definicion de prototipos]=================================*/

void OS_InitTask(void *entryPoint, tarea *task, uint8_t prioridad );
void OS_Init(void);
int32_t os_getError(void);
tarea* os_getTareaActual(void);
void os_CPU_YIELD(void);


#endif /* INC_MSE_OS_CORE_H_ */
