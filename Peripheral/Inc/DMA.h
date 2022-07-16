/*
 * DMA.h
 *
 *  Created on: 1 jul. 2022
 *      Author: Wels
 */

#ifndef INC_DMA_H_
#define INC_DMA_H_

#include "main.h"

void ADC_DMAConfig(uint32_t *pData, uint32_t len, uint32_t base);

void ADC_OneChannle(void);

/**/
void USART_DMAConfig(uint32_t *pData, uint32_t len, uint32_t base);

#endif /* INC_DMA_H_ */
