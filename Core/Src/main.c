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

uint32_t stack1[STACK_SIZE];
uint32_t stack2[STACK_SIZE];
uint32_t stack3[STACK_SIZE];

uint32_t sp_tarea1;
uint32_t sp_tarea2;
uint32_t sp_tarea3;

void tarea_Wels(void)  {
	int i;
	while (1) {
		i++;
	}
}

void tarea_Theory(void)  {
	int j;
	while (1) {
		j++;
	}
}

void tarea_tercera(void)  {
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
	os_InitTarea(tarea_Wels, &stack1, &sp_tarea1);
	os_InitTarea(tarea_Theory, &stack2, &sp_tarea2);
	os_InitTarea(tarea_tercera, &stack3, &sp_tarea3);
	while(1)
	{

	}
	return 0;
}



