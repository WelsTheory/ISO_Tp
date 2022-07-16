/*
 * ADC.h
 *
 *  Created on: Jul 1, 2022
 *      Author: Wels
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "main.h"

void ADC_Init(void);

void ADC_Start(void);

void ADC_Stop(void);

bool ADC_PollEndConversion(uint32_t timeout);

uint16_t ADC_Read(void);

/**/
void ADC_DisableNVIC(void);

void ADC_EnableNVIC(void);

/*adc multichannel*/
void ADC_Multi(void);


#endif /* INC_ADC_H_ */
