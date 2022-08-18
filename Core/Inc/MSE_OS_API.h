/*
 * MSE_OS_API.h
 *
 *  Created on: 12 ago. 2022
 *      Author: Wels
 */

#ifndef INC_MSE_OS_API_H_
#define INC_MSE_OS_API_H_

#include "MSE_OS_Core.h"

/********************************************************************************
 * Definicion de la estructura para los semaforos
 *******************************************************************************/
struct _semaforo  {
	tarea* tarea_asociada;
	bool tomado;
};

typedef struct _semaforo osSemaforo;

void os_Delay(uint32_t ticks);
void os_SemaforoInit(osSemaforo* sem);
void os_SemaforoTake(osSemaforo* sem);
void os_SemaforoGive(osSemaforo* sem);

#endif /* INC_MSE_OS_API_H_ */
