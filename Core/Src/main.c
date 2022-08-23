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

task_handler_t Tarea1,Tarea2, Botones;	//prioridad 3

os_semaforo_t semLed1,semLed2,semLed3;

os_cola_t cola1,cola2;

/*================= DATOS A ENVIAR EN COLA =================================*/
typedef struct
{
	float	floatData;
	int16_t intData;
	char message[8];
} cola_enviar_t;

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
/*==================[Definicion de tareas para el OS]==========================*/
void tarea_tarea1(void)
{
	int i = 0;
	cola_enviar_t queueData;
	queueData.floatData = 3.1421;
	queueData.intData = -128;
	strcpy(queueData.message, "hola");
	write_LED1(LOW);
	while (1) {
		if (i%10 == 0)
		{
			Os_Cola_Write(&cola1, &queueData);
			Os_Sem_Take(&semLed1);
		}
		toggle_LED1();
		os_Delay(200);
		i++;
	}
}

void tarea_tarea2(void)
{
	int k = 0;
	write_LED2(LOW);
	cola_enviar_t queueData;
	while (1) {
		if (k%10 == 0)
		{
			Os_Cola_Read(&cola1, &queueData);
		}
		write_LED2(HIGH);
		os_Delay(500);
		write_LED2(LOW);
		os_Delay(500);
		k++;
	}
}

void tarea_botones(void)
{
	cola_enviar_t queueData;
	queueData.floatData = 189.1421;
	queueData.intData = -87;
	strcpy(queueData.message, "test2");
	while(1)  {
		if(read_BUTTON())
			Os_Sem_Give(&semLed1);
		os_Delay(100);
	}
}

int main(void)
{
	initHardware();

	Os_Sem_Init(&semLed1);
	Os_Sem_Init(&semLed2);
	Os_Sem_Init(&semLed3);

	Os_Cola_Init(&cola1, sizeof(cola_enviar_t));
	Os_Cola_Init(&cola2, sizeof(cola_enviar_t));

	OS_InitTask(&Tarea1, tarea_tarea1, 1);
	OS_InitTask(&Tarea2, tarea_tarea2, 0);
	OS_InitTask(&Botones, tarea_botones, 3);

	OS_Init();

	while(1)
	{
		__WFI();
	}
	return 0;
}

static uint32_t global_tickCounter = 0;
void tickHook(void)
{
	global_tickCounter++;
}

static uint32_t globla_idleTaskCounter = 0;
void taskIdleHook()
{
	while(1)
	{
		globla_idleTaskCounter++;
	}
}

