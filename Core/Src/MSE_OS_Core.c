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
	Os_error_notask,
	Os_error_invalid_state
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
	bool ContinueTask;
} os_control_t;

static os_control_t os_control;
static task_handler_t os_idleTask;

/******************************************************************************************
 * @brief Implementación de Tarea Idle y Hooks
 *
 ************************************************************************************************/
void __attribute__((weak)) returnHook(void){
	while(1);
}

void __attribute__((weak)) tickHook(void){
	__asm volatile( "nop" );
}

void __attribute__((weak)) taskIdleHook(void)  {
	__asm volatile( "nop" );
}

void __attribute__((weak)) errorHook(void *caller)  {
	/* el error del sistema operativo se encuentra en os_control.error
	 * TODO: agregar un método que devuelva el error*/
	while(1);
}

void __attribute__((weak)) idleTask(void)  {
	while(1)  {
		__WFI();
	}
}

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
		taskHandler->stack[STACK_SIZE/4 - LR] = (uint32_t)returnHook;
		/**
		 * El valor previo de LR (que es EXEC_RETURN en este caso) es necesario dado que
		 * en esta implementacion, se llama a una funcion desde dentro del handler de PendSV
		 * con lo que el valor de LR se modifica por la direccion de retorno para cuando
		 * se termina de ejecutar getContextoSiguiente
		 */
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
		errorHook(OS_InitTask);
	}
}


/*************************************************************************************************
	 *  @brief Inicializacion de la tarea idle.
     *
     *  @details
     *   Esta funcion es una version reducida de os_initTarea para la tarea idle. Como esta tarea
     *   debe estar siempre presente y el usuario no la inicializa, los argumentos desaparecen
     *   y se toman estructura y entryPoint fijos. Tampoco se contabiliza entre las tareas
     *   disponibles (no se actualiza el contador de cantidad de tareas). El id de esta tarea
     *   se establece como 255 (0xFF) para indicar que es una tarea especial.
     *
	 *  @param 		None.
	 *  @return     None
	 *  @see os_InitTarea
***************************************************************************************************/
void initIdleTask()
{
	os_idleTask.stack[STACK_SIZE/4 - XPSR] = INIT_XPSR;						//necesario para bit thumb
	os_idleTask.stack[STACK_SIZE/4 - PC_REG] = (uint32_t)idleTask;		//direccion de la tarea (ENTRY_POINT)
	os_idleTask.stack[STACK_SIZE/4 - LR] = (uint32_t)returnHook;			//Retorno en la rutina de la tarea. Esto no está permitido
	/**
	 * El valor previo de LR (que es EXEC_RETURN en este caso) es necesario dado que
	 * en esta implementacion, se llama a una funcion desde dentro del handler de PendSV
	 * con lo que el valor de LR se modifica por la direccion de retorno para cuando
	 * se termina de ejecutar getContextoSiguiente
	 */
	os_idleTask.stack[STACK_SIZE/4 - LR_PREV] = EXEC_RETURN;

	os_idleTask.entryPoint = idleTask;

	os_idleTask.stackPointer = (uint32_t) (os_idleTask.stack + STACK_SIZE/4 - FULL_STACKING_SIZE);

	os_idleTask.state = Task_State_Ready;

	os_idleTask.taskID = Os_IdleTask_ID;
}


void OS_Init(void)  {
	uint8_t i;

	/*
	 * Todas las interrupciones tienen prioridad 0 (la maxima) al iniciar la ejecucion. Para que
	 * no se de la condicion de fault mencionada en la teoria, debemos bajar su prioridad en el
	 * NVIC. La cuenta matematica que se observa da la probabilidad mas baja posible.
	 */
	NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS)-1);

	initIdleTask();

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
	uint8_t id;
	bool seekForTask, allBlocked;
	uint8_t blockedTasksCounter = 0;

	os_control.ContinueTask = false;

	if (Os_From_Reset == os_control.state)
	{
		if (0 == os_control.tasksAdded)
		{
			os_control.state = Os_From_Error;
			os_control.error = Os_error_notask;
			errorHook(os_schedule);
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
		id = os_control.actualTaskIndex;
		seekForTask = true;
		allBlocked = false;
		while(seekForTask)
		{
			id++;
			if (id >= os_control.tasksAdded)
			{
				id = 0;
			}
			switch (os_control.task[id]->state)
			{
			case Task_State_Ready:
				seekForTask = false;
				break;
			case Task_State_Blocked:
				blockedTasksCounter++;
				if (blockedTasksCounter >= os_control.tasksAdded)
				{
					/*all tasks blocked*/
					seekForTask = false;
					allBlocked = true;
				}
				break;
			case Task_State_Running:
				/*all tasks are blocked except the one that was running*/
				seekForTask = false;
				break;
			case Task_State_Suspended:
				/*Nothing to do now*/
				break;
			default:
				os_control.state = Os_From_Error;
				os_control.error = Os_error_invalid_state;
				seekForTask = false;
				errorHook(os_schedule);
			}
		}
		if (allBlocked)
		{
			/*No one task ready, run Idle Task */
			os_control.nextTask = &os_idleTask;
		}
		else if (id != os_control.actualTaskIndex)
		{
			os_control.actualTaskIndex = id;
			os_control.nextTask = os_control.task[os_control.actualTaskIndex];
		}
		else
		{
			/*only actual task is ready to continue*/
			os_control.ContinueTask = true;
		}
	}
}

void SysTick_Handler(void)  {

	os_schedule();
	/* tickHook*/
	tickHook();
	/**
	 * Se setea el bit correspondiente a la excepcion PendSV
	 */
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;

	__ISB();
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
		if(!os_control.ContinueTask)
		{
			os_control.actualTask->stackPointer = p_stack_actual;
			if(Task_State_Running == os_control.actualTask->state)
			{
				os_control.actualTask->state = Task_State_Ready;
			}

			p_stack_siguiente = os_control.nextTask->stackPointer;

			os_control.actualTask = os_control.nextTask;
			os_control.actualTask->state = Task_State_Running;
		}
	}
	return(p_stack_siguiente);
}
