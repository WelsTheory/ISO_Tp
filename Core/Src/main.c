/*
 * main.c
 *
 *  Created on: Jun 30, 2022
 *      Author: Wels
 */
#include "main.h"
#include "RCC.h"
#include "SysTick.h"
#include "GPIO.h"
#include "USART.h"
#include "MSE_OS_Core.h"
#include "MSE_OS_API.h"

#define PRIORIDAD_0		0
#define PRIORIDAD_3		3

/*==================[Global data declaration]==============================*/

tarea Tarea1;	//prioridad 0
tarea Botones;	//prioridad 3

osSemaforo Sem_LED1;

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void initHardware(void)  {
	Sys_ClockConfig();
	GPIO_Leds_Init();
	GPIO_Button_Init();
	//USART_Init(Baudios_115);
	//printf("hola...\r\n");
	SysTick_ClockConfig(SysTick_ClockMax);
}

/*==================[Definicion de tareas para el OS]==========================*/
void tarea1(void)  {
	uint32_t i;

	while (1) {
		i++;

		if (i%9 == 0)
		{
			os_SemaforoTake(&Sem_LED1);
		}
		write_LED1(true);
		os_Delay(100);
		write_LED1(false);
		os_Delay(100);
	}
}

void botones(void)  {
	while(1)  {
		if(read_BUTTON() == HIGH)
		{
			os_SemaforoGive(&Sem_LED1);
		}

		os_Delay(100);
	}
}


int main(void)
{
	initHardware();

	OS_InitTask(tarea1, &Tarea1, PRIORIDAD_0);
	OS_InitTask(botones, &Botones, PRIORIDAD_3);

	os_SemaforoInit(&Sem_LED1);

	OS_Init();

	while(1)
	{
	}
	return 0;
}



