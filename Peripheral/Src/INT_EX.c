/*
 * INT_EX.c
 *
 *  Created on: Jul 1, 2022
 *      Author: Wels
 */
#include "INT_EX.h"

uint16_t flag = 0;

void INT_Init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	SYSCFG->EXTICR[3] &= ~(SYSCFG_EXTICR4_EXTI13);
	SYSCFG->EXTICR[3] |= (SYSCFG_EXTICR4_EXTI13_PC);

	EXTI->IMR |= EXTI_IMR_IM13;
	EXTI->EMR |= EXTI_EMR_EM13;

	EXTI->FTSR |= EXTI_FTSR_TR13;
	EXTI->RTSR &= ~(EXTI_RTSR_TR13);

	NVIC_SetPriority(EXTI15_10_IRQn,1);

	NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI->PR & EXTI_PR_PR13)
	{
		flag = 1;
		NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
		EXTI->PR |= (EXTI_PR_PR13);
	}
}




