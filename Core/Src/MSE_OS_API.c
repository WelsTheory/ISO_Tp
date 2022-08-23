/*
 * MSE_OS_API.c
 *
 *  Created on: 12 ago. 2022
 *      Author: Wels
 */
#include "MSE_OS_API.h"
#include "MSE_OS_Core.h"

/*************************************************************************************************
 *  @brief delay no preciso en base a ticks del sistema
 *
 *  @details
 *   Para utilizar un delay en el OS se vale del tick de sistema para contabilizar cuantos
 *   ticks debe una tarea estar bloqueada.
 *
 *  @param		ticks	Cantidad de ticks de sistema que esta tarea debe estar bloqueada
 *  @return     None.
 ***************************************************************************************************/
void os_Delay(uint32_t ticks)  {
	task_handler_t* tarea_actual;

	if (Os_Running_from_IRQ == Os_GetControlState())
	{
		Os_SetError(Os_error_daly_from_IRQ, os_Delay);
	}

	if (0 < ticks)
	{
		Os_Critical_Enter();
		tarea_actual = Os_GetTareaActual();
		tarea_actual->blockedTicks = ticks;
		Os_Critical_Exit();

		/*
		 * El proximo bloque while tiene la finalidad de asegurarse que la tarea solo se desbloquee
		 * en el momento que termine la cuenta de ticks. Si por alguna razon la tarea se vuelve a
		 * ejecutar antes que termine el periodo de bloqueado, queda atrapada.
		 *
		 */

		while (0 < tarea_actual->blockedTicks)
		{
			tarea_actual->state = Task_Blocked;
			os_CpuYield();
		}
	}
}


/*************************************************************************************************
 *  @brief Inicializacion de un semaforo binario
 *
 *  @details
 *   Antes de utilizar cualquier semaforo binario en el sistema, debe inicializarse el mismo.
 *   Todos los semaforos se inicializan tomados
 *
 *  @param		sem		Semaforo a inicializar
 *  @return     None.
 ***************************************************************************************************/
void Os_Sem_Init(os_semaforo_t* sem)  {
	sem->tomado = true;
	sem->tarea_asociada = NULL;
}



/*************************************************************************************************
 *  @brief Tomar un semaforo
 *
 *  @details
 *   Esta funcion es utilizada para tomar un semaforo cualquiera.
 *
 *  @param		sem		Semaforo a tomar
 *  @return     None.
 ***************************************************************************************************/
void Os_Sem_Take(os_semaforo_t* sem)  {
	bool tomado = false;
	task_handler_t* tarea_actual;

	while (!tomado)
	{
		if (sem->tomado)
		{
			/* esperar hasta que esté libre el semaforo*/

			Os_Critical_Enter();
			tarea_actual = Os_GetTareaActual();
			tarea_actual->state = Task_Blocked;
			sem->tarea_asociada = tarea_actual;
			Os_Critical_Exit();

			os_CpuYield();
		}
		else
		{
			sem->tomado = true;
			sem->tarea_asociada = tarea_actual;
			tomado = true;
		}
	}
}



/********************************************************************************
 *  @brief Liberar un semaforo
 *
 *  @details
 *   Esta funcion es utilizada para liberar un semaforo cualquiera.
 *
 *  @param		sem		Semaforo a liberar
 *  @return     None.
 *******************************************************************************/
void Os_Sem_Give(os_semaforo_t* sem)
{
	if ((sem->tomado) &&
			(NULL != sem->tarea_asociada))
	{
		sem->tomado = false;
		sem->tarea_asociada->state = Task_Ready;

		if (Os_Running_from_IRQ == Os_GetControlState())
		{
			Os_SetSchedulingFromIRQ();
		}
	}
}

/*************************************************************************************************
 *  @brief Inicializacion de una cola
 *
 *  @details
 *   Antes de utilizar cualquier cola en el sistema, debe inicializarse la misma.
 *   Todas las colas se inicializan vacias y sin una tarea asociada. Aqui se determina
 *   cuantos elementos (espacios) tendra disponible la cola dado el tamaño de la cola
 *   en bytes definida por la constante QUEUE_HEAP_SIZE y el tamaño en bytes de cada
 *   elemento que se desea almacenar. Es una forma facil de determinar los limites
 *   de los indices head y tail en el momento de la operacion. Se puede volver a inicializar
 *   una cola para resetearla y cambiar el tipo de datos que contiene
 *
 *  @param		cola		Cola a inicializar
 *  @param		datasize	Tamaño de los elementos que seran almacenados en la cola.
 *  						Debe ser pasado mediante la funcion sizeof()
 *  @return     None.
 *
 *  @warning	Esta inicializacion fija el tamaño de cada elemento, por lo que no vuelve
 *  			a consultarse en otras funciones, pasar datos con otros tamaños en funciones
 *  			de escritura y lectura puede dar lugar a corrupcion de datos.
 ***************************************************************************************************/
void Os_Cola_Init(os_cola_t* cola, uint16_t datasize)  {
	cola->element_size = datasize;
	cola->size_cola = 0;
	cola->element_max = OS_COLA_HEAP_SIZE / datasize;
	cola->tareaesperando = NULL;
	cola->head = 0;
	cola->tail = 0;
	memset(cola->data, OS_COLA_DEFAULT_VALUE,OS_COLA_HEAP_SIZE);
}


/*************************************************************************************************
 *  @brief Escritura en una cola
 *
 *  @details
 *
 *
 *  @param		cola		Cola donde escribir el dato
 *  @param		dato		Puntero a void del dato a escribir
 *  @return     None.
 ***************************************************************************************************/
void Os_Cola_Write(os_cola_t* cola, void* data)
{
	task_handler_t* actualTask;

	if (0 == cola->size_cola)
	{
		if ((NULL != cola->tareaesperando) &&
				(Task_Blocked == cola->tareaesperando->state))
		{
			cola->tareaesperando->state = Task_Ready;
		}

		if (Os_Running_from_IRQ == Os_GetControlState())
		{
			Os_SetSchedulingFromIRQ();
		}

	}

	if ((Os_Running_from_IRQ == Os_GetControlState()) &&
			(cola->size_cola >= cola->element_max))
	{

	}
	else
	{
		while (cola->size_cola >= cola->element_max)
		{
			Os_Critical_Enter();
			actualTask = Os_GetTareaActual();
			actualTask->state = Task_Blocked;
			cola->tareaesperando = actualTask;
			Os_Critical_Exit();

			os_CpuYield();
		}

		memcpy(cola->data + (cola->head * cola->element_size), data, cola->element_size);
		cola->head++;
		if (cola->head >= cola->element_max)
		{
			cola->head = 0;
		}
		cola->size_cola++;
		cola->tareaesperando = NULL;
	}
}

void Os_Cola_Read(os_cola_t* cola, void* data)
{
	task_handler_t* actualTask;

	if (cola->size_cola >= cola->element_max)
	{
		if ((NULL != cola->tareaesperando) &&
				(Task_Blocked == cola->tareaesperando->state))
		{
			cola->tareaesperando->state = Task_Ready;
		}

		if (Os_Running_from_IRQ == Os_GetControlState())
		{
			Os_SetSchedulingFromIRQ();
		}
	}

	if ((Os_Running_from_IRQ == Os_GetControlState()) &&
			(0 == cola->size_cola ))
	{

	}
	else
	{
		while (0 == cola->size_cola)
		{
			Os_Critical_Enter();
			actualTask = Os_GetTareaActual();
			actualTask->state = Task_Blocked;
			cola->tareaesperando = actualTask;
			Os_Critical_Exit();

			os_CpuYield();
		}

		memcpy(data, cola->data + (cola->tail * cola->element_size), cola->element_size);
		cola->tail++;
		if (cola->tail >= cola->element_max)
		{
			cola->tail = 0;
		}
		cola->size_cola--;
		cola->tareaesperando = NULL;
	}
}



