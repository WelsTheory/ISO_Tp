/*
 * TIMER.c
 *
 *  Created on: Jul 1, 2022
 *      Author: Wels
 */
#include "TIMER.h"
#include "GPIO.h"

void TIM_Init(uint32_t arr_load)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	// ASCENDENTE
	TIM2->CR1 &= ~(TIM_CR1_DIR);
	// ÚNICO PULSO
	TIM2->CR1 |= TIM_CR1_OPM;
	// MODO RESET
	TIM2->CR2 &= ~(TIM_CR2_MMS);
	// PRESCALER => 0
	TIM2->PSC = 0;
	// VALOR AUTOGUARDADO -> 179999
	TIM2->ARR = arr_load - 1;
	// TEMPORIZACIÓN DE 0.1mS
}

void Timer_Delay_ms(uint32_t delay)
{
	// 50mS -> TEMPORIZACION 0.1mS
	// 50*10 = 500 * 0.1mS = 50mS
	TIM2->PSC = (delay * 10) - 1;
	// COUNTER ENABLE
	TIM2->CR1 |= TIM_CR1_CEN;
	// UI -> 0
	while((TIM2->SR & TIM_SR_UIF) == 0);
	// UI -> 1
	// APAGANDO EL FLAG
	TIM2->SR &= ~(TIM_SR_UIF);
}

void Timer_Periodi_Init(uint32_t delay, uint32_t arr_load)
{
	// CLOCK ENABLE
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	// ASCENDENTE
	TIM2->CR1 &= ~(TIM_CR1_DIR);
	// PERIODICO
	TIM2->CR1 &= ~(TIM_CR1_OPM);
	// MODO RESET
	TIM2->CR2 &= ~(TIM_CR2_MMS);
	// PRESCALER =>
	TIM2->PSC = (10 * delay) - 1;
	// VALOR AUTOGUARDADO -> 9000
	TIM2->ARR = arr_load - 1;
	// TEMPORIZACIÓN DE 0.1mS

	// LIMPIAR BANDERA
	TIM2->SR &= ~(TIM_SR_UIF);
	// HABILITAR INTERRUPCION
	TIM2->DIER |= TIM_DIER_UIE;
	// PRIORIDAD
	NVIC_SetPriority(TIM2_IRQn,2);
	// HABILITAR NVIC
	NVIC_EnableIRQ(TIM2_IRQn);
	// TIM2 ENABLE
	TIM2->CR1 |= TIM_CR1_CEN;
}

void TIM2_IRQHandler(void)
{
	NVIC_ClearPendingIRQ(TIM2_IRQn);
	TIM2->SR &= ~(TIM_SR_UIF);
	toggle_LED3();
}

