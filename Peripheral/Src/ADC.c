/*
 * ADC.c
 *
 *  Created on: Jul 1, 2022
 *      Author: Wels
 */
#include "ADC.h"
#include "SysTick.h"

void ADC_Init(void)
{
	//PA0
	// GPIO
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER |= GPIO_MODER_MODE0;// ANALÓGICO
	//CLOCK ADC
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	// 16 PINES ANÁLOGOS
	// ALINEACIÓN DERECHA
	ADC1->CR2 &= ~(ADC_CR2_ALIGN);
	// SINGLE
	ADC1->CR2 &= ~(ADC_CR2_CONT);
	// RESOLUCIÓN 12 BITS
	ADC1->CR1 &= ~(ADC_CR1_RES);
	// FCLK = 84MHz/2 = 42MHz
	ADC->CCR &= ~(ADC_CCR_ADCPRE);
	// 3 cycles + 12 cycles = 15 cycles
	ADC1->SMPR2 &= ~(ADC_SMPR2_SMP0);
	// CONVERSIONES 1
	ADC1->SQR1 &= ~(ADC_SQR1_L);
	// CANAL
	ADC1->SQR3 = 0;
//	ADC1->SQR3 = ADC_SQR3_SQ2_0;
	//ADC1->SQR3 = ADC_SQR3_SQ3_2;
	// ADC ON
	ADC1->CR2 |= ADC_CR2_ADON;
	// SWSTART
	ADC1->CR2 |= ADC_CR2_SWSTART;
	// DELAY 10MS
	Delay_ms(10);
}

void ADC_Start(void){
	ADC1->SR = 0;// limpiar flag
	ADC1->CR2 |= ADC_CR2_ADON;
	ADC1->CR2 |= ADC_CR2_SWSTART;
}

void ADC_Stop(void)
{
	ADC1->SR = 0;// limpiar flag
	ADC1->CR2 &= ~(ADC_CR2_SWSTART);
}

bool ADC_PollEndConversion(uint32_t timeout)
{
	uint32_t starTime = msGetTicks();
	while((ADC1->SR & ADC_SR_EOC) == 0)
	{
		if((msGetTicks() - starTime)  > timeout)
		{
			return false;
		}
	}
	return true;
}

uint16_t ADC_Read(void)
{
	return ((ADC1->DR) & ADC_DR_DATA);// 12 bits
}

/*
 * ADC INTERRUPT
 * */
void ADC_EnableNVIC(void)
{
	ADC1->CR1 |= ADC_CR1_EOCIE;// INT EOC
	NVIC_SetPriority(ADC_IRQn,5);
	NVIC_EnableIRQ(ADC_IRQn);
}

void ADC_DisableNVIC(void)
{
	ADC1->CR1 &= ~ADC_CR1_EOCIE;// INT EOC
	NVIC_DisableIRQ(ADC_IRQn);
}

/*
 * ADC_Init();
	ADC_EnableNVIC();

	if(flag == 1)
		{
			voltaje = adc_valor*(3.6/4095);
			sprintf(datos,"Vol: %.2f",voltaje);
			LCD_XY(1, 0);
			LCD_Cadena(datos);
			ADC_Start();
		}

	void ADC_IRQHandler(void)
{
	NVIC_ClearPendingIRQ(ADC_IRQn);
	ADC1->SR &= ADC_SR_EOC;// limpiar flag
	adc_valor = ADC_Read();
	flag = 1;
}
 * */

void ADC_Multi(void)
{
	// PA0 - PA1 - PA4
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	// GPIO
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	GPIOA->MODER |= (GPIO_MODER_MODE3 | GPIO_MODER_MODE0 | GPIO_MODER_MODE1);// ANALÓGICO
	// SCAN ENABLE
	ADC1->CR1 |= ADC_CR1_SCAN;
	// TIEMPO DE MUESTREO 0 = 3CYCLES
	ADC1->SMPR2 &= ~(ADC_SMPR2_SMP3 | ADC_SMPR2_SMP1 | ADC_SMPR2_SMP0);
	// CONVERSIONES 3
	ADC1->SQR1 |= (ADC_SQR1_L_1);//2 -> 3 CONVERSIONES
	// CANALES
	ADC1->SQR3 = 0;
	// CONVERSION 1 -> CANAL 0
	// CONVERSION 2 -> CANAL 1
	// CONVERSION 3 -> CANAL 3
	ADC1->SQR3 |= ADC_SQR3_SQ2_0 | ADC_SQR3_SQ3_0 | ADC_SQR3_SQ3_1;
	// ALINEACION Y EOCS
	ADC1->CR2 &= ~(ADC_CR2_ALIGN | ADC_CR2_EOCS);
	// CONVERSION SIMPLE
	ADC1->CR2 &= ~(ADC_CR2_CONT);
	// DMA ENABLE
	ADC1->CR2 |= ADC_CR2_ADON;
	Delay_ms(10);
}

//ADC1->SQR1 &= ~(ADC_SQR1_L);
//		ADC1->SQR3 = 0;
//		ADC_Start();
//		if(ADC_PollEndConversion(10))
//		{
//			datas[0]= ADC_Read();
//			ADC_Stop();
//		}
//		Delay_ms(100);
//		ADC1->SQR1 &= ~(ADC_SQR1_L);
//		ADC1->SQR3 |= ADC_SQR3_SQ1_0;
//		ADC_Start();
//		if(ADC_PollEndConversion(10))
//		{
//			datas[1]= ADC_Read();
//			ADC_Stop();
//		}
//		Delay_ms(100);
//		ADC1->SQR1 &= ~(ADC_SQR1_L);
//		ADC1->SQR3 |= ADC_SQR3_SQ1_1;
//		ADC_Start();
//		if(ADC_PollEndConversion(10))
//		{
//			datas[2]= ADC_Read();
//			ADC_Stop();
//		}
//		//		}
//		printf("Gyro: X=%d, Y=%d, Z=%d\r\n",datas[0],datas[1],datas[2]);
//		printf("----------------------\r\n");
//		Delay_ms(100);

