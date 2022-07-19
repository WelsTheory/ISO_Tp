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

task_handler_t task1, task2, task3;

void tarea1(void)  {
	int i;
	while (1) {
		i++;
	}
}

void tarea2(void)  {
	int j;
	while (1) {
		j++;
	}
}

void tarea3(void)  {
	int k;
	while (1) {
		k++;
	}
}

int main(void)
{
	Sys_ClockConfig();
	GPIO_Leds_Init();
	USART_Init(Baudios_115);
	printf("hola...\r\n");
	SysTick_ClockConfig(SysTick_ClockMax);
	OS_Init();
	OS_InitTask(&task1, tarea1);
	OS_InitTask(&task2, tarea2);
	OS_InitTask(&task3, tarea3);

	while(1)
	{
		__WFI();
	}
	return 0;
}



