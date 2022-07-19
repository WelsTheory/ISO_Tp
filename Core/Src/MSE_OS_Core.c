/*
 * MSE_OS_Core.c
 *
 *  Created on: 8 jul. 2022
 *      Author: Wels
 */
#include "MSE_OS_Core.h"

#define TASK_MAX_ALLOWED		8

/********************************************************************************
 * Definicion de la estructura para cada tarea
 *******************************************************************************/
typedef enum
{
	Os_error_none,
	Os_error_exceeded,
	Os_error_notask
}os_error_t;

typedef enum
{
	Os_From_Reset,
	Os_From_Running,
	Os_From_Error
} os_state_t;

typedef struct
{
	task_handler_t *task[TASK_MAX_ALLOWED];
	uint8_t actualTaskIndex;
	uint8_t tasksAdded;
	task_handler_t * actualTask;
	task_handler_t * nextTask;
	os_error_t error;
	os_state_t state;
} os_control_t;

static os_control_t os_control;

/*************************************************************************************************
 *  @brief Inicializa las tareas que correran en el OS.
 *
 *  @details
 *   Inicializa una tarea para que pueda correr en el OS implementado.
 *   Es necesario llamar a esta funcion para cada tarea antes que inicie
 *   el OS.
 *
 *  @param *tarea			Puntero a la tarea que se desea inicializar.
 *  @param *stack			Puntero al espacio reservado como stack para la tarea.
 *  @param *stack_pointer   Puntero a la variable que almacena el stack pointer de la tarea.
 *  @return     None.
 ***************************************************************************************************/
void OS_InitTask(task_handler_t *taskHandler, void* entryPoint)
{
	if (os_control.tasksAdded < TASK_MAX_ALLOWED)
	{
		taskHandler->stack[STACK_SIZE/4 - XPSR] = INIT_XPSR;								//necesari para bit thumb
		taskHandler->stack[STACK_SIZE/4 - PC_REG] = (uint32_t)entryPoint;
		taskHandler->stack[STACK_SIZE/4 - LR_PREV] = EXEC_RETURN;
		taskHandler->entryPoint = entryPoint;

		taskHandler->stackPointer = (uint32_t) (taskHandler->stack + STACK_SIZE/4 - FULL_STACKING_SIZE);

		taskHandler->state = Task_State_Ready;

		taskHandler->taskID = os_control.tasksAdded;

		os_control.task[os_control.tasksAdded] = taskHandler;
		os_control.tasksAdded++;
	}
	else
	{
		os_control.error = Os_error_exceeded;
	}
}

void OS_Init(void)  {
	uint8_t i;

	/*
	 * Todas las interrupciones tienen prioridad 0 (la maxima) al iniciar la ejecucion. Para que
	 * no se de la condicion de fault mencionada en la teoria, debemos bajar su prioridad en el
	 * NVIC. La cuenta matematica que se observa da la probabilidad mas baja posible.
	 */
	NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS)-1);

	os_control.tasksAdded = 0;

	os_control.actualTask = NULL;
	os_control.nextTask = NULL;

	os_control.error = Os_error_none;
	os_control.state = Os_From_Reset;

	for (i = 0; i<TASK_MAX_ALLOWED; i++)
	{
		os_control.task[i] = NULL;
	}
}

static void os_schedule(void)
{
	if (Os_From_Reset == os_control.state)
	{
		if (0 == os_control.tasksAdded)
		{
			os_control.state = Os_From_Error;
			os_control.error = Os_error_notask;

		}
		else
		{
			/*seleccionar la primer tarea para que sea ejecutada*/
			os_control.actualTaskIndex = 0;
			os_control.actualTask = os_control.task[os_control.actualTaskIndex];
		}
	}
	else
	{
		os_control.actualTaskIndex++;
		if (os_control.actualTaskIndex >= os_control.tasksAdded)
		{
			os_control.actualTaskIndex = 0;
		}

		os_control.nextTask = os_control.task[os_control.actualTaskIndex];
	}
}

void SysTick_Handler(void)  {

	os_schedule();

	__DSB();
}

uint32_t getContextoSiguiente(uint32_t p_stack_actual)
{
	uint32_t p_stack_siguiente = p_stack_actual; /* por defecto continuo con la tarea actual*/

	if (Os_From_Reset == os_control.state)
	{
		p_stack_siguiente = os_control.actualTask->stackPointer;
		os_control.actualTask->state = Task_State_Running;
		os_control.state = Os_From_Running;
	}
	else
	{
		os_control.actualTask->stackPointer = p_stack_actual;
		os_control.actualTask->state = Task_State_Ready;

		p_stack_siguiente = os_control.nextTask->stackPointer;

		os_control.actualTask = os_control.nextTask;
		os_control.actualTask->state = Os_From_Running;
	}

	return(p_stack_siguiente);
}
