/*
 * USART.c
 *
 *  Created on: Jul 1, 2022
 *      Author: Wels
 */
#include "USART.h"

//const uint8_t msg[] = "https://furkan.space/\n\r";
//
//#define valor_rx 10
//volatile char msg_rx[valor_rx];
//
//volatile int bufpos = 0;
//
//volatile uint8_t variable_flag = 0;

void USART_Gpio(void)
{
	/*
	 * PD8 -> RX
	 * PD9 -> TX
	 * */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	// AF
	GPIOD->MODER &= ~(GPIO_MODER_MODE8);
	GPIOD->MODER &= ~(GPIO_MODER_MODE9);
	GPIOD->MODER |= (GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1);
	// PUPDR
	GPIOD->OTYPER &= ~(GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9);
	// PUPDR
	GPIOD->PUPDR &= ~(GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9);
	// SPEED
	GPIOD->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED8 | GPIO_OSPEEDR_OSPEED9);
	// AF7 -> TX Y RX
	// AFRH AFRL
	GPIOD->AFR[1] &= ~(GPIO_AFRH_AFSEL8 | GPIO_AFRH_AFSEL9);
	GPIOD->AFR[1] |= (GPIO_AFRH_AFSEL8_2 | GPIO_AFRH_AFSEL8_1 | GPIO_AFRH_AFSEL8_0);
	GPIOD->AFR[1] |= (GPIO_AFRH_AFSEL9_2 | GPIO_AFRH_AFSEL9_1 | GPIO_AFRH_AFSEL9_0);
}

void USART_Init(uint32_t baud)
{
	USART_Gpio();
	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	// BRR = (2*clock + baudios)/2*baudios
	USART3->BRR = (2*CLOCK_45MHZ+baud)/(2*baud);
	// 8 BITS
	USART3->CR1 &= ~(USART_CR1_M);
	// PARIDAD DESHABILITADA
	USART3->CR1 &= ~(USART_CR1_PCE);
	//
	USART3->CR1 &= ~(USART_CR1_PS);
	// STOP
	USART3->CR2 &= ~(USART_CR2_STOP);
	// FLOW CONTROL DESAHBILITADO
	USART3->CR3 &= ~(USART_CR3_CTSE | USART_CR3_RTSE);
	// FULL DUPLEX
	USART3->CR3 &= ~(USART_CR3_HDSEL);
	// HABILITAR TX Y RX
	USART3->CR1 |= (USART_CR1_TE | USART_CR1_RE);
	// USART ENABLE
	USART3->CR1 |= USART_CR1_UE;
}

void USART_Tx(char data){
	while((USART3->SR & USART_SR_TC) == 0);
	USART3->DR = (data & 0xFF);//1FF
}

char USART_Rx(void)
{
	while((USART3->SR & USART_SR_RXNE) == 0);
	return (USART3->DR & 0xFF);
}

// 1- DONDE ALMACENAS
// 2  CUANTA INFORMACIÓN
// 3  TIMEOUT
// char datos[10]
//bool USART_Rx_Cadena(uint8_t *data, uint8_t len,uint32_t timeout)
//{
//	uint8_t data_received = len;//10
//	uint32_t starTime = msGetTicks();
//	while(data_received > 0)
//	{
//		if(USART2->SR & USART_SR_RXNE)
//		{
//			*data++ = USART2->DR & 0xFF;
//			data_received--;
//		}
//		else
//		{
//			if((msGetTicks() - starTime)  > timeout)
//			{
//				return false;
//			}
//		}
//	}
//	return true;
//}

/**/
void USART_Tx_Int(void)
{

	NVIC_SetPriority(USART3_IRQn,3);
	NVIC_EnableIRQ(USART3_IRQn);
	USART3->CR1 |= USART_CR1_TXEIE;
}

void USART_Rx_Int(void)
{
	NVIC_SetPriority(USART3_IRQn,3);
	NVIC_EnableIRQ(USART3_IRQn);
	USART3->CR1 |= USART_CR1_RXNEIE;
	USART3->CR1 &= ~(USART_CR1_TXEIE);
}

//void USART3_IRQHandler(void)
//{
	//TX
	// check if the source is transmit interrupt
//	if (USART3->SR & USART_SR_TXE) {
//
//		if (bufpos == sizeof(msg)) {
//			// buffer is flushed out, disable tx interrupt
//			bufpos = 0;
//			USART3->CR1 &= ~(USART_CR1_TXEIE);
//		}
//		else {
//			// flush ot the next char in the buffer
//			USART3->DR = msg[bufpos++];
//		}
//	}
	//RX
//	if (USART3->SR & USART_SR_RXNE) {
//
//		if (bufpos == valor_rx) {
//			// buffer is flushed out, disable tx interrupt
//			bufpos = 0;
//			variable_flag = 1;
//			USART3->CR1 &= ~(USART_CR1_RXNEIE);
//		}
//		else {
//			// flush ot the next char in the buffer
//			msg_rx[bufpos++] = USART3->DR;
//			//USART3->DR = msg[bufpos++];
//		}
//	}
//}

/*DMA
 * */
void USART_Init_DMA(uint32_t baud)
{
	USART_Gpio();
	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	// BRR = (2*clock + baudios)/2*baudios
	USART3->BRR = (2*CLOCK_45MHZ+baud)/(2*baud);
	// USART DMA
	USART3->CR3 |= USART_CR3_DMAT;
	// 8 BITS
	USART3->CR1 &= ~(USART_CR1_M);
	// PARIDAD DESHABILITADA
	USART3->CR1 &= ~(USART_CR1_PCE);
	//
	USART3->CR1 &= ~(USART_CR1_PS);
	// STOP
	USART3->CR2 &= ~(USART_CR2_STOP);
	// FLOW CONTROL DESAHBILITADO
	USART3->CR3 &= ~(USART_CR3_CTSE | USART_CR3_RTSE);
	// FULL DUPLEX
	USART3->CR3 &= ~(USART_CR3_HDSEL);
	// HABILITAR TX Y RX
	USART3->CR1 |= (USART_CR1_TE | USART_CR1_RE);
	// USART ENABLE
	USART3->CR1 |= USART_CR1_UE;
	USART3->SR &= ~(USART_SR_TC);
}




