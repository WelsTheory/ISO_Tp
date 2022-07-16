/*
 * MSE_OS_Core.h
 *
 *  Created on: 8 jul. 2022
 *      Author: Wels
 */

#ifndef INC_MSE_OS_CORE_H_
#define INC_MSE_OS_CORE_H_

#include <stdint.h>


/************************************************************************************
 * 			Tamaño del stack predefinido para cada tarea expresado en bytes
 ***********************************************************************************/

#define STACK_SIZE 256

//----------------------------------------------------------------------------------



/************************************************************************************
 * 	Posiciones dentro del stack frame de los registros que conforman el stack frame
 ***********************************************************************************/

#define XPSR		1
#define PC_REG		2
#define LR			3
#define R12			4
#define R3			5
#define R2			6
#define R1			7
#define R0			8

//----------------------------------------------------------------------------------


/************************************************************************************
 * 			Valores necesarios para registros del stack frame inicial
 ***********************************************************************************/

#define INIT_XPSR 	1 << 24				//xPSR.T = 1
#define EXEC_RETURN	0xFFFFFFF9			//retornar a modo thread con MSP, FPU no utilizada

//----------------------------------------------------------------------------------


/************************************************************************************
 * 						Definiciones varias
 ***********************************************************************************/
#define STACK_FRAME_SIZE	8


/*==================[definicion de prototipos]=================================*/

void os_InitTarea(void *tarea, uint32_t *stack, uint32_t *stack_pointer);

#endif /* INC_MSE_OS_CORE_H_ */
