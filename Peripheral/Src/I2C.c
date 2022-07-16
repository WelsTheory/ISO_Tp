/*
 * I2C.c
 *
 *  Created on: 1 jul. 2022
 *      Author: Wels
 */
#include "I2C.h"
#include "SysTick.h"

void I2C_Gpio(void)
{
	// PB6 Y PB9
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	// FUNCIONES ALTERNAS
	GPIOB->MODER &= ~(GPIO_MODER_MODE8 | GPIO_MODER_MODE9);
	GPIOB->MODER |= (GPIO_MODER_MODE8_1 | GPIO_MODER_MODE9_1);
	// AF4
	GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL8);
	GPIOB->AFR[1] |= (GPIO_AFRH_AFSEL8_2);
	GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL9);
	GPIOB->AFR[1] |= (GPIO_AFRH_AFSEL9_2);
	// PULL UP
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD8 | GPIO_PUPDR_PUPD9);
	GPIOB->PUPDR |= (GPIO_PUPDR_PUPD8_0 | GPIO_PUPDR_PUPD9_0);
	// OPEN DRAIN
	GPIOB->OTYPER |= (GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9);
	// VERY HIGH SPEED
	GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR8 | GPIO_OSPEEDER_OSPEEDR9);
}

void I2C_Init(void)
{
	I2C_Gpio();
	// I2C1 ENABLE
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	// RESET
	I2C1->CR1 |= I2C_CR1_SWRST;
	I2C1->CR1 &= ~I2C_CR1_SWRST;
//	// SM
//	I2C1->CCR &= ~(I2C_CCR_FS);
	// THIGH Y TLOW - 5000nS
//	I2C1->CCR |= 225<<0;//45MHZ
	// Trise
//		I2C1->TRISE = 46;
	// 400 KHz
	I2C1->CCR |= I2C_CCR_FS;
	I2C1->CCR |= 56<<0;
	// Trise
	I2C1->TRISE = 10;
	// CLOCK
	I2C1->CR2 |= 45<<I2C_CR2_FREQ_Pos;
	// HABILITAR
	I2C1->CR1 |= I2C_CR1_PE;
}

bool I2C_SlaveExist(uint8_t address, uint32_t timeout)
{
	uint32_t starTime = msGetTicks();
	while(I2C1->SR2 & I2C_SR2_BUSY)
	{
		// OCUPADO EL BUS
		if((msGetTicks() - starTime)  > timeout)
		{
			return false;
		}
	}
	I2C1->CR1 &= ~(I2C_CR1_POS);
	I2C1->CR1 |= I2C_CR1_START;
	while((I2C1->SR1 & I2C_SR1_SB) == 0)
	{
		if((msGetTicks() - starTime)  > timeout)
		{
			return false;
		}
	}
	// ADDRESS
	I2C1->DR = address;// ADDRESS + R/W
	while((I2C1->SR1 & I2C_SR1_ADDR) == 0)
	{
		if((msGetTicks() - starTime)  > timeout)
		{
			return false;
		}
	}
	I2C1->CR1 |= I2C_CR1_STOP;// STOP
	uint32_t temp = I2C1->SR1 | I2C1->SR2;
	(void)temp;// no warning
	while(I2C1->SR2 & I2C_SR2_BUSY)
	{
		// OCUPADO EL BUS
		if((msGetTicks() - starTime)  > timeout)
		{
			return false;
		}
	}
	return true;
}

bool I2C_masterTransmit(uint8_t address, uint8_t *pData, uint32_t len, uint32_t timeout)
{
	uint32_t starTime = msGetTicks();
	while(I2C1->SR2 & I2C_SR2_BUSY)
	{
		// OCUPADO EL BUS
		if((msGetTicks() - starTime)  > timeout)
		{
			return false;
		}
	}
	I2C1->CR1 &= ~(I2C_CR1_POS);
	I2C1->CR1 |= I2C_CR1_START;
	while((I2C1->SR1 & I2C_SR1_SB) == 0)
	{
		if((msGetTicks() - starTime)  > timeout)
		{
			return false;
		}
	}
	// ADDRESS
	I2C1->DR = address;// ADDRESS + R/W
	while((I2C1->SR1 & I2C_SR1_ADDR) == 0)
	{
		if((msGetTicks() - starTime)  > timeout)
		{
			return false;
		}
	}
	uint32_t temp = I2C1->SR1 | I2C1->SR2;
	(void)temp;// no warning
	uint32_t dataIdx = 0;
	uint32_t dataSize = len;
	// ENVIAR 1 DATO
	// TXE Y BTF
	while(dataSize > 0)
	{
		while((I2C1->SR1 & I2C_SR1_TXE) == 0)
		{
			if((msGetTicks() - starTime)  > timeout)
			{
				return false;
			}
		}
		I2C1->DR = pData[dataIdx];//0 1 9
		while((I2C1->SR1 & I2C_SR1_BTF) == 0)
		{
			if((msGetTicks() - starTime)  > timeout)
			{
				return false;
			}
		}
		// envio el primer dato
		dataSize--;
		dataIdx++;
	}
	I2C1->CR1 |= I2C_CR1_STOP;// STOP
	temp = I2C1->SR1 | I2C1->SR2;
	(void)temp;// no warning
	while(I2C1->SR2 & I2C_SR2_BUSY)
	{
		// OCUPADO EL BUS
		if((msGetTicks() - starTime)  > timeout)
		{
			return false;
		}
	}
	return true;
}

bool I2C_masterReceive(uint8_t address, uint8_t *pData, uint32_t len, uint32_t timeout)
{
	uint32_t starTime = msGetTicks();
	while(I2C1->SR2 & I2C_SR2_BUSY)
	{
		// OCUPADO EL BUS
		if((msGetTicks() - starTime)  > timeout)
		{
			return false;
		}
	}
	I2C1->CR1 &= ~(I2C_CR1_POS);
	I2C1->CR1 |= I2C_CR1_ACK;
	I2C1->CR1 |= I2C_CR1_START;
	while((I2C1->SR1 & I2C_SR1_SB) == 0)
	{
		if((msGetTicks() - starTime)  > timeout)
		{
			return false;
		}
	}
	// ADDRESS
	I2C1->DR = address | 0x01;// ADDRESS + R/W
	while((I2C1->SR1 & I2C_SR1_ADDR) == 0)
	{
		if((msGetTicks() - starTime)  > timeout)
		{
			return false;
		}
	}
	uint32_t temp = I2C1->SR1 | I2C1->SR2;
	(void)temp;// no warning
	if(len == 0)
	{
		uint32_t temp = I2C1->SR1 | I2C1->SR2;
		(void)temp;// no warning
		I2C1->CR1 |= I2C_CR1_STOP;
		return false;
	}
	else if(len == 1)
	{
		I2C1->CR1 &= ~(I2C_CR1_ACK);
		temp = I2C1->SR1 | I2C1->SR2;
		(void)temp;// no warning
		I2C1->CR1 |= I2C_CR1_STOP;
	}
	else if(len == 2)
	{
		I2C1->CR1 &= ~(I2C_CR1_ACK);
		I2C1->CR1 |= I2C_CR1_POS;
		temp = I2C1->SR1 | I2C1->SR2;
		(void)temp;// no warning
	}
	else
	{
		I2C1->CR1 |= (I2C_CR1_ACK);
		temp = I2C1->SR1 | I2C1->SR2;
		(void)temp;// no warning
	}
	uint32_t dataIdx = 0;
	uint32_t dataSize = len;
	while(dataSize > 0)
	{
		if(dataSize <= 3)
		{
			if(dataSize == 1)
			{
				while((I2C1->SR1 & I2C_SR1_RXNE) == 0)
				{
					if((msGetTicks() - starTime)  > timeout)
					{
						return false;
					}
				}
				pData[dataIdx] = I2C1->DR;
				dataSize--;
				dataIdx++;
			}
			else if(dataSize == 2)
			{
				while((I2C1->SR1 & I2C_SR1_BTF) == 0)
				{
					if((msGetTicks() - starTime)  > timeout)
					{
						return false;
					}
				}
				I2C1->CR1 |= I2C_CR1_STOP;
				// DATO 2
				pData[dataIdx] = I2C1->DR;
				dataSize--;
				dataIdx++;
				// DATO 2
				pData[dataIdx] = I2C1->DR;
				dataSize--;
				dataIdx++;
			}
			else
			{
				while((I2C1->SR1 & I2C_SR1_BTF) == 0)
				{
					if((msGetTicks() - starTime)  > timeout)
					{
						return false;
					}
				}
				I2C1->CR1 &= ~(I2C_CR1_ACK);
				// DATO 2
				pData[dataIdx] = I2C1->DR;
				dataSize--;
				dataIdx++;
				while((I2C1->SR1 & I2C_SR1_BTF) == 0)
				{
					if((msGetTicks() - starTime)  > timeout)
					{
						return false;
					}
				}
				I2C1->CR1 |= I2C_CR1_STOP;
				// DATO 2
				pData[dataIdx] = I2C1->DR;
				dataSize--;
				dataIdx++;
				// DATO 2
				pData[dataIdx] = I2C1->DR;
				dataSize--;
				dataIdx++;
			}
		}
		else
		{
			while((I2C1->SR1 & I2C_SR1_RXNE) == 0)
			{
				if((msGetTicks() - starTime)  > timeout)
				{
					return false;
				}
			}
			pData[dataIdx] = I2C1->DR;
			dataSize--;
			dataIdx++;
			while((I2C1->SR1 & I2C_SR1_BTF) == 0)
			{
				if((msGetTicks() - starTime)  > timeout)
				{
					return false;
				}
			}
		}
	}
	return true;
}

