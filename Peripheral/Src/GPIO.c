/*
 * GPIO.c
 *
 *  Created on: Jul 1, 2022
 *      Author: Wels
 */
#include "GPIO.h"

void GPIO_Leds_Init(void)
{
	// Enable Port B clock
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	// Mode to Output
	GPIOB->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE7 | GPIO_MODER_MODE14);
	GPIOB->MODER |= GPIO_MODER_MODE0_0 | GPIO_MODER_MODE7_0 | GPIO_MODER_MODE14_0;
	// Push Pull
	GPIOB->OTYPER &= ~(GPIO_OTYPER_OT0 | GPIO_OTYPER_OT7 | GPIO_OTYPER_OT14);
	// SPEED
	GPIOB->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR0 | GPIO_OSPEEDER_OSPEEDR7 | GPIO_OSPEEDER_OSPEEDR14);
	// NO PULL UP PULL DOWN
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD0 | GPIO_PUPDR_PUPD7 | GPIO_PUPDR_PUPD14);
	// ESCRITURA
	GPIOB->ODR &= ~(GPIO_ODR_OD0 | GPIO_ODR_OD7 | GPIO_ODR_OD14);
}

void write_LED1(GPIO_STATE state)
{
	if(state)
	{
		GPIOB->ODR |= (GPIO_ODR_OD0);
	}
	else
	{
		GPIOB->ODR &= ~(GPIO_ODR_OD0);
	}

}

void toggle_LED1(void)
{
	GPIOB->ODR ^= (GPIO_ODR_OD0);
}

void write_LED2(GPIO_STATE state)
{
	if(state)
	{
		GPIOB->ODR |= (GPIO_ODR_OD7);
	}
	else
	{
		GPIOB->ODR &= ~(GPIO_ODR_OD7);
	}

}

void toggle_LED2(void)
{
	GPIOB->ODR ^= (GPIO_ODR_OD7);
}

void write_LED3(GPIO_STATE state)
{
	if(state)
	{
		GPIOB->ODR |= (GPIO_ODR_OD14);
	}
	else
	{
		GPIOB->ODR &= ~(GPIO_ODR_OD14);
	}

}

void toggle_LED3(void)
{
	GPIOB->ODR ^= (GPIO_ODR_OD14);
}

GPIO_STATE read_BUTTON(void)
{
	if((GPIOC->IDR & GPIO_IDR_ID13)){
		return HIGH;
	}
	else
	{
		return LOW;
	}
}

void GPIO_Button_Init(void)
{
	// Enable Port C clock PIN 13
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	// Mode to INPUT
	//	GPIOC->MODER &= ~(GPIO_MODER_MODE13);
	GPIOC->MODER &= ~(GPIO_MODER_MODE13);
	//GPIOC->MODER |= (GPIO_MODER_MODE13_1);
	// SPEED
	GPIOC->OSPEEDR &= ~(GPIO_OSPEEDER_OSPEEDR13);
	// PULL UP no PULL DOWN
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD13);
	//GPIOC->PUPDR |= (GPIO_PUPDR_PUPD13_0);
}


