/*
 * SPI.h
 *
 *  Created on: Jul 1, 2022
 *      Author: Wels
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_

#include "main.h"

void SPI_GPIO(void);

void SPI_Init(void);

void SPI_CS(bool state);

bool SPI_Transmit(uint8_t *pData, uint32_t len, uint32_t timeout);

bool SPI_TransmitReceive(uint8_t *pData, uint8_t *rxData, uint32_t len, uint32_t timeout);

bool SPI_Receive(uint8_t *rxData, uint32_t len, uint32_t timeout);


#endif /* INC_SPI_H_ */
