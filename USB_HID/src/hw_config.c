/******************** (C) COPYRIGHT 2011 STMicroelectronics ********************
* File Name          : hw_config.c
* Author             : MCD Application Team
* Version            : V3.3.0
* Date               : 21-March-2011
* Description        : Hardware Configuration & Setup
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

#include "demo.h"

#include "hw_config.h"
#include "usb_lib.h"
#include "usb_desc.h"	 
#include "usb_pwr.h"
#include "usb_lib.h" 


ErrorStatus HSEStartUpStatus;
extern __IO uint8_t PrevXferComplete;
/****************************************************************************
* 名    称：u8 JoyState(void)
* 功    能：是否有键按下
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
u8 JoyState(void)
{
  /* 右键被按下 */
  if (!GPIO_ReadInputDataBit(GPIOE, JOY_RIGHT))
  {
    return RIGHT;
  }
  /* 左键被按下 */
  if (!GPIO_ReadInputDataBit(GPIOE, JOY_LEFT))
  {
    return LEFT;
  }
  /* 上键被按下 */
  if (!GPIO_ReadInputDataBit(GPIOE, JOY_UP))
  {
    return UP;
  }
  /* 下键被按下 */
  if (!GPIO_ReadInputDataBit(GPIOE, JOY_DOWN))
  {
    return DOWN;
  }
//  if (!GPIO_ReadInputDataBit(GPIOC, JOY_LEFT_BUTTON))
//  {
//    return LEFT_BUTTON;
//  }
//   if (!GPIO_ReadInputDataBit(GPIOC, JOY_RIGHT_BUTTON))
//  {
//    return RIGHT_BUTTON;
//  }
  /* 没有键被按下 */
  else
  {
    return 0;
  }
}

/****************************************************************************
* 名    称：void Joystick_Send(u8 Keys)
* 功    能：根据检测的键值向USB端点1发送信息
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void Joystick_Send(u8 Keys)
{
  u8 Mouse_Buffer[4] = {0, 0, 0, 0};
  s8 X = 0, Y = 0,BUTTON=0;

  switch (Keys)
  {
    case RIGHT:
      X += CURSOR_STEP;
      break;
    case LEFT:

      X -= CURSOR_STEP;
      break;
    case UP:
      Y -= CURSOR_STEP;
      break;
    case DOWN:
      Y += CURSOR_STEP;
      break;
	 case LEFT_BUTTON:
      BUTTON = BUTTON|0x01;
      break;
 	 case RIGHT_BUTTON:
      BUTTON = BUTTON|0x02;
      break;
    
    default:
      return;
  }

  /* prepare buffer to send */
  Mouse_Buffer[0] = BUTTON;
  Mouse_Buffer[1] = X;
  Mouse_Buffer[2] = Y;

  /* Reset the control token to inform upper layer that a transfer is ongoing */
  PrevXferComplete = 0;

  /* 将鼠标值拷贝到USB端点1的发送缓存区 */
  UserToPMABufferCopy(Mouse_Buffer, GetEPTxAddr(ENDP1), 4);
  if(Mouse_Buffer[0]!= 0)
  {
    Mouse_Buffer[0] = 0;
    UserToPMABufferCopy(Mouse_Buffer, GetEPTxAddr(ENDP1), 4);
  } 
  /* 使能端点1发送 */
  SetEPTxValid(ENDP1);
}

/*******************************************************************************
* Function Name  : USB_Disconnect_Config
* Description    : Disconnect pin configuration
* Input          : None.
* Return         : None.
*******************************************************************************/
void USB_Disconnect_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable USB_DISCONNECT GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  /* USB_DISCONNECT_PIN used as USB pull-up */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
}
/*******************************************************************************
* Function Name  : HexToChar.
* Description    : Convert Hex 32Bits value into char.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
static void IntToUnicode (uint32_t value , uint8_t *pbuf , uint8_t len)
{
  uint8_t idx = 0;
  
  for( idx = 0 ; idx < len ; idx ++)
  {
    if( ((value >> 28)) < 0xA )
    {
      pbuf[ 2* idx] = (value >> 28) + '0';
    }
    else
    {
      pbuf[2* idx] = (value >> 28) + 'A' - 10; 
    }
    
    value = value << 4;
    
    pbuf[ 2* idx + 1] = 0;
  }
}
/*******************************************************************************
* Function Name  : Get_SerialNum.
* Description    : Create the serial number string descriptor.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Get_SerialNum(void)
{
  uint32_t Device_Serial0, Device_Serial1, Device_Serial2;

#ifdef STM32L1XX_MD
  Device_Serial0 = *(uint32_t*)(0x1FF80050);
  Device_Serial1 = *(uint32_t*)(0x1FF80054);
  Device_Serial2 = *(uint32_t*)(0x1FF80064);
#else  
  Device_Serial0 = *(__IO uint32_t*)(0x1FFFF7E8);
  Device_Serial1 = *(__IO uint32_t*)(0x1FFFF7EC);
  Device_Serial2 = *(__IO uint32_t*)(0x1FFFF7F0);
#endif /* STM32L1XX_MD */
  
  Device_Serial0 += Device_Serial2;

  if (Device_Serial0 != 0)
  {
    IntToUnicode (Device_Serial0, &Joystick_StringSerial[2] , 8);
    IntToUnicode (Device_Serial1, &Joystick_StringSerial[18], 4);
  }
}

/*******************************************************************************
* Function Name  : Enter_LowPowerMode.
* Description    : Power-off system clocks and power while entering suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
//  /* Set the device state to suspend */
//  bDeviceState = SUSPENDED;
//
//  /* Clear EXTI Line18 pending bit */
//  EXTI_ClearITPendingBit(KEY_BUTTON_EXTI_LINE);
//
//  /* Request to enter STOP mode with regulator in low power mode */
//  PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
}

/*******************************************************************************
* Function Name  : Leave_LowPowerMode.
* Description    : Restores system clocks and power while exiting suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
  DEVICE_INFO *pInfo = &Device_Info;

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
  {}
  
#ifdef  STM32F10X_CL
  /* Enable PLL2 */
  RCC_PLL2Cmd(ENABLE);  

  /* Wait till PLL2 is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_PLL2RDY) == RESET)
  {}
#endif /* STM32F10X_CL */
  
  /* Enable PLL1 */
  RCC_PLLCmd(ENABLE);

  /* Wait till PLL1 is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
  {}

  /* Select PLL as system clock source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

  /* Wait till PLL is used as system clock source */
#ifdef STM32L1XX_MD  
  while (RCC_GetSYSCLKSource() != 0x0C)
#else   
  while (RCC_GetSYSCLKSource() != 0x08)
#endif /* STM32L1XX_MD */ 
  {}  
  
  /* Set the device state to the correct state */
  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
  else
  {
    bDeviceState = ATTACHED;
  }
}


/*******************************************************************************
* Function Name  : USB_Cable_Config
* Description    : Software Connection/Disconnection of USB Cable.
* Input          : None.
* Return         : Status
*******************************************************************************/
void USB_Cable_Config (FunctionalState NewState)
{  
  if (NewState != DISABLE)
  {
    GPIO_ResetBits(GPIOC, GPIO_Pin_13);
  }
  else
  {
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
  }	
}
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
