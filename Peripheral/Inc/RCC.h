/*
 * RCC.h
 *
 *  Created on: Jun 30, 2022
 *      Author: Wels
 */

#ifndef INC_RCC_H_
#define INC_RCC_H_

#include "main.h"

#define PLL_M 	4
#define PLL_N 	180
#define PLL_P 	0  //2
#define PLL_Q	7

void Sys_ClockConfig(void);

#endif /* INC_RCC_H_ */
