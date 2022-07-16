/*
 * GPIO.h
 *
 *  Created on: Jul 1, 2022
 *      Author: Wels
 */

#ifndef INC_GPIO_H_
#define INC_GPIO_H_

#include "main.h"

/*
 * 	PB0 -> LD1
 * 	PB7 -> LD2
 * 	PB14 -> LD3
 * 	PC13 -> INPUT
 * */

typedef enum{
	LOW,
	HIGH
}GPIO_STATE;

void GPIO_Leds_Init(void);

void write_LED1(GPIO_STATE state);

void toggle_LED1(void);

void write_LED2(GPIO_STATE state);

void toggle_LED2(void);

void write_LED3(GPIO_STATE state);

void toggle_LED3(void);

GPIO_STATE read_BUTTON(void);
void GPIO_Button_Init(void);

#endif /* INC_GPIO_H_ */
