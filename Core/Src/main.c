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

osSemaforo colaTarea1;

/*================= DATOS A ENVIAR EN COLA =================================*/
struct _mydata  {
	float var_float;
	int32_t var_int;
	char name[3];
};

typedef struct _mydata my_data;

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
	my_data datos;

	while (1) {

		os_ColaRead(&colaTarea1,&datos);
		write_LED1(true);
		os_Delay(1000);
		write_LED1(false);
		memset(&datos,0x00,sizeof(my_data));

	}
}

void botones(void)  {
	my_data datos_enviar;
	while(1)  {
		if(read_BUTTON() == HIGH)
		{
			datos_enviar.var_float = 2.5;
			datos_enviar.var_int = -5;
			datos_enviar.name[0] = 'I';
			datos_enviar.name[1] = 'S';
			datos_enviar.name[2] = 'O';
			os_ColaWrite(&colaTarea1,&datos_enviar);
		}

		os_Delay(100);
	}
}


int main(void)
{
	initHardware();

	OS_InitTask(tarea1, &Tarea1, PRIORIDAD_0);
	OS_InitTask(botones, &Botones, PRIORIDAD_3);

	os_ColaInit(&colaTarea1,sizeof(my_data));

	OS_Init();

	while(1)
	{
	}
	return 0;
}



