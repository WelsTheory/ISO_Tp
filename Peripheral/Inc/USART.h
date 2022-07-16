/*
 * USART.h
 *
 *  Created on: Jul 1, 2022
 *      Author: Wels
 */

#ifndef INC_USART_H_
#define INC_USART_H_

#include "main.h"

/*
 * PD8 -> RX
 * PD9 -> TX
 * */

#define CLOCK_45MHZ 45000000UL
#define Baudios_115 115200

void USART_Gpio(void);

void USART_Init(uint32_t baud);

char USART_Rx(void);

void USART_Tx(char data);

/**/
void USART_Tx_Int(void);

void USART_Rx_Int(void);

/**/
void USART_Init_DMA(uint32_t baud);

#endif /* INC_USART_H_ */
