/*
 * MSE_OS_Core.c
 *
 *  Created on: 8 jul. 2022
 *      Author: Wels
 */
#include "MSE_OS_Core.h"

#define TASK_MAX_ALLOWED		8

/*==================[definicion de estructura para el SO]=================================*/

typedef struct
{
	task_handler_t *tarea[TASK_MAX_ALLOWED];
	uint8_t ActualTareaId;
	uint8_t NumTarea;

} os_schedule_elem_t;

typedef struct
{
	os_schedule_elem_t TareaPorPrioridad[OS_MAX_PRIORITY+1];

} os_schedule_t;

typedef struct
{
	os_schedule_t schedule;
	task_handler_t *listaTareas[TASK_MAX_ALLOWED];
	uint8_t TareaActual;
	uint8_t TareaAgregad;
	task_handler_t * actualTarea;
	task_handler_t * nextTarea;
	os_error_t error;
	os_state_t state;
	bool cambioContextoNecesario;
	int16_t TareaEnZonaCritica;
	bool SchedulingFromIRQ;
}os_control_t;

static os_control_t Os_Control;
static task_handler_t tareaIdle;

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


/********************************************************************************
 * Definicion de prototipos static
 *******************************************************************************/
static void setPendSV();
static void os_scheduler();

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
void OS_InitTask(task_handler_t *tareaHandler, void* entryPoint, uint8_t prioridad  )
{
	if(Os_Control.TareaAgregad >= TASK_MAX_ALLOWED)
	{
		Os_Control.error = Os_error_exceeded;
		errorHook(OS_InitTask);
	}
	else if(prioridad > OS_MAX_PRIORITY)
	{
		Os_Control.error = Os_error_maxpriority_exceeded;
	}
	else{
		tareaHandler->stack[STACK_SIZE/4 - XPSR] = INIT_XPSR;								//necesari para bit thumb
		tareaHandler->stack[STACK_SIZE/4 - PC_REG] = (uint32_t)entryPoint;
		tareaHandler->stack[STACK_SIZE/4 - LR] = (uint32_t)returnHook;
		/**
		 * El valor previo de LR (que es EXEC_RETURN en este caso) es necesario dado que
		 * en esta implementacion, se llama a una funcion desde dentro del handler de PendSV
		 * con lo que el valor de LR se modifica por la direccion de retorno para cuando
		 * se termina de ejecutar getContextoSiguiente
		 */
		tareaHandler->stack[STACK_SIZE/4 - LR_PREV] = EXEC_RETURN;

		tareaHandler->stackPointer = (uint32_t) (tareaHandler->stack + STACK_SIZE/4 - FULL_STACKING_SIZE);

		tareaHandler->entryPoint = entryPoint;

		tareaHandler->id = Os_Control.TareaAgregad;
		tareaHandler->state = Task_Ready;
		tareaHandler->priority = prioridad;

		Os_Control.schedule.TareaPorPrioridad[prioridad].tarea[
															   Os_Control.schedule.TareaPorPrioridad[prioridad].NumTarea] = tareaHandler;

		Os_Control.schedule.TareaPorPrioridad[prioridad].NumTarea++;

		Os_Control.listaTareas[Os_Control.TareaAgregad] = tareaHandler;
		Os_Control.TareaAgregad++;
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
static void initIdleTask(void)
{
	tareaIdle.stack[STACK_SIZE/4 - XPSR] = INIT_XPSR;						//necesario para bit thumb
	tareaIdle.stack[STACK_SIZE/4 - PC_REG] = (uint32_t)idleTask;		//direccion de la tarea (ENTRY_POINT)
	tareaIdle.stack[STACK_SIZE/4 - LR] = (uint32_t)returnHook;			//Retorno en la rutina de la tarea. Esto no está permitido
	/**
	 * El valor previo de LR (que es EXEC_RETURN en este caso) es necesario dado que
	 * en esta implementacion, se llama a una funcion desde dentro del handler de PendSV
	 * con lo que el valor de LR se modifica por la direccion de retorno para cuando
	 * se termina de ejecutar getContextoSiguiente
	 */
	tareaIdle.stack[STACK_SIZE/4 - LR_PREV] = EXEC_RETURN;
	tareaIdle.stackPointer = (uint32_t) (tareaIdle.stack + STACK_SIZE/4 - FULL_STACKING_SIZE);

	tareaIdle.entryPoint = idleTask;

	tareaIdle.id = Os_IdleTask_ID;
	tareaIdle.state = Task_Ready;

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

	/*
	 * Al iniciar el OS se especifica que se encuentra en la primer ejecucion desde un reset.
	 * Este estado es util para cuando se debe ejecutar el primer cambio de contexto. Los
	 * punteros de tarea_actual y tarea_siguiente solo pueden ser determinados por el scheduler
	 */
	Os_Control.actualTarea = NULL;
	Os_Control.nextTarea = NULL;

	Os_Control.error = Os_error_none;
	Os_Control.state = Os_From_Reset;

	/*
	 * El vector de tareas termina de inicializarse asignando NULL a las posiciones que estan
	 * luego de la ultima tarea. Esta situacion se da cuando se definen menos de 8 tareas.
	 * Estrictamente no existe necesidad de esto, solo es por seguridad.
	 */
	for (i = Os_Control.TareaAgregad; i<TASK_MAX_ALLOWED; i++)
	{
		Os_Control.listaTareas[i] = NULL;
	}

	Os_Control.TareaEnZonaCritica = 0;
	Os_SetSchedulingFromIRQ(false);
}

static task_handler_t * Os_SelectNextTask_ByPriority(uint8_t prioridad)
{
	uint8_t id;
	bool seekForTask, allBlocked;
	uint8_t blockedTasksCounter = 0;
	task_handler_t * taskSelected = NULL;

	id = Os_Control.schedule.TareaPorPrioridad[prioridad].ActualTareaId;
	/* Solo buscar una posible tarea a ejecutar, si al menos hay una tarea
	 * agregada en esta prioridad */
	if (0 < Os_Control.schedule.TareaPorPrioridad[prioridad].NumTarea)
	{
		seekForTask = true;
		allBlocked = false;
		while (seekForTask)
		{
			id++;
			if (id >= Os_Control.schedule.TareaPorPrioridad[prioridad].NumTarea)
			{
				id = 0;
			}
			switch (Os_Control.schedule.TareaPorPrioridad[prioridad].tarea[id]->state)
			{
			case Task_Ready:
				seekForTask = false;
				break;
			case Task_Blocked:
				blockedTasksCounter++;
				if (blockedTasksCounter >=
						Os_Control.schedule.TareaPorPrioridad[prioridad].NumTarea)
				{
					/*all tasks blocked*/
					seekForTask = false;
					allBlocked = true;
				}
				break;
			case Task_Running:
				/*all tasks are blocked except the one that was running*/
				seekForTask = false;
				break;
			case Task_Suspended:
				/*Nothing to do now*/
				break;
			default:
				Os_Control.state = Os_Error;
				Os_Control.error = Os_error_invalid_state;
				seekForTask = false;
				errorHook(Os_SelectNextTask_ByPriority);
			}
		}

		if (allBlocked)
		{
			/*No one task ready for actual priority */
			taskSelected = NULL;
		}
		else if (id != Os_Control.schedule.TareaPorPrioridad[prioridad].ActualTareaId)
		{
			Os_Control.schedule.TareaPorPrioridad[prioridad].ActualTareaId = id;
			taskSelected = Os_Control.schedule.TareaPorPrioridad[prioridad].tarea[id];
		}
		else
		{
			/*only actual task is ready to continue*/
			taskSelected = Os_Control.schedule.TareaPorPrioridad[prioridad].tarea[id];
		}
	}

	return (taskSelected);
}

/*************************************************************************************************
 *  @brief Extrae el codigo de error de la estructura de control del OS.
 *
 *  @details
 *   La estructura de control del OS no es visible al usuario, por lo que se facilita una API
 *   para extraer el ultimo codigo de error ocurrido, para su posterior tratamiento. Esta
 *   funcion puede ser utilizada dentro de errorHook
 *
 *  @param 		None.
 *  @return     Ultimo error ocurrido dentro del OS.
 *  @see errorHook
 ***************************************************************************************************/
int32_t os_getError(void)  {
	return Os_Control.error;
}

static void os_scheduler(void)
{
	uint8_t priority = 0;
	task_handler_t * taskSelected = NULL;

	Os_Control.cambioContextoNecesario = false;

	if (Os_From_Reset == Os_Control.state)
	{
		if (0 == Os_Control.TareaAgregad)
		{
			Os_Control.state = Os_Error;
			Os_Control.error = Os_error_notask;
			errorHook(os_scheduler);

		}
		else
		{
			/*seleccionar la primer tarea para que sea ejecutada*/
			/*se selecciona como primer tarea a ejecutar la tarea idle*/
			Os_Control.actualTarea = &tareaIdle;
			Os_Control.cambioContextoNecesario = true;
		}
	}
	else
	{
		/* Checkear que el SO no esté en medio de un scheduling en otro hilo */
		if (Os_Normal_Run == Os_Control.state)
		{
			Os_Control.state = Os_Scheduling;
			while ((OS_MAX_PRIORITY > priority) && (NULL == taskSelected))
			{
				taskSelected = Os_SelectNextTask_ByPriority(priority);
				priority++;
			}
			if (NULL == taskSelected)
			{
				taskSelected = &tareaIdle;
			}
			Os_Control.cambioContextoNecesario = (Os_Control.nextTarea != taskSelected);

			Os_Control.nextTarea = taskSelected;
			Os_Control.state = Os_Normal_Run;
		}
	}

	if (Os_Control.cambioContextoNecesario)
	{
		setPendSV();
	}
}

void Os_UpdateTicksInAllTaskBlocked()
{
	uint8_t priorityID;
	uint8_t i;
	for (priorityID = 0; priorityID< OS_MAX_PRIORITY; priorityID++)
	{
		for (i = 0; i< Os_Control.schedule.TareaPorPrioridad[priorityID].NumTarea; i++)
		{
			if ((Task_Blocked == Os_Control.schedule.TareaPorPrioridad[priorityID].tarea[i]->state) &&
					(0 < Os_Control.schedule.TareaPorPrioridad[priorityID].tarea[i]->blockedTicks))
			{
				Os_Control.schedule.TareaPorPrioridad[priorityID].tarea[i]->blockedTicks--;
				if (0 == Os_Control.schedule.TareaPorPrioridad[priorityID].tarea[i]->blockedTicks)
				{
					Os_Control.schedule.TareaPorPrioridad[priorityID].tarea[i]->state = Task_Ready;
				}
			}
		}
	}
}

void SysTick_Handler(void)
{
	Os_UpdateTicksInAllTaskBlocked();
	os_scheduler();
	/* tickHook*/
	tickHook();
}

/*************************************************************************************************
 *  @brief Setea la bandera correspondiente para lanzar PendSV.
 *
 *  @details
 *   Esta funcion simplemente es a efectos de simplificar la lectura del programa. Setea
 *   la bandera comrrespondiente para que se ejucute PendSV
 *
 *  @param 		None
 *  @return     None
 ***************************************************************************************************/
static void setPendSV(void)
{
	/**
	 * Se setea el bit correspondiente a la excepcion PendSV
	 */
	SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;

	/**
	 * Instruction Synchronization Barrier; flushes the pipeline and ensures that
	 * all previous instructions are completed before executing new instructions
	 */
	__ISB();

	/**
	 * Data Synchronization Barrier; ensures that all memory accesses are
	 * completed before next instruction is executed
	 */
	__DSB();
}

/*************************************************************************************************
 *  @brief Funcion para determinar el proximo contexto.
 *
 *  @details
 *   Esta funcion obtiene el siguiente contexto a ser cargado. El cambio de contexto se
 *   ejecuta en el handler de PendSV, dentro del cual se llama a esta funcion
 *
 *  @param 		sp_actual	Este valor es una copia del contenido de MSP al momento en
 *  			que la funcion es invocada.
 *  @return     El valor a cargar en MSP para apuntar al contexto de la tarea siguiente.
 ***************************************************************************************************/
uint32_t getContextoSiguiente(uint32_t p_stack_actual)  {
	uint32_t p_stack_siguiente = p_stack_actual; /* por defecto continuo con la tarea actual*/

	if (Os_From_Reset == Os_Control.state)
	{
		p_stack_siguiente = Os_Control.actualTarea->stackPointer;
		Os_Control.actualTarea->state = Task_Running;
		Os_Control.state = Os_Normal_Run;
	}
	else
	{

		Os_Control.actualTarea->stackPointer = p_stack_actual;

		if (Task_Running == Os_Control.actualTarea->state)
		{
			Os_Control.actualTarea->state = Task_Ready;
		}

		p_stack_siguiente = Os_Control.nextTarea->stackPointer;

		Os_Control.actualTarea = Os_Control.nextTarea;
		Os_Control.actualTarea->state = Task_Running;

	}

	return(p_stack_siguiente);
}

task_handler_t* Os_GetTareaActual()
{
	return (Os_Control.actualTarea);
}

os_state_t Os_GetControlState()
{
	return(Os_Control.state);
}

void Os_SetControlState(task_state_t newState)
{
	Os_Control.state = newState;
}

void Os_Critical_Enter()
{
	__disable_irq();
	Os_Control.TareaEnZonaCritica++;
}

void Os_Critical_Exit()
{
	Os_Control.TareaEnZonaCritica--;
	if (0 >= Os_Control.TareaEnZonaCritica)
	{
		Os_Control.TareaEnZonaCritica = 0;
		__enable_irq();
	}

}


/*************************************************************************************************
 *  @brief Fuerza una ejecucion del scheduler.
 *
 *  @details
 *   En los casos que un delay de una tarea comience a ejecutarse instantes luego de que
 *   ocurriese un scheduling, se despericia mucho tiempo hasta el proximo tick de sistema,
 *   por lo que se fuerza un scheduling y un cambio de contexto si es necesario.
 *
 *  @param 		None
 *  @return     None.
 ***************************************************************************************************/
void os_CpuYield(void)  {
	os_scheduler();
}

void Os_SetSchedulingFromIRQ()
{
	Os_Control.SchedulingFromIRQ = true;
}

void Os_ClearSchedulingFromIRQ()
{
	Os_Control.SchedulingFromIRQ = false;
}

bool Os_IsSchedulingFromIRQ()
{
	return(Os_Control.SchedulingFromIRQ);
}

void Os_SetError(os_error_t err, void* caller)
{
	Os_Control.error = err;
	errorHook(caller);
}
