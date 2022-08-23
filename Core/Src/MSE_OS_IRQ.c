///*
// * MSE_OS_IRQ.c
// *
// *  Created on: 18 ago. 2022
// *      Author: Wels
// */
//#include "MSE_OS_IRQ.h"
//
//static void* isr_user_handler_vector[OS_NUMBER_OF_IRQ];				//vector de punteros a funciones para nuestras interrupciones
//
//bool os_insertIRQ(LPC43XX_IRQn_Type irq, void* isr_user_handler)
//{
//	bool result = false;
//
//	if (isr_user_handler_vector[irq] == NULL)
//	{
//		isr_user_handler_vector[irq] = isr_user_handler;
//		NVIC_ClearPendingIRQ(irq);
//		NVIC_EnableIRQ(irq);
//		result = true;
//	}
//
//	return result;
//}
//
//bool os_removeIRQ(LPC43XX_IRQn_Type irq)
//{
//	bool result = false;
//
//	if (isr_user_handler_vector[irq] != NULL)
//	{
//		isr_user_handler_vector[irq] = NULL;
//		NVIC_ClearPendingIRQ(irq);
//		NVIC_DisableIRQ(irq);
//		result = true;
//	}
//
//	return result;
//}
//
//void os_IRQHandler(LPC43XX_IRQn_Type IRQn)
//{
//	void (*user_IRQ_handler)(void);
//
//	os_control_state_t previus_os_control_state;
//
//	/*Guardar estado anterior del sistema operativo*/
//	previus_os_control_state = os_get_controlState();
//
//	/*Establecer el estado del sistema operativo para indicar que está atendiendo una interrupción */
//	os_set_controlState(os_control_state__running_from_IRQ);
//
//	/*Obtener el handler de interrupción asociado a la interrupción actual */
//	user_IRQ_handler = isr_user_handler_vector[IRQn];
//
//	/*Ejecutar el handler de interrupción del usuario*/
//	if (NULL != user_IRQ_handler)
//	{
//		user_IRQ_handler();
//	}
//
//	/*Restablecer el estado anterior del sistema operativo*/
//	os_set_controlState(previus_os_control_state);
//
//	/*Limpiar el flag de la interrupción, sino volvería a generarse la misma interrupción*/
//	NVIC_ClearPendingIRQ(IRQn);
//
//	 /* Si hubo alguna llamada desde una interrupcion a una api liberando un evento, entonces
//	 * llamamos al scheduler
//	 */
//	if (os_isSchedulingFromIRQ())  {
//		os_clearSchedulingFromIRQ();
//		os_CpuYield();
//	}
//}
//
///*==================[interrupt service routines]=============================*/
//
//void WWDG_IRQHandler(void){os_IRQHandler(         WWDG_IRQn         );}
//void PVD_IRQHandler(void){os_IRQHandler(       PVD_IRQn       );}
//void TAMP_STAMP_IRQHandler(void){os_IRQHandler(         TAMP_STAMP_IRQn         );}
//void RTC_WKUP_IRQHandler(void){os_IRQHandler(RTC_WKUP_IRQn   );}
//void FLASH_IRQHandler(void){os_IRQHandler(FLASH_IRQn   );}                                        */
//void RCC_IRQHandler                    /* RCC                          */
//.word     EXTI0_IRQHandler                  /* EXTI Line0                   */
//.word     EXTI1_IRQHandler                  /* EXTI Line1                   */
//.word     EXTI2_IRQHandler                  /* EXTI Line2                   */
//.word     EXTI3_IRQHandler                  /* EXTI Line3                   */
//.word     EXTI4_IRQHandler                  /* EXTI Line4                   */
//.word     DMA1_Stream0_IRQHandler           /* DMA1 Stream 0                */
//.word     DMA1_Stream1_IRQHandler           /* DMA1 Stream 1                */
//.word     DMA1_Stream2_IRQHandler           /* DMA1 Stream 2                */
//.word     DMA1_Stream3_IRQHandler           /* DMA1 Stream 3                */
//.word     DMA1_Stream4_IRQHandler           /* DMA1 Stream 4                */
//.word     DMA1_Stream5_IRQHandler           /* DMA1 Stream 5                */
//.word     DMA1_Stream6_IRQHandler           /* DMA1 Stream 6                */
//.word     ADC_IRQHandler                    /* ADC1, ADC2 and ADC3s         */
//.word     CAN1_TX_IRQHandler                /* CAN1 TX                      */
//.word     CAN1_RX0_IRQHandler               /* CAN1 RX0                     */
//.word     CAN1_RX1_IRQHandler               /* CAN1 RX1                     */
//.word     CAN1_SCE_IRQHandler               /* CAN1 SCE                     */
//.word     EXTI9_5_IRQHandler                /* External Line[9:5]s          */
//.word     TIM1_BRK_TIM9_IRQHandler          /* TIM1 Break and TIM9          */
//.word     TIM1_UP_TIM10_IRQHandler          /* TIM1 Update and TIM10        */
//.word     TIM1_TRG_COM_TIM11_IRQHandler     /* TIM1 Trigger and Commutation and TIM11 */
//.word     TIM1_CC_IRQHandler                /* TIM1 Capture Compare         */
//.word     TIM2_IRQHandler                   /* TIM2                         */
//.word     TIM3_IRQHandler                   /* TIM3                         */
//.word     TIM4_IRQHandler                   /* TIM4                         */
//.word     I2C1_EV_IRQHandler                /* I2C1 Event                   */
//.word     I2C1_ER_IRQHandler                /* I2C1 Error                   */
//.word     I2C2_EV_IRQHandler                /* I2C2 Event                   */
//.word     I2C2_ER_IRQHandler                /* I2C2 Error                   */
//.word     SPI1_IRQHandler                   /* SPI1                         */
//.word     SPI2_IRQHandler                   /* SPI2                         */
//.word     USART1_IRQHandler                 /* USART1                       */
//.word     USART2_IRQHandler                 /* USART2                       */
//.word     USART3_IRQHandler                 /* USART3                       */
//.word     EXTI15_10_IRQHandler              /* External Line[15:10]s        */
//.word     RTC_Alarm_IRQHandler              /* RTC Alarm (A and B) through EXTI Line */
//.word     OTG_FS_WKUP_IRQHandler            /* USB OTG FS Wakeup through EXTI line */
//.word     TIM8_BRK_TIM12_IRQHandler         /* TIM8 Break and TIM12         */
//.word     TIM8_UP_TIM13_IRQHandler          /* TIM8 Update and TIM13        */
//.word     TIM8_TRG_COM_TIM14_IRQHandler     /* TIM8 Trigger and Commutation and TIM14 */
//.word     TIM8_CC_IRQHandler                /* TIM8 Capture Compare         */
//.word     DMA1_Stream7_IRQHandler           /* DMA1 Stream7                 */
//.word     FMC_IRQHandler                    /* FMC                         */
//.word     SDIO_IRQHandler                   /* SDIO                         */
//.word     TIM5_IRQHandler                   /* TIM5                         */
//.word     SPI3_IRQHandler                   /* SPI3                         */
//.word     UART4_IRQHandler                  /* UART4                        */
//.word     UART5_IRQHandler                  /* UART5                        */
//.word     TIM6_DAC_IRQHandler               /* TIM6 and DAC1&2 underrun errors */
//.word     TIM7_IRQHandler                   /* TIM7                         */
//.word     DMA2_Stream0_IRQHandler           /* DMA2 Stream 0                */
//.word     DMA2_Stream1_IRQHandler           /* DMA2 Stream 1                */
//.word     DMA2_Stream2_IRQHandler           /* DMA2 Stream 2                */
//.word     DMA2_Stream3_IRQHandler           /* DMA2 Stream 3                */
//.word     DMA2_Stream4_IRQHandler           /* DMA2 Stream 4                */
//.word     ETH_IRQHandler                    /* Ethernet                     */
//.word     ETH_WKUP_IRQHandler               /* Ethernet Wakeup through EXTI line */
//.word     CAN2_TX_IRQHandler                /* CAN2 TX                      */
//.word     CAN2_RX0_IRQHandler               /* CAN2 RX0                     */
//.word     CAN2_RX1_IRQHandler               /* CAN2 RX1                     */
//.word     CAN2_SCE_IRQHandler               /* CAN2 SCE                     */
//.word     OTG_FS_IRQHandler                 /* USB OTG FS                   */
//.word     DMA2_Stream5_IRQHandler           /* DMA2 Stream 5                */
//.word     DMA2_Stream6_IRQHandler           /* DMA2 Stream 6                */
//.word     DMA2_Stream7_IRQHandler           /* DMA2 Stream 7                */
//.word     USART6_IRQHandler                 /* USART6                       */
//.word     I2C3_EV_IRQHandler                /* I2C3 event                   */
//.word     I2C3_ER_IRQHandler                /* I2C3 error                   */
//.word     OTG_HS_EP1_OUT_IRQHandler         /* USB OTG HS End Point 1 Out   */
//.word     OTG_HS_EP1_IN_IRQHandler          /* USB OTG HS End Point 1 In    */
//.word     OTG_HS_WKUP_IRQHandler            /* USB OTG HS Wakeup through EXTI */
//.word     OTG_HS_IRQHandler                 /* USB OTG HS                   */
//.word     DCMI_IRQHandler                   /* DCMI                         */
//.word     0                                 /* Reserved                     */
//.word     HASH_RNG_IRQHandler               /* Hash and Rng                 */
//.word     FPU_IRQHandler                    /* FPU                          */
//.word     UART7_IRQHandler                  /* UART7                        */
//.word     UART8_IRQHandler                  /* UART8                        */
//.word     SPI4_IRQHandler                   /* SPI4                         */
//.word     SPI5_IRQHandler                   /* SPI5 						  */
//.word     SPI6_IRQHandler                   /* SPI6						  */
//.word     SAI1_IRQHandler                   /* SAI1						  */
//.word     LTDC_IRQHandler                   /* LTDC_IRQHandler			  */
//.word     LTDC_ER_IRQHandler                /* LTDC_ER_IRQHandler			  */
//.word     DMA2D_IRQHandler
