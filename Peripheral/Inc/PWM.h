/*
 * PWM.h
 *
 *  Created on: Jul 1, 2022
 *      Author: Wels
 */

#ifndef INC_PWM_H_
#define INC_PWM_H_

#include "main.h"

void PWM_Gpio(void);

void PWM_Init(void);

/**/
void PWM_ServoMotor(void);

void PWM_SetDutyServo(uint16_t duty);

#endif /* INC_PWM_H_ */
