/*
 * SysTick.c
 *
 *  Created on: Jul 1, 2022
 *      Author: Wels
 */
#include "SysTick.h"

static __IO uint32_t msTick = 0;

void SysTick_ClockConfig(uint32_t arr)
{
	// Reset Control Register
	SysTick->CTRL = 0;
	// Set Reload Value
	SysTick->LOAD = arr-1;
	// Priority Systick Interrupt
	NVIC_SetPriority(SysTick_IRQn,0);
	// Reset Systick Value to 0
	SysTick->VAL = 0;
	// Enable SysTick from control Register
	SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void msIncTicks(void)
{
	msTick++;
}

uint32_t msGetTicks(void)
{
	return msTick;
}

void Delay_ms(uint32_t ms)
{
	uint32_t startTicks = msGetTicks();
	while((msGetTicks() - startTicks) < ms);
}

//void SysTick_Handler(void)
//{
//	NVIC_ClearPendingIRQ(SysTick_IRQn);
//	msIncTicks();
//}

/*NO bloqueante
 * */

void delay_msNB(delay_t * delay,uint32_t duration){
	delay->duration = duration/1;
	delay->running = 0;
}

bool delayRead(delay_t * delay){
	bool timeArrived = 0;

	if( !delay->running ) {
	  delay->startTime = msGetTicks();
	  delay->running = 1;
	}
	else {
	  if ((uint32_t)(msGetTicks()-delay->startTime) >= delay->duration ) {
		 timeArrived = 1;
		 delay->running = 0;
	  }
	}

	return timeArrived;
}

/*
 *
 * #define TIEMPO_NORMAL 	500

delay_t Delay_Normal;

 * if(delayRead(&Delay_Normal)){
			printf("Hey!\n\r");
			toggle_LED3();
		}
 * */

