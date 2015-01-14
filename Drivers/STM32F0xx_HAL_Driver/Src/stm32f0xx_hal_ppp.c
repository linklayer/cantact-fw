/**
  ******************************************************************************
  * @file    stm32f0xx_hal_ppp.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    03-Oct-2014
  * @brief   PPP HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the PPP peripheral:
  *           + Initialization/de-initialization functions
  *           + I/O operation functions
  *           + Peripheral Control functions 
  *           + Peripheral State functions
  *         
  @verbatim
================================================================================
          ##### Product specific features integration #####
================================================================================
           
  [..]  This section can contain: 
       (#) Description of the product specific implementation; all features
           that is specific to this IP: separate clock for RTC/LCD/IWDG/ADC,
           power domain (backup domain for the RTC)...   
       (#) IP main features, only when needed and not mandatory for all IPs,
           ex. for xWDG, GPIO, COMP...

  [..]  You can add as much sections as needed.

  [..]  You can add as much sections as needed.

            ##### How to use this driver #####
================================================================================
           [..]
            (#) Add here ALL NEEDED STEPS (i.e. list of functions and parameters) 
                TO INITIALIZE AND USE THE PERIPHERAL. Following the listed steps, 
                user should be able to make the peripheral working without any
                additional information from other resources (ex. product Reference 
                Manual, HAL User Manual...).   
 
                Basically, it should be the same as we provide in the examples.
 
            (#) Exception can be made for system IPs (there is no standard order
                of API usage) for which the description of API usage will be made
                in each function group. These IPs are: PWR, RCC and FLASH
                In this case, this section should contain the following note:
                "Refer to the description within the different function groups below"
  
            (+) 
  
            (+) 
  
            (+) 
  
            (+) 
  
            (+) 
  
            (+) 
  
  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2014 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************  
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/** @addtogroup STM32F0xx_HAL_Driver
  * @{
  */

/** @defgroup PPP PPP HAL module driver
  * @brief PPP HAL module driver
  * @{
  */

#ifdef HAL_PPP_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions ---------------------------------------------------------*/
/** @defgroup PPP_Exported_Functions PPP Exported Functions
  * @{
  */

/** @defgroup PPP_Exported_Functions_Group1 Initialization/de-initialization functions 
 *  @brief    Initialization and Configuration functions 
 *
@verbatim    
 ===============================================================================
              ##### Initialization and Configuration functions #####
 ===============================================================================
    [..]  This section provides functions allowing to:
 
@endverbatim
  * @{
  */

/**
  * @brief  Initializes the PPP according to the specified
  *         parameters in the PPP_InitTypeDef and create the associated handle.
  * @param  hppp: PPP handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PPP_Init(PPP_HandleTypeDef *hppp)
{ 

  /* Check the parameters */
  assert_param(IS_PPP_CONFIGx(hppp->Config.Config1)); 

  /* Check the PPP handle allocation */
  if(hppp == NULL)
  {
    return HAL_ERROR;
  }
 
  /* Init the low level hardware : GPIO, CLOCK, NVIC and DMA */
  HAL_PPP_MspInit(hppp);
  
  /* Set PPP parameters */
  
  /* Enable the Peripharal */
  
  /* Initialize the PPP state*/
  hppp->State = HAL_PPP_STATE_READY;
  
  return HAL_OK;
}

/**
  * @brief  DeInitializes the PPP peripheral 
  * @param  hppp: PPP handle
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PPP_DeInit(PPP_HandleTypeDef *hppp)
{
  /* Check the PPP peripheral state */
  if(hppp->State == HAL_PPP_STATE_BUSY)
  {
     return HAL_ERROR;
  }

  hppp->State = HAL_PPP_STATE_BUSY;
  
  /* DeInit the low level hardware: GPIO, CLOCK, NVIC and DMA */
  HAL_PPP_MspDeInit(hppp);
    
  hppp->State = HAL_PPP_STATE_DISABLED; 
  return HAL_OK;
}

/**
  * @brief  Initializes the PPP MSP.
  * @param  hppp: PPP handle
  * @retval None
  */
__weak void HAL_PPP_MspInit(PPP_HandleTypeDef *hppp)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PPP_MspInit could be implenetd in the user file
   */
}

/**
  * @brief  DeInitializes PPP MSP.
  * @param  hppp: PPP handle
  * @retval None
  */
__weak void HAL_PPP_MspDeInit(PPP_HandleTypeDef *hppp)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PPP_MspDeInit could be implenetd in the user file
   */
}

/**
  * @}
  */

/** @defgroup PPP_Exported_Functions_Group2 I/O operation functions 
 *  @brief   Data transfers functions 
 *
@verbatim   
 ===============================================================================
                      ##### IO operation functions #####
 ===============================================================================  
    [..]
    This subsection provides a set of functions allowing to manage the PPP data 
    transfers.

@endverbatim
  * @{
  */

/**
  * @brief  Send an amount of data in blocking mode 
  * @param  hppp: PPP handle
  * @param  pData: pointer to data buffer
  * @param  Size: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PPP_Transmit(PPP_HandleTypeDef *hppp, uint8_t *pData, uint16_t Size)
{
  /* Process Locked */
  __HAL_LOCK(hppp);
  
  hppp->State = HAL_PPP_STATE_BUSY;  
  /* PPP processing... */ 
  hppp->State = HAL_PPP_STATE_READY; 
                  
  /* Process Unlocked */
  __HAL_UNLOCK(hppp);      
  
  return HAL_OK;
}

/**
  * @brief  Receive an amount of data in blocking mode 
  * @param  hppp: PPP handle
  * @param  pData: pointer to data buffer
  * @param  Size: amount of data to be received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PPP_Receive(PPP_HandleTypeDef *hppp, uint8_t *pData, uint16_t Size)
{ 
  /* Process Locked */
  __HAL_LOCK(hppp);
  
  hppp->State = HAL_PPP_STATE_BUSY;  
  /* PPP processing... */ 
  hppp->State = HAL_PPP_STATE_READY; 
                  
  /* Process Unlocked */
  __HAL_UNLOCK(hppp);      
  
  return HAL_OK;
}

/**
  * @brief  Send an amount of data in non blocking mode 
  * @param  hppp: PPP handle
  * @param  pData: pointer to data buffer
  * @param  Size: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PPP_Transmit_IT(PPP_HandleTypeDef *hppp, uint8_t *pData, uint16_t Size)
{
  /* Process Locked */
  __HAL_LOCK(hppp);
  
  hppp->State = HAL_PPP_STATE_BUSY;  
  /* PPP processing... */ 
  hppp->State = HAL_PPP_STATE_READY; 
                  
  /* Process Unlocked */
  __HAL_UNLOCK(hppp);      
  
  return HAL_OK;
}

/**
  * @brief  Receive an amount of data in non blocking mode 
  * @param  hppp: PPP handle
  * @param  pData: pointer to data buffer
  * @param  Size: amount of data to be received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PPP_Receive_IT(PPP_HandleTypeDef *hppp, uint8_t *pData, uint16_t Size)
{
  /* Process Locked */
  __HAL_LOCK(hppp);
  
  hppp->State = HAL_PPP_STATE_BUSY;  
  /* PPP processing... */ 
  hppp->State = HAL_PPP_STATE_READY; 
                  
  /* Process Unlocked */
  __HAL_UNLOCK(hppp);      
  
  return HAL_OK;
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  hppp: PPP handle
  * @retval HAL status
  */
void HAL_PPP_IRQHandler(PPP_HandleTypeDef *hppp)
{
  /* PPP in mode Tramitter ---------------------------------------------------*/
  if (__HAL_PPP_GET_IT(hppp, PPP_IT_TC) != RESET)
  { 
    __HAL_PPP_CLEAR_IT(hppp, PPP_IT_TC);
    HAL_PPP_Transmit_IT(hppp, NULL, 0);
  }
  
  /* PPP in mode Receiver ----------------------------------------------------*/
  if (__HAL_PPP_GET_IT(hppp, PPP_IT_RXNE) != RESET)
  {
    __HAL_PPP_CLEAR_IT(hppp, PPP_IT_RXNE);
    HAL_PPP_Receive_IT(hppp, NULL, 0);
  } 
}

/**
  * @brief  Tx Transfer completed callbacks
  * @param  hppp: ppp handle
  * @retval None
  */
 __weak void HAL_PPP_TxCpltCallback(PPP_HandleTypeDef *hppp)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PPP_TxCpltCallback could be implenetd in the user file
   */ 
}

/**
  * @brief  Rx Transfer completed callbacks
  * @param  hppp: PPP handle
  * @retval None
  */
__weak void HAL_PPP_RxCpltCallback(PPP_HandleTypeDef *hppp)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_PPP_TxCpltCallback could be implenetd in the user file
   */ 
}

/**
  * @brief  Send an amount of data in non blocking mode 
  * @param  hppp: ppp handle
  * @param  pData: pointer to data buffer
  * @param  Size: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PPP_Transmit_DMA(PPP_HandleTypeDef *hppp, uint8_t *pData, uint16_t Size)
{
  /* Check the PPP peripheral state */
  if(hppp->State == HAL_PPP_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Process Locked */
  __HAL_LOCK(hppp);
  
  hppp->State = HAL_PPP_STATE_BUSY;  
  /* PPP processing... */ 
  hppp->State = HAL_PPP_STATE_READY; 
                  
  /* Process Unlocked */
  __HAL_UNLOCK(hppp);      
  
  return HAL_OK;
}

/**
  * @brief  Receive an amount of data in non blocking mode 
  * @param  hppp: PPP handle
  * @param  pData: pointer to data buffer
  * @param  Size: amount of data to be received
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PPP_Receive_DMA(PPP_HandleTypeDef *hppp, uint8_t *pData, uint16_t Size)
{
  /* Check the PPP peripheral state */
  if(hppp->State == HAL_PPP_STATE_BUSY)
  {
     return HAL_BUSY;
  }
  
  /* Process Locked */
  __HAL_LOCK(hppp);
  
  hppp->State = HAL_PPP_STATE_BUSY;  
  /* PPP processing... */ 
  hppp->State = HAL_PPP_STATE_READY; 
                  
  /* Process Unlocked */
  __HAL_UNLOCK(hppp);      
  
  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup PPP_Exported_Functions_Group3 Peripheral Control functions 
 *  @brief   management functions 
 *
@verbatim   
 ===============================================================================
                      ##### Peripheral Control functions #####
 ===============================================================================  
    [..]
    This subsection provides a set of functions allowing to control the PPP data 
    transfers.

@endverbatim
  * @{
  */

/**
  * @brief  Configures PPP features and fonctionality. 
  * @param  hppp: PPP handle
  * @param  control: Specifies which PPP_InitTypeDef structure’s field will be affected
  * @param  Size: amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_PPP_Ctl(PPP_HandleTypeDef *hppp, PPP_ControlTypeDef  control, uint16_t *args)
{
  if(hppp->State == HAL_PPP_STATE_BUSY)
  {
     return HAL_BUSY;
  }

  hppp->State = HAL_PPP_STATE_BUSY;

  /* Update the PPP features and fonctionality */
  /* ... */
  
  hppp->State = HAL_PPP_STATE_READY;
  
  return HAL_OK; 
}
/**
  * @}
  */

/** @defgroup PPP_Exported_Functions_Group4 Peripheral State functions
 *  @brief   Peripheral State functions 
 *
@verbatim   
 ===============================================================================
                      ##### Peripheral State functions #####
 ===============================================================================  
    [..]
    This subsection permit to get in run-time the status of the peripheral 
    and the data flow.

@endverbatim
  * @{
  */

/**
  * @brief  Return the PPP state
  * @param  hppp : PPP handle
  * @retval HAL state
  */
HAL_PPP_StateTypeDef HAL_PPP_GetState(PPP_HandleTypeDef *hppp)
{
  return hppp->State;
}
/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_PPP_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
