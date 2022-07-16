/*
 * DMA.c
 *
 *  Created on: 1 jul. 2022
 *      Author: Wels
 */
#include "DMA.h"
#include "SysTick.h"

////USART DMA
//volatile uint8_t msg[] = "https://furkan.space/\n\r";

/* 1 CANAL*/
void ADC_DMAConfig(uint32_t *pData, uint32_t len, uint32_t base)
{
	//CLOCK DMA
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
	// DMA -> 8 STREAM
	DMA2_Stream0->CR = 0;// DESHABILITADO
	// FUENTE PERIFERICO
	DMA2_Stream0->PAR = base;
	// DESTINO
	DMA2_Stream0->M0AR = (uint32_t)pData;// direccion del arreglo
	// LONGITUD
	DMA2_Stream0->FCR = 0;
	DMA2_Stream0->NDTR = len;
	// PERIFERICO A MEMORIA
	DMA2_Stream0->CR &= ~(DMA_SxCR_DIR);
	// TAMAÑO DE MEMORIA Y PERIFERICO 16 BITS
	DMA2_Stream0->CR |= DMA_SxCR_MSIZE_0;
	DMA2_Stream0->CR |= DMA_SxCR_PSIZE_0;
	// MEMORIA INC | PERIFERICO SE MANTIENE
	DMA2_Stream0->CR |= DMA_SxCR_MINC;
	DMA2_Stream0->CR &= ~DMA_SxCR_PINC;
	// MODO NORMAL
	DMA2_Stream0->CR &= ~DMA_SxCR_CIRC;
	// MODO CIRCULAR
	//DMA2_Stream0->CR |= DMA_SxCR_CIRC;
	// TRANSFER COMPLETE
	DMA2_Stream0->CR |= DMA_SxCR_TCIE;
	// HALF COMPLETE
	DMA2_Stream0->CR &= ~(DMA_SxCR_HTIE);
	// PRIORIDAD
	NVIC_SetPriority(DMA2_Stream0_IRQn,3);
	NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	// HABILITAR DMA
	DMA2_Stream0->CR |= DMA_SxCR_EN;
}

void ADC_OneChannle(void)
{
	// PA0
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	// GPIO
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER |= (GPIO_MODER_MODE0);// ANALÓGICO
	// SCAN ENABLE
	ADC1->CR1 &= ~ADC_CR1_SCAN;
	// TIEMPO DE MUESTREO 0 = 3CYCLES
	ADC1->SMPR2 &= ~(ADC_SMPR2_SMP0);
	// CONVERSIONES 1
	ADC1->SQR1 &= ~(ADC_SQR1_L);//2 -> 3 CONVERSIONES
	// CANALES
	ADC1->SQR3 = 0;
	// CONVERSION 1 -> CANAL 0
	// ALINEACION Y EOCS
	ADC1->CR2 &= ~(ADC_CR2_ALIGN | ADC_CR2_EOCS);
	// CONVERSION CONTINUA
	ADC1->CR2 |= (ADC_CR2_CONT);
	// DMA ENABLE
	ADC1->CR2 |= (ADC_CR2_DDS | ADC_CR2_DMA);
	ADC1->CR2 |= ADC_CR2_ADON;
	Delay_ms(10);
}

//void ADC_MultiChannel(void)
//{
//	// PA0 - PA1 - PA4
//	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
//	// GPIO
//	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
//	GPIOA->MODER |= (GPIO_MODER_MODE4 | GPIO_MODER_MODE0 | GPIO_MODER_MODE1);// ANALÓGICO
//	// SCAN ENABLE
//	ADC1->CR1 |= ADC_CR1_SCAN;
//	// TIEMPO DE MUESTREO 0 = 3CYCLES
//	ADC1->SMPR2 &= ~(ADC_SMPR2_SMP4 | ADC_SMPR2_SMP1 | ADC_SMPR2_SMP0);
//	// CONVERSIONES 3
//	ADC1->SQR1 |= (ADC_SQR1_L_1);//2 -> 3 CONVERSIONES
//	// CANALES
//	ADC1->SQR3 = 0;
//	// CONVERSION 1 -> CANAL 0
//	// CONVERSION 2 -> CANAL 1
//	// CONVERSION 3 -> CANAL 4
//	ADC1->SQR3 |= ADC_SQR3_SQ2_0 | ADC_SQR3_SQ3_2;
//	// ALINEACION Y EOCS
//	ADC1->CR2 &= ~(ADC_CR2_ALIGN | ADC_CR2_EOCS);
//	// CONVERSION SIMPLE
//	ADC1->CR2 &= ~(ADC_CR2_CONT);
//	// DMA ENABLE
//	ADC1->CR2 |= (ADC_CR2_DDS | ADC_CR2_DMA);
//	ADC1->CR2 |= ADC_CR2_ADON;
//	Delay_ms(10);
//}

/*3 CANALES*/
//void ADC_DMAConfig(uint32_t *pData, uint32_t len, uint32_t base)
//{
//	//CLOCK DMA
//	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
//	// DMA -> 8 STREAM
//	DMA2_Stream0->CR = 0;// DESHABILITADO
//	// FUENTE PERIFERICO
//	DMA2_Stream0->PAR = base;
//	// DESTINO
//	DMA2_Stream0->M0AR = (uint32_t)pData;// direccion del arreglo
//	// LONGITUD
//	DMA2_Stream0->FCR = 0;
//	DMA2_Stream0->NDTR = len;
//	// PERIFERICO A MEMORIA
//	DMA2_Stream0->CR &= ~(DMA_SxCR_DIR);
//	// TAMAÑO DE MEMORIA Y PERIFERICO 16 BITS
//	DMA2_Stream0->CR |= DMA_SxCR_MSIZE_0;
//	DMA2_Stream0->CR |= DMA_SxCR_PSIZE_0;
//	// MEMORIA INC | PERIFERICO SE MANTIENE
//	DMA2_Stream0->CR |= DMA_SxCR_MINC;
//	DMA2_Stream0->CR &= ~DMA_SxCR_PINC;
//	// MODO CIRCULAR
//	DMA2_Stream0->CR |= DMA_SxCR_CIRC;
//	// TRANSFER COMPLETE
//	DMA2_Stream0->CR |= DMA_SxCR_TCIE;
//	// HALF COMPLETE
//	DMA2_Stream0->CR &= ~(DMA_SxCR_HTIE);
//	// PRIORIDAD
//	NVIC_SetPriority(DMA2_Stream0_IRQn,3);
//	NVIC_EnableIRQ(DMA2_Stream0_IRQn);
//	// HABILITAR DMA
//	DMA2_Stream0->CR |= DMA_SxCR_EN;
//}

/*
 *
 *#define ARRAY_SIZE 500

uint16_t adcBuffer[ARRAY_SIZE] = {0};

uint32_t flag_dma = 0;
 *
 *	ADC_OneChannle();
	ADC_DMAConfig(adcBuffer, ARRAY_SIZE, (uint32_t)&ADC1->DR);
	ADC_Start();
 * if(flag_dma == 1)
		{
			for(uint16_t i = 0; i < ARRAY_SIZE; i++)
			{
				printf("ADC BUFFER[%d]: %d \r\n", i,adcBuffer[i]);
			}
			printf("--------------\r\n");
			Delay_ms(500);
			ADC_Start();
			flag_dma = 0;
		}


		void DMA2_Stream0_IRQHandler(void){
	if(DMA2->LISR & DMA_LISR_TCIF0)
	{
		NVIC_ClearPendingIRQ(DMA2_Stream0_IRQn);
		DMA2->LIFCR |= DMA_LIFCR_CTCIF0;
		flag_dma = 1;
		ADC_Stop();
	}
	if(DMA2->LISR & DMA_LISR_HTIF0)
	{
		DMA2->LIFCR |= DMA_LIFCR_CHTIF0;
	}

}
 * */


/**/
// usart dma
//void USART_DMAConfig(uint32_t *pData, uint32_t len, uint32_t base)
//{
//	//CLOCK DMA
//	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
//	// DMA -> 8 STREAM
//	DMA1_Stream3->CR = 0;// DESHABILITADO
//	while(DMA1_Stream6->CR & (1 << 0));
//	// Channel
//	DMA1_Stream3->CR = 4<<DMA_SxCR_CHSEL_Pos;
//	// FUENTE PERIFERICO
//	DMA1_Stream3->PAR = base;
//	// DESTINO
//	DMA1_Stream3->M0AR = (uint32_t)pData;// direccion del arreglo
//	// LONGITUD
//	DMA1_Stream3->FCR = 0;
//	DMA1_Stream3->NDTR = len;
//	// MEMORIA A PERIFERICO
//	DMA1_Stream3->CR |= DMA_SxCR_DIR_0;
//	// TAMAÑO DE MEMORIA Y PERIFERICO 16 BITS
//	DMA1_Stream3->CR &= ~(DMA_SxCR_MSIZE);
//	DMA1_Stream3->CR &= ~(DMA_SxCR_PSIZE);
//	// MEMORIA INC | PERIFERICO SE MANTIENE
//	DMA1_Stream3->CR |= DMA_SxCR_MINC;
//	DMA1_Stream3->CR &= ~DMA_SxCR_PINC;
//	// MODO NORMAL
//	DMA1_Stream3->CR &= ~DMA_SxCR_CIRC;
//	// MODO CIRCULAR
////	DMA2_Stream0->CR |= DMA_SxCR_CIRC;
//	// TRANSFER COMPLETE
//	DMA1_Stream3->CR |= DMA_SxCR_TCIE;
//	// PRIORIDAD
//	DMA1_Stream3->CR |= 2<<16;
//	// HALF COMPLETE
//	DMA1_Stream3->CR &= ~(DMA_SxCR_HTIE);
//	// PRIORIDAD
//	NVIC_SetPriority(DMA1_Stream3_IRQn,4);
//	NVIC_EnableIRQ(DMA1_Stream3_IRQn);
//	// HABILITAR DMA
//	DMA1_Stream3->CR |= DMA_SxCR_EN;
//}
//
//void DMA1_Stream3_IRQHandler(void)
//{
//    // clear stream transfer complete interrupt - bit21 for stream 6
//    if (DMA1->LISR & DMA_LISR_TCIF3 ) {
//        // clear interrupt
//        DMA1->LIFCR |= DMA_LIFCR_CTCIF3;
//        // clean all LEDs
//        write_LED1(HIGH);
//    }
//}
//uint8_t msg[27] = "https://welstheory.com/\n\r";
//USART_Init_DMA(Baudios_115);
//	USART_DMAConfig(msg, 27, (uint32_t)&USART3->DR);



