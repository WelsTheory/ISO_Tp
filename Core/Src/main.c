/*
 * main.c
 *
 *  Created on: Jun 30, 2022
 *      Author: Wels
 */
#include "main.h"
#include <string.h>
#include "RCC.h"
#include "SysTick.h"
#include "GPIO.h"
#include "USART.h"
#include "INT_EX.h"
#include "MSE_OS_Core.h"
#include "MSE_OS_API.h"

#define PRIORIDAD_0				0
#define PRIORIDAD_3				3

/*==================[Global data declaration]==============================*/

task_handler_t  _tarea1,_tarea2, _tareaUart;
os_cola_t cola_uart,cola_eventos;

/*================= DATOS A ENVIAR EN COLA =================================*/
typedef struct
{
	float floatData;
	int16_t intData;
	char message[30];
}cola_enviar_t;

typedef enum{
	INT2_ON,
	INT1_ON,
	WAITING
}event_t;

typedef struct
{
	event_t eventos;
}cola_eventos_t;

/*==================[internal functions declaration]=========================*/

/*==================[internal data definition]===============================*/

/*==================[external data definition]===============================*/

/*==================[internal functions definition]==========================*/

static void initHardware(void)  {
	Sys_ClockConfig();
	GPIO_Leds_Init();
	GPIO_Button_Init();
	INT_Init(); // Incialización de Interrupción
	USART_Init(Baudios_115);
	printf("--------------------------------------------\r\n");
	printf("¡TP FINAL OS! - WILLIAMS LIMONCHI\r\n");

	SysTick_ClockConfig(SysTick_ClockMax);
}

/*==================[Definicion de tareas para el OS]==========================*/
void tarea_tarea1(void)
{
	write_LED1(LOW);
	while (1) {
		toggle_LED1();
		os_Delay(200);
	}
}

void tarea_tarea2(void)
{
	cola_eventos_t msg;
	cola_enviar_t data;
	write_LED2(LOW);
	msg.eventos = WAITING;
	while (1) {
		Os_Cola_Read(&cola_eventos, &msg);
		switch(msg.eventos)
		{
		case WAITING:
			// NO ENVIA NADA
			break;
		case INT2_ON:
			data.floatData = 1.0;
			data.intData = 1;
			strcpy(data.message, "INTERRUPCION 2 HABILITADA\r\n");
			Os_Cola_Write(&cola_uart,&data);
			break;

		case INT1_ON:
			data.floatData = 3.1415;
			data.intData = -1;
			strcpy(data.message, "INTERRUPCION 1 HABILITADA\r\n");
			Os_Cola_Write(&cola_uart,&data);
			break;

		default:
			data.floatData = 0.0;
			data.intData = 0;
			strcpy(data.message, "     ERROR GENERADO    \r\n");
			Os_Cola_Write(&cola_uart,&data);
			break;

		}
		write_LED2(HIGH);
		os_Delay(500);
		write_LED2(LOW);
	}
}

void tarea_uart(void)
{
	cola_enviar_t data;
	write_LED3(LOW);
	while(1)
	{
		Os_Cola_Read(&cola_uart,&data);
		Os_Critical_Enter();
		USART_Cadena(data.message,30);
		Os_Critical_Exit();
		write_LED3(HIGH);
		os_Delay(100);
		write_LED3(LOW);
	}
}

void Int_Button1()
{
	cola_eventos_t msg;
	msg.eventos = INT1_ON;
	Os_Cola_Write(&cola_eventos, &msg);
}
void Int_Button2()
{
	cola_eventos_t msg;
	msg.eventos = INT2_ON;
	Os_Cola_Write(&cola_eventos, &msg);
}


int main(void)
{
	initHardware();

	Os_Cola_Init(&cola_eventos, sizeof(cola_enviar_t));
	Os_Cola_Init(&cola_uart, sizeof(cola_enviar_t));

	OS_InitTask(&_tarea1, tarea_tarea1, 2);
	OS_InitTask(&_tarea2, tarea_tarea2, 0);
	OS_InitTask(&_tareaUart, tarea_uart, 3);

	Os_SetIRQ(EXTI15_10_IRQn, Int_Button1);
	Os_SetIRQ(EXTI0_IRQn, Int_Button2);

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
