/*
 * MSE_OS_Core.c
 *
 *  Created on: 8 jul. 2022
 *      Author: Wels
 */
#include "MSE_OS_Core.h"

/********************************************************************************
 * Definicion de variables globales
 *******************************************************************************/

static os_control control_OS;
static tarea tareaIdle;

/********************************************************************************
 * Definicion de prototipos static
 *******************************************************************************/
static void initIdleTask(void);
static void setPendSV(void);
static void ordenarPrioridades(void);
static int32_t partition(tarea** arr, int32_t l, int32_t h);

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
void OS_InitTask(void *entryPoint, tarea *task, uint8_t prioridad )
{
	static uint8_t id = 0; //el id sera correlativo a medida que se generen mas tareas

	/*
	 * Al principio se efectua un pequeño checkeo para determinar si llegamos a la cantidad maxima de
	 * tareas que pueden definirse para este OS. En el caso de que se traten de inicializar mas tareas
	 * que el numero maximo soportado, se guarda un codigo de error en la estructura de control del OS
	 * y la tarea no se inicializa. La tarea idle debe ser exceptuada del conteo de cantidad maxima
	 * de tareas
	 */
	if (control_OS.cantidad_Tareas < TASK_MAX_ALLOWED)
	{
		task->stack[STACK_SIZE/4 - XPSR] = INIT_XPSR;								//necesari para bit thumb
		task->stack[STACK_SIZE/4 - PC_REG] = (uint32_t)entryPoint;
		task->stack[STACK_SIZE/4 - LR] = (uint32_t)returnHook;
		/**
		 * El valor previo de LR (que es EXEC_RETURN en este caso) es necesario dado que
		 * en esta implementacion, se llama a una funcion desde dentro del handler de PendSV
		 * con lo que el valor de LR se modifica por la direccion de retorno para cuando
		 * se termina de ejecutar getContextoSiguiente
		 */
		task->stack[STACK_SIZE/4 - LR_PREV] = EXEC_RETURN;

		task->stackPointer = (uint32_t) (task->stack + STACK_SIZE/4 - FULL_STACKING_SIZE);

		task->entryPoint = entryPoint;


		task->id = id;
		task->state = Task_Ready;
		task->priority = prioridad;


		/*
		 * Actualizacion de la estructura de control del OS, guardando el puntero a la estructura de tarea
		 * que se acaba de inicializar, y se actualiza la cantidad de tareas definidas en el sistema.
		 * Luego se incrementa el contador de id, dado que se le otorga un id correlativo a cada tarea
		 * inicializada, segun el orden en que se inicializan.
		 */
		control_OS.listaTareas[id] = task;
		control_OS.cantidad_Tareas++;
		control_OS.cantTareas_prioridad[prioridad]++;
		id++;
	}
	else
	{
		/*
		 * En el caso que se hayan excedido la cantidad de tareas que se pueden definir, se actualiza
		 * el ultimo error generado en la estructura de control del OS y se llama a errorHook y se
		 * envia informacion de quien es quien la invoca.
		 */
		control_OS.error = OS_ERROR_TAREAS;
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

	tareaIdle.priority = 0xFF;
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
	control_OS.estado_sistema = Os_From_Reset;
	control_OS.actualTask = NULL;
	control_OS.nextTask = NULL;

	/*
	 * El vector de tareas termina de inicializarse asignando NULL a las posiciones que estan
	 * luego de la ultima tarea. Esta situacion se da cuando se definen menos de 8 tareas.
	 * Estrictamente no existe necesidad de esto, solo es por seguridad.
	 */
	for (i = 0; i<TASK_MAX_ALLOWED; i++)
	{
		if(i>=control_OS.cantidad_Tareas)
		{
			control_OS.listaTareas[i] = NULL;
		}
	}

	ordenarPrioridades();
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
	return control_OS.error;
}

static void os_scheduler(void)
{
	static uint8_t indicePrioridad[COUNT_PRIORITY];
	uint8_t indiceArrayTareas = 0;
	uint8_t prioridadActual = MAX_PRIORITY;
	uint8_t cantBloqueadas_prioridadActual = 0;
	bool salir = false;
	uint8_t cant_bloqueadas = 0;

	if (Os_From_Reset == control_OS.estado_sistema)
	{
		control_OS.actualTask = (tarea*)&tareaIdle;
		memset(indicePrioridad,0,sizeof(uint8_t)*COUNT_PRIORITY);
		control_OS.estado_sistema = Os_Normal_Run;
		/*
		 * Se comienza a iterar sobre el vector de tareas, pero teniendo en cuenta las
		 * prioridades de las tareas.
		 * En esta implementacion, durante la ejecucion de os_Init() se aplica la
		 * tecnica de quicksort sobre el vector que contiene la lista de tareas
		 * y se ordenan los punteros a tareas segun la prioridad que tengan. Los
		 * punteros a tareas quedan ordenados de mayor a menor prioridad.
		 * Gracias a eso, dividiendo el vector de punteros en cuantas prioridades haya
		 * podemos recorrer estas subsecciones manteniendo indices para cada prioridad
		 *
		 * La mecanica de RoundRobin para tareas de igual prioridad se mantiene,
		 * asimismo la determinacion de cuando la tarea idle debe ser ejecutada. Cuando
		 * se recorren todas las tareas de una prioridad y todas estan bloqueadas, entonces
		 * se pasa a la prioridad menor siguiente
		 *
		 * Recordar que aunque todas las tareas definidas por el usuario esten bloqueadas
		 * la tarea Idle solamente puede tomar estados READY y RUNNING.
		 */


		/*
		 * Puede darse el caso en que se haya invocado la funcion os_CpuYield() la cual hace una
		 * llamada al scheduler. Si durante la ejecucion del scheduler (la cual fue forzada) y
		 * esta siendo atendida en modo Thread ocurre una excepcion dada por el SysTick, habra una
		 * instancia del scheduler pendiente en modo trhead y otra corriendo en modo Handler invocada
		 * por el SysTick. Para evitar un doble scheduling, se controla que el sistema no este haciendo
		 * uno ya. En caso afirmativo se vuelve prematuramente
		 */
		if(control_OS.estado_sistema == Os_Scheduling)
		{
			return;
		}
		/*
		 * Cambia el estado del sistema para que no se produzcan schedulings anidados cuando
		 * existen forzados por alguna API del sistema.
		 */
		control_OS.estado_sistema = Os_Scheduling;
		while(!salir)
		{
			/*
			 * La variable indiceArrayTareas contiene la posicion real dentro del array listaTareas
			 * de la tarea siguiente a ser ejecutada. Se inicializa en 0
			 */
			indiceArrayTareas = 0;

			/*
			 * Puede darse el caso de que se hayan definido tareas de prioridades no contiguas, por
			 * ejemplo dos tareas de maxima prioridad y una de minima prioridad. Quiere decir que no
			 * existen tareas con prioridades entre estas dos. Este bloque if determina si existen
			 * funciones para la prioridad actual. Si no existen, se pasa a la prioridad menor
			 * siguiente
			 */
			if(control_OS.cantTareas_prioridad[prioridadActual] > 0)  {

				/*
				 * esta linea asegura que el indice siempre este dentro de los limites de la subseccion
				 * que forman los punteros a las tareas de prioridad actual
				 */
				indicePrioridad[prioridadActual] %= control_OS.cantTareas_prioridad[prioridadActual];

				/*
				 * El bucle for hace una conversion de indices de subsecciones al indice real que debe
				 * usarse sobre el vector de punteros a tareas. Cuando se baja de prioridad debe sumarse
				 * el total de tareas que existen en la subseccion anterior. Recordar que el vector de
				 * tareas esta ordenado de mayor a menor
				 */
				for (int i=0; i<prioridadActual;i++) {
					indiceArrayTareas += control_OS.cantTareas_prioridad[i];
				}
				indiceArrayTareas += indicePrioridad[prioridadActual];

				switch (((tarea*)control_OS.listaTareas[indiceArrayTareas])->state) {

				case Task_Ready:
					control_OS.nextTask = (tarea*) control_OS.listaTareas[indiceArrayTareas];
					control_OS.cambioContextoNecesario = true;
					indicePrioridad[prioridadActual]++;
					salir = true;
					break;

					/*
					 * Para el caso de las tareas bloqueadas, la variable cantBloqueadas_prioActual se utiliza
					 * para hacer el seguimiento de si se debe bajar un escalon de prioridad
					 * El bucle del scheduler se ejecuta completo, pasando por todas las prioridades cada vez
					 * hasta que encuentra una tarea en READY.
					 * La determinacion de la necesidad de ejecucion de la tarea idle sigue siendo la misma.
					 */
				case Task_Blocked:
					cant_bloqueadas++;
					cantBloqueadas_prioridadActual++;
					indicePrioridad[prioridadActual]++;
					if (cant_bloqueadas == control_OS.cantidad_Tareas)  {
						control_OS.nextTask = &tareaIdle;
						control_OS.cambioContextoNecesario = true;
						salir = true;
					}
					else {
						if(cantBloqueadas_prioridadActual == control_OS.cantTareas_prioridad[prioridadActual])  {
							cantBloqueadas_prioridadActual = 0;
							indicePrioridad[prioridadActual] = 0;
							prioridadActual++;
						}
					}
					break;

					/*
					 * El unico caso que la siguiente tarea este en estado RUNNING es que
					 * todas las demas tareas excepto la tarea corriendo actualmente esten en
					 * estado BLOCKED, con lo que un cambio de contexto no es necesario, porque
					 * se sigue ejecutando la misma tarea
					 */
				case Task_Running:
					indicePrioridad[prioridadActual]++;
					control_OS.cambioContextoNecesario = false;
					salir = true;
					break;

					/*
					 * En el caso que lleguemos al caso default, la tarea tomo un estado
					 * el cual es invalido, por lo que directamente se llama errorHook
					 * y se actualiza la variable de ultimo error
					 */
				default:
					control_OS.error = OS_ERROR_SCHEDULING;
					errorHook(os_scheduler);
				}
			}

			/*
			 * Antes de salir del scheduler se devuelve el sistema a su estado normal
			 */
			control_OS.estado_sistema = Os_Normal_Run;

			/*
			 * Se checkea la bandera correspondiente para verificar si es necesario un cambio de
			 * contexto. En caso afirmativo, se lanza PendSV
			 */

			if(control_OS.cambioContextoNecesario)
				setPendSV();
		}
	}
}

void SysTick_Handler(void)  {
	uint8_t i;
	tarea* task;		//variable para legibilidad

	/*
	 * Systick se encarga de actualizar todos los temporizadores por lo que se recorren
	 * todas las tareas que esten definidas y si tienen un valor de ticks de bloqueo mayor
	 * a cero, se decrementan en una unidad. Si este contador llega a cero, entonces
	 * se debe pasar la tarea a READY. Es conveniente hacerlo aqui dado que la condicion
	 * de que pase a descontar el ultimo tick se da en esta porcion de codigo
	 */
	i = 0;

	while (control_OS.listaTareas[i] != NULL)  {
		task = (tarea*)control_OS.listaTareas[i];

		if(task->blockedTicks > 0 )  {
			if((--task->blockedTicks == 0) && (task->state == Task_Blocked))  {
				task->state = Task_Ready;
			}
		}

		i++;
	}

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
static void setPendSV(void)  {

	/*
	 * Se indica en la estructura del OS que el cambio de contexto se esta por invocar
	 * Se hace antes de setear PendSV para no interferir con las barreras de datos
	 * y memoria
	 */
	control_OS.cambioContextoNecesario = false;

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
uint32_t getContextoSiguiente(uint32_t sp_actual)  {
	uint32_t sp_siguiente;

	/*
	 * Esta funcion efectua el cambio de contexto. Se guarda el MSP (sp_actual) en la variable
	 * correspondiente de la estructura de la tarea corriendo actualmente. Ahora que el estado
	 * BLOCKED esta implementado, se debe hacer un assert de si la tarea actual fue expropiada
	 * mientras estaba corriendo o si la expropiacion fue hecha de manera prematura dado que
	 * paso a estado BLOCKED. En el segundo caso, solamente se puede pasar de BLOCKED a READY
	 * a partir de un evento. Se carga en la variable sp_siguiente el stack pointer de la
	 * tarea siguiente, que fue definida por el scheduler. Se actualiza la misma a estado RUNNING
	 * y se retorna al handler de PendSV
	 */

	control_OS.actualTask->stackPointer = sp_actual;

	if (control_OS.actualTask->state == Task_Running)
		control_OS.actualTask->state = Task_Ready;

	sp_siguiente = control_OS.nextTask->stackPointer;

	control_OS.actualTask = control_OS.nextTask;
	control_OS.actualTask->state = Task_Running;


	/*
	 * Indicamos que luego de retornar de esta funcion, ya no es necesario un cambio de contexto
	 * porque se acaba de gestionar.
	 */
	control_OS.estado_sistema = Os_Normal_Run;

	return sp_siguiente;
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



/*************************************************************************************************
	 *  @brief Devuelve una copia del puntero a estructura tarea actual.
     *
     *  @details
     *   En aras de mantener la estructura de control aislada solo en el archivo de core esta
     *   funcion proporciona una copia de la estructura de la tarea actual
     *
	 *  @param 		None
	 *  @return     puntero a la estructura de la tarea actual.
***************************************************************************************************/
tarea* os_getTareaActual(void)  {
	return control_OS.actualTask;
}


/*************************************************************************************************
	 *  @brief Ordena tareas de mayor a menor prioridad.
     *
     *  @details
     *   Ordena los punteros a las estructuras del tipo tarea que estan almacenados en la variable
     *   de control de OS en el array listadoTareas por prioridad, de mayor a menor. Para esto
     *   utiliza un algoritmo de quicksort. Esto da la posibilidad de cambiar la prioridad
     *   de cualquier tarea en tiempo de ejecucion.
     *
	 *  @param 		None
	 *  @return     None.
***************************************************************************************************/
static void ordenarPrioridades(void)  {
	// Create an auxiliary stack
	int32_t stack[TASK_MAX_ALLOWED];

	// initialize top of stack
	int32_t top = -1;
	int32_t l = 0;
	int32_t h = control_OS.cantidad_Tareas - 1;

	// push initial values of l and h to stack (indices a estructuras de tareas)
	stack[++top] = l;
	stack[++top] = h;

	// Keep popping from stack while is not empty
	while (top >= 0) {
		// Pop h and l
		h = stack[top--];
		l = stack[top--];

		// Set pivot element at its correct position
		// in sorted array
		int32_t p = partition(control_OS.listaTareas, l, h);

		// If there are elements on left side of pivot,
		// then push left side to stack
		if (p - 1 > l) {
			stack[++top] = l;
			stack[++top] = p - 1;
		}

		// If there are elements on right side of pivot,
		// then push right side to stack
		if (p + 1 < h) {
			stack[++top] = p + 1;
			stack[++top] = h;
		}
	}
}


/*************************************************************************************************
	 *  @brief Ordena tareas de mayor a menor prioridad.
     *
     *  @details
     *   Funcion de soporte para ordenarPrioridades. No debe llamarse fuera de mencionada
     *   funcion.
     *
	 *  @param 	arr		Puntero a la lista de punteros de estructuras de tareas a ordenar
	 *  @param	l		Inicio del vector a ordenar (puede ser un subvector)
	 *  @param	h		Fin del vector a ordenar (puede ser un subvector)
	 *  @return     	Retorna la posicion del pivot necesario para el algoritmo
***************************************************************************************************/
static int32_t partition(tarea** arr, int32_t l, int32_t h)  {
	tarea* x = arr[h];
	tarea* aux;
	int32_t i = (l - 1);

	for (int j = l; j <= h - 1; j++) {
		if (arr[j]->priority <= x->priority) {
			i++;
			//swap(&arr[i], &arr[j]);
			aux = arr[i];
			arr[i] = arr[j];
			arr[j] = aux;
		}
	}
	//swap(&arr[i + 1], &arr[h]);
	aux = arr[i+1];
	arr[i+1] = arr[h];
	arr[h] = aux;

	return (i + 1);
}
