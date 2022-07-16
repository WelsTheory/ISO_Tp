/*
 * TIMER.h
 *
 *  Created on: Jul 1, 2022
 *      Author: Wels
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "main.h"

#define TIM2_90MHz 9000

void TIM_Init(uint32_t arr_load);

void Timer_Delay_ms(uint32_t delay);

void Timer_Periodi_Init(uint32_t delay, uint32_t arr_load);

#endif /* INC_TIMER_H_ */
