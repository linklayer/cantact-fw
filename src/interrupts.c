//
// interrupts: handle global system interrupts
//

#include "stm32f0xx_hal.h"
#include "stm32f0xx.h"
#include "interrupts.h"

extern PCD_HandleTypeDef hpcd_USB_FS;


// Handle USB interrupts
void USB_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&hpcd_USB_FS);
}


// Handle SysTick interrupt
void SysTick_Handler(void)
{
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
}


// Handle CAN interrupts
void CEC_CAN_IRQHandler(void)
{
  HAL_CAN_IRQHandler(can_gethandle());
}
