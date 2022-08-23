/*
 * MSE_OS_IRQ.h
 *
 *  Created on: 18 ago. 2022
 *      Author: Wels
 */

#ifndef INC_MSE_OS_IRQ_H_
#define INC_MSE_OS_IRQ_H_

#include "MSE_OS_Core.h"
#include "MSE_OS_API.h"
#include "main.h"

typedef void (*IRQ_UserHandler_t)();

void Os_SetIRQ(IRQn_Type valor_IRQ, IRQ_UserHandler_t IRQ_UserHandler);

void OS_ClearIRQ(IRQn_Type valor_IRQ);

#endif /* INC_MSE_OS_IRQ_H_ */
