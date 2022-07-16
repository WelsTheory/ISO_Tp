/*
 * SysTick.h
 *
 *  Created on: Jul 1, 2022
 *      Author: Wels
 */

#ifndef INC_SYSTICK_H_
#define INC_SYSTICK_H_

#include "main.h"

#define SysTick_ClockMax 180000

void SysTick_ClockConfig(uint32_t arr);
void msIncTicks(void);
uint32_t msGetTicks(void);
void Delay_ms(uint32_t ms);

/*no bloqueante
 * */
typedef struct{
	uint32_t startTime;
	uint32_t duration;
	uint32_t running;
}delay_t;

void delay_msNB(delay_t * delay,uint32_t duration);
bool delayRead(delay_t * delay);


#endif /* INC_SYSTICK_H_ */
