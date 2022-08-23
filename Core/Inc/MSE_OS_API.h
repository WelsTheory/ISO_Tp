/*
 * MSE_OS_API.h
 *
 *  Created on: 12 ago. 2022
 *      Author: Wels
 */

#ifndef INC_MSE_OS_API_H_
#define INC_MSE_OS_API_H_

#include "MSE_OS_Core.h"

#define OS_COLA_HEAP_SIZE		256
#define OS_COLA_DEFAULT_VALUE 0xFF

/********************************************************************************
 * Definicion de la estructura para los semaforos
 *******************************************************************************/
typedef struct   {
	task_handler_t* tarea_asociada;
	bool tomado;
}os_semaforo_t;

/********************************************************************************
 * Definicion de la estructura para las colas
 *******************************************************************************/
typedef struct {
	uint16_t head;
	uint16_t tail;
	uint16_t size_cola;
	uint16_t element_max;
	uint16_t element_size;
	uint8_t data[OS_COLA_HEAP_SIZE];
	task_handler_t* tareaesperando;
}os_cola_t;



void os_Delay(uint32_t ticks);

void Os_Sem_Init(os_semaforo_t* sem);
void Os_Sem_Take(os_semaforo_t* sem);
void Os_Sem_Give(os_semaforo_t* sem);

void Os_Cola_Init(os_cola_t* cola, uint16_t datasize);
void Os_Cola_Write(os_cola_t* cola, void* data);
void Os_Cola_Read(os_cola_t* cola, void* data);

#endif /* INC_MSE_OS_API_H_ */
