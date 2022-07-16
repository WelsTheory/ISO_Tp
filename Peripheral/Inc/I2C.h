/*
 * I2C.h
 *
 *  Created on: 1 jul. 2022
 *      Author: Wels
 */

#ifndef INC_I2C_H_
#define INC_I2C_H_

#include "main.h"

void I2C_Gpio(void);

void I2C_Init(void);

bool I2C_SlaveExist(uint8_t address, uint32_t timeout);

bool I2C_masterTransmit(uint8_t address, uint8_t *pData, uint32_t len, uint32_t timeout);

bool I2C_masterReceive(uint8_t address, uint8_t *pData, uint32_t len, uint32_t timeout);


#endif /* INC_I2C_H_ */
