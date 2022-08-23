
#include "MSE_OS_IRQ.h"

static IRQ_UserHandler_t Os_IRQ_UserHandler[OS_NUMBER_OF_IRQ];

void Os_IRQHandler(IRQn_Type valor_IRQ)
{
	if(0 != Os_IRQ_UserHandler[valor_IRQ])
	{
		Os_IRQ_UserHandler[valor_IRQ]();
	}
	NVIC_ClearPendingIRQ(valor_IRQ);
}

void Os_SetIRQ(IRQn_Type valor_IRQ, IRQ_UserHandler_t IRQ_UserHandler)
{
	Os_IRQ_UserHandler[valor_IRQ] = IRQ_UserHandler;
	NVIC_ClearPendingIRQ(valor_IRQ);
	NVIC_EnableIRQ(valor_IRQ);
}

void OS_ClearIRQ(IRQn_Type valor_IRQ)
{
	Os_IRQ_UserHandler[valor_IRQ] = 0;
	NVIC_ClearPendingIRQ(valor_IRQ);
	NVIC_DisableIRQ(valor_IRQ);
}

//void EXTI15_10_IRQHandler(void)
//{
//	if(EXTI->PR & EXTI_PR_PR13)
//	{
//		flag = 1;
//		NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
//		EXTI->PR |= (EXTI_PR_PR13);
//	}
//}

/**
 *
 *
 * */
void PVD_IRQHandler(void)
{
	Os_IRQHandler(PVD_IRQn);
}

void TAMP_STAMP_IRQHandler(void)
{
	Os_IRQHandler(TAMP_STAMP_IRQn);
}

void RTC_WKUP_IRQHandler(void)
{
	Os_IRQHandler(RTC_WKUP_IRQn);
}

void FLASH_IRQHandler(void)
{
	Os_IRQHandler(FLASH_IRQn);
}

void RCC_IRQHandler(void)
{
	Os_IRQHandler(RCC_IRQn);
}

void EXTI0_IRQHandler(void)
{
	Os_IRQHandler(EXTI0_IRQn);
}

void EXTI1_IRQHandler(void)
{
	Os_IRQHandler(EXTI1_IRQn);
}

void EXTI2_IRQHandler(void)
{
	Os_IRQHandler(EXTI2_IRQn);
}

void EXTI3_IRQHandler(void)
{
	Os_IRQHandler(EXTI3_IRQn);
}

void EXTI4_IRQHandler(void)
{
	Os_IRQHandler(EXTI4_IRQn);
}

void DMA1_Stream0_IRQHandler(void)
{
	Os_IRQHandler(DMA1_Stream0_IRQn);
}

void DMA1_Stream1_IRQHandler(void)
{
	Os_IRQHandler(DMA1_Stream1_IRQn);
}

void DMA1_Stream2_IRQHandler(void)
{
	Os_IRQHandler(DMA1_Stream2_IRQn);
}

void DMA1_Stream3_IRQHandler(void)
{
	Os_IRQHandler(DMA1_Stream3_IRQn);
}

void DMA1_Stream4_IRQHandler(void)
{
	Os_IRQHandler(DMA1_Stream4_IRQn);
}

void DMA1_Stream5_IRQHandler(void)
{
	Os_IRQHandler(DMA1_Stream5_IRQn);
}

void DMA1_Stream6_IRQHandler(void)
{
	Os_IRQHandler(DMA1_Stream6_IRQn);
}

void ADC_IRQHandler(void)
{
	Os_IRQHandler(ADC_IRQn);
}

void CAN1_TX_IRQHandler(void)
{
	Os_IRQHandler(CAN1_TX_IRQn);
}

void CAN1_RX0_IRQHandler(void)
{
	Os_IRQHandler(CAN1_RX0_IRQn);
}

void CAN1_RX1_IRQHandler(void)
{
	Os_IRQHandler(CAN1_RX1_IRQn);
}

void CAN1_SCE_IRQHandler(void)
{
	Os_IRQHandler(CAN1_SCE_IRQn);
}

void EXTI9_5_IRQHandler(void)
{
	Os_IRQHandler(EXTI9_5_IRQn);
}

void TIM1_BRK_TIM9_IRQHandler(void)
{
	Os_IRQHandler(TIM1_BRK_TIM9_IRQn);
}

void TIM1_BRK_TIM10_IRQHandler(void)
{
	Os_IRQHandler(TIM1_UP_TIM10_IRQn);
}

void TIM1_CC_IRQHandler(void)
{
	Os_IRQHandler(TIM1_CC_IRQn);
}

void TIM2_IRQHandler(void)
{
	Os_IRQHandler(TIM2_IRQn);
}

void TIM3_IRQHandler(void)
{
	Os_IRQHandler(TIM3_IRQn);
}

void TIM4_IRQHandler(void)
{
	Os_IRQHandler(TIM4_IRQn);
}

void I2C1_EV_IRQHandler(void)
{
	Os_IRQHandler(I2C1_EV_IRQn);
}

void I2C1_ER_IRQHandler(void)
{
	Os_IRQHandler(I2C1_ER_IRQn);
}

void I2C2_EV_IRQHandler(void)
{
	Os_IRQHandler(I2C2_EV_IRQn);
}

void I2C2_ER_IRQHandler(void)
{
	Os_IRQHandler(I2C2_ER_IRQn);
}

void SPI1_IRQHandler(void)
{
	Os_IRQHandler(SPI1_IRQn);
}

void SPI2_IRQHandler(void)
{
	Os_IRQHandler(SPI2_IRQn);
}

void USART1_IRQHandler(void)
{
	Os_IRQHandler(USART1_IRQn);
}

void USART2_IRQHandler(void)
{
	Os_IRQHandler(USART2_IRQn);
}

void USART3_IRQHandler(void)
{
	Os_IRQHandler(USART3_IRQn);
}

void EXTI15_10_IRQHandler(void)
{
	Os_IRQHandler(EXTI15_10_IRQn);
}

void RTC_Alarm_IRQHandler(void)
{
	Os_IRQHandler(RTC_Alarm_IRQn);
}

void OTG_FS_WKUP_IRQHandler(void)
{
	Os_IRQHandler(OTG_FS_WKUP_IRQn);
}

void OTG_HS_EP1_OUT_IRQHandler(void)
{
	Os_IRQHandler(OTG_HS_EP1_OUT_IRQn);
}

void OTG_HS_EP1_IN_IRQHandler(void)
{
	Os_IRQHandler(OTG_HS_EP1_IN_IRQn);
}

void OTG_HS_WKUP_IRQHandler(void)
{
	Os_IRQHandler(OTG_HS_WKUP_IRQn);
}

void OTG_HS_IRQHandler(void)
{
	Os_IRQHandler(OTG_HS_IRQn);
}

void DCMI_IRQHandler(void)
{
	Os_IRQHandler(DCMI_IRQn);
}

void HASH_RNG_IRQHandler(void)
{
	Os_IRQHandler(HASH_RNG_IRQn);
}

void FPU_IRQHandler(void)
{
	Os_IRQHandler(FPU_IRQn);
}

void UART7_IRQHandler(void)
{
	Os_IRQHandler(UART7_IRQn);
}

void UART8_IRQHandler(void)
{
	Os_IRQHandler(UART8_IRQn);
}

void SPI4_IRQHandler(void)
{
	Os_IRQHandler(SPI4_IRQn);
}

void SPI5_IRQHandler(void)
{
	Os_IRQHandler(SPI5_IRQn);
}

void SPI6_IRQHandler(void)
{
	Os_IRQHandler(SPI6_IRQn);
}

void SAI1_IRQHandler(void)
{
	Os_IRQHandler(SAI1_IRQn);
}

void LTDC_IRQHandler(void)
{
	Os_IRQHandler(LTDC_IRQn);
}

void LTDC_ER_IRQHandler(void)
{
	Os_IRQHandler(LTDC_ER_IRQn);
}

void DMA2D_IRQn_IRQHandler(void)
{
	Os_IRQHandler(DMA2D_IRQn);
}
