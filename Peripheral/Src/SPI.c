/*
 * SPI.c
 *
 *  Created on: Jul 1, 2022
 *      Author: Wels
 */
#include "SPI.h"
#include "SysTick.h"

void SPI_GPIO(void)
{
	/*
	 *  PC11  -> MISO
		PC12  -> MOSI
		PC10 -> SCK
		PC9  -> CS
	 * */
	RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOCEN);
	GPIOC->MODER &= ~(GPIO_MODER_MODE10 | GPIO_MODER_MODE11 | GPIO_MODER_MODE12);
	GPIOC->MODER |= ( GPIO_MODER_MODE10_1 | GPIO_MODER_MODE11_1 |GPIO_MODER_MODE12_1);// GPIO SALIDA

	//gpio PC9
	GPIOC->MODER &= ~(GPIO_MODER_MODE9);
	GPIOC->MODER |= (GPIO_MODER_MODE9_0);

	// AF5 -> PB10 | PC2 | PC3
	GPIOC->AFR[1] &= ~(GPIO_AFRH_AFSEL10 | GPIO_AFRH_AFSEL11 | GPIO_AFRH_AFSEL12);
	GPIOC->AFR[1] |= GPIO_AFRH_AFSEL10_0 | GPIO_AFRH_AFSEL10_2;
	GPIOC->AFR[1] |= GPIO_AFRH_AFSEL11_0 | GPIO_AFRH_AFSEL11_2;
	GPIOC->AFR[1] |= GPIO_AFRH_AFSEL12_0 | GPIO_AFRH_AFSEL12_2;

	// PUSH PULL
	GPIOC->OTYPER &= ~(GPIO_OTYPER_OT10 | GPIO_OTYPER_OT11 | GPIO_OTYPER_OT12 | GPIO_OTYPER_OT9);
	// PULL UP
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD10 | GPIO_PUPDR_PUPD11 | GPIO_PUPDR_PUPD12 | GPIO_PUPDR_PUPD9);
	// VERY HIGH SPEED PC2 PC3 PB10 - LOW SPEED PB6
	GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR10 | GPIO_OSPEEDER_OSPEEDR11 | GPIO_OSPEEDER_OSPEEDR12 | GPIO_OSPEEDER_OSPEEDR9;

	GPIOC->ODR |= (1UL<<9);
}

void SPI_Init(void)
{
	SPI_GPIO();
	// CONFIGURACION DEL SPI
	RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
	SPI3->CR1 = 0;
	// BAUDIOS -> 45MHz/128
	SPI3->CR1 &= ~(SPI_CR1_BR);
	SPI3->CR1 |= 6<<SPI_CR1_BR_Pos;//(SPI_CR1_BR_2 | SPI_CR1_BR_1);
	// POLARIDAD DE RELOJ 0
	SPI3->CR1 &= ~(SPI_CR1_CPOL);
	// FASE DE RELOJ
	SPI3->CR1 &= ~(SPI_CR1_CPHA);
	// MASTER
	SPI3->CR1 |= SPI_CR1_MSTR;
	// MSB FIRST
	SPI3->CR1 &= ~(SPI_CR1_LSBFIRST);
	// FULL DUPLEX
	SPI3->CR1 &= ~(SPI_CR1_RXONLY);
	// 8 BITS
	SPI3->CR1 &= ~(SPI_CR1_DFF);
	// SELECTOR
	SPI3->CR1 |= (SPI_CR1_SSM | SPI_CR1_SSI);
	// HABILITAR SPI
	SPI3->CR1 |= SPI_CR1_SPE;
	// FLAG LIMPIAR
	uint32_t temp = SPI3->SR;
	(void)temp;
}

void SPI_CS(bool state)
{
	if(state == 1){
		GPIOC->ODR |= GPIO_ODR_OD9;
	}
	else{
		GPIOC->ODR &= ~(GPIO_ODR_OD9);
	}

}

bool SPI_Transmit(uint8_t *pData, uint32_t len, uint32_t timeout)
{
	SPI3->CR1 &= ~SPI_CR1_SPE;
	SPI3->CR1 |= SPI_CR1_SPE;
	uint32_t startTime = msGetTicks();
	uint32_t dataIdx = 0;
	uint32_t dataSize = 0;//10
	uint8_t data = pData[dataIdx];
	// BUSY
	if(SPI3->SR & SPI_SR_BSY)
	{
		if((msGetTicks() - startTime) > timeout){
			return false;
		}
	}
	while(dataSize < len)
	{
		if(SPI3->SR & SPI_SR_TXE)
		{
			//uint32_t temp = SPI2->DR;
			//SPI2->DR &= ~(SPI_DR_DR_Msk);
			SPI3->DR |= (uint8_t)data;
			dataIdx++;
			dataSize++;
		}
		else
		{
			if((msGetTicks() - startTime) > timeout){
				return false;
			}
		}

	}
	// BUSY
	if(SPI3->SR & SPI_SR_BSY)
	{
		if((msGetTicks() - startTime) > timeout){
			return false;
		}
	}
	uint32_t temp = SPI3->DR;
	temp = SPI3->SR;
	(void)temp;
	return true;
}

bool SPI_TransmitReceive(uint8_t *pData, uint8_t *rxData, uint32_t len, uint32_t timeout)
{
	SPI3->CR1 |= SPI_CR1_SPE;
	uint32_t startTime = msGetTicks();
	uint32_t dataIdx = 0;
	uint32_t dataSize = 0;//10
	uint32_t flag_spi = 1;
	while(dataSize < len)
	{
		if((SPI3->SR & SPI_SR_TXE) && flag_spi)
		{
			SPI3->DR = pData[dataIdx];
			flag_spi = 0;
		}
		if(SPI3->SR & SPI_SR_RXNE)
		{
			rxData[dataIdx] = SPI3->DR;
			dataIdx++;
			dataSize++;
			flag_spi = 1;
		}
		else
		{
			if((msGetTicks() - startTime) > timeout){
				return false;
			}
		}

	}
	// BUSY
	if(SPI3->SR & SPI_SR_BSY)
	{
		if((msGetTicks() - startTime) > timeout){
			return false;
		}
	}
	uint32_t temp = SPI3->DR;
	temp = SPI3->SR;
	(void)temp;
	return true;
}

bool SPI_Receive(uint8_t *rxData, uint32_t len, uint32_t timeout)
{
	SPI3->CR1 |= SPI_CR1_SPE;
	uint32_t startTime = msGetTicks();
	uint32_t dataIdx = 0;
	uint32_t dataSize = 0;//10
	while(dataSize < len)
	{
		if(SPI3->SR & SPI_SR_RXNE)
		{
			rxData[dataIdx] = SPI3->DR;
			dataIdx++;
			dataSize++;
		}
		else
		{
			if((msGetTicks() - startTime) > timeout){
				return false;
			}
		}

	}
	// BUSY
	if(SPI3->SR & SPI_SR_BSY)
	{
		if((msGetTicks() - startTime) > timeout){
			return false;
		}
	}
	uint32_t temp = SPI3->DR;
	temp = SPI3->SR;
	(void)temp;
	return true;
}

/*
 *
 * #include "fatfs.h"

FRESULT fresult;
FATFS fs;
FIL fil;
DWORD freeClusters;
FATFS *pFatFS;
uint32_t total, free_space;

UINT nbyte;
 *
 *
 *MX_FATFS_Init();
	fresult = f_mount(&fs, "", 1);
	if(fresult != FR_OK){
		printf("ERROR EN LA SD\r\n");
	}
	else
	{
		f_getfree("",&freeClusters, &pFatFS);
		total = (pFatFS->n_fatent - 2 )*pFatFS->csize;
		free_space = freeClusters*pFatFS->csize;
		printf("%lu KB Total drive spacec.\r\n%lu KB available\r\n",total/2,free_space/2);
		printf("SD CARD EXITOSA\r\n");
		fresult = f_open(&fil, "WELS.txt", FA_WRITE | FA_CREATE_NEW );
		if(fresult != FR_OK){
			printf("NO SE ABRIO EL ARCHIVO\r\n");
		}
		else
		{
			printf("SE CREO EL ARCHIVO\r\n");
			fresult = f_write(&fil, "PICTRANCE ACA NO QUEMAMOS SD CARD :D \r\n hola amigos \r\n", 58, &nbyte);
			if(fresult != FR_OK){
				printf("NO SE GUARDO LA INFORMACION\r\n");
			}
			else
			{
				if(nbyte == 16)
				{
					toggle_LED2();
				}
				printf("EUREKA\r\n");
				f_close(&fil);
			}
		}

	}
 *
 * */

