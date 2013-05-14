/**
  ******************************************************************************
  * @file    main.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    20-September-2012
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @addtogroup STM32F3-Discovery_Demo
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

extern __IO uint8_t PrevXferComplete;
__IO uint32_t USBConnectTimeOut = 100;

const uint8_t nrf_addr[] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};// RX_ADDR0;

  DECLARE_RING_BUFFER(usb_rx, 64, 8)
  DECLARE_RING_BUFFER(nrf_rx, 64, 8)
    
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef RF_USE_ACK_PAYLOAD
#warning Use ACK payload mode, make sure the "remote controller", "bootloader", and "app" use the same rf mode
#endif

/* callback functions */
void nrf_tx_done(uint8_t success)
{
#ifndef RF_USE_ACK_PAYLOAD
    nrf_enter_rx_mode();
#endif
    
    if(NRF_TX_SUCCESS == success){
        //LED4_ON;
    }else if(NRF_TX_FAIL == success){
        //LED5_ON;
    }else if(NRF_ACK_SUCCESS == success){
        //LED6_ON;
    }
}

void nrf_on_rx_data(const void* data, uint32_t len, uint8_t channel)
{
    uint8_t buf[64] = {0x55, 0xaa};
    buf[2] = len;
    memcpy(buf+3, data, len);
    ring_buf_push(nrf_rx, buf, 64);
}

void usb_get_data(const void* p, uint32_t len)
{
    ring_buf_push(usb_rx, p, 64);
}

/**
  * @brief  Main program.
  * @param  None 
  * @retval None
  */
int main(void)
{
    uint32_t led_status = 0;
    /* 2 bit for pre-emption priority, 2 bits for subpriority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    ring_buf_init(usb_rx);
    ring_buf_init(nrf_rx);
    
    setup_systick();
    enable_tick_count();
    setup_io_leds();
    setup_io_usb();
    
    nrf_init();
    nrf_detect();
#ifdef RF_USE_ACK_PAYLOAD
    nrf_tx_mode_dual(nrf_addr, 4, 0);
#else
    nrf_tx_mode_dyn(nrf_addr, 4, 0);
#endif
    {
        uint8_t status = nrf_read_reg(NRF_STATUS);
        nrf_write_reg(NRF_FLUSH_RX, 0xff);
        nrf_write_reg(NRF_FLUSH_TX, 0xff);
        nrf_write_reg(NRF_WRITE_REG|NRF_STATUS,status); // clear IRQ flags
    }
    
    //pwm_input_init();
    USB_Init();
    
    // wait usb ready
    //while ((bDeviceState != CONFIGURED)&&(USBConnectTimeOut != 0))
    //{}
    
    // endless loop
    while(1)
    {
        uint8_t data[64];
        if(ring_buf_pop(usb_rx, data, 64)){
            if(data[0] == 0x55 && data[1] == 0xaa && data[2]<32){
                // process usb data
#ifndef RF_USE_ACK_PAYLOAD
                nrf_enter_tx_mode();
#endif
                //LED7_ON;
                nrf_tx_packet_no_wait(data+3, data[2]);
                //LED8_TOGGLE;
            }else{
                //LED10_TOGGLE;
                usb_send_data(data,64);
            }
        }
        if(ring_buf_pop(nrf_rx, data, 64)){
            usb_send_data(data,64);
            //LED9_TOGGLE;
        }
        led_status++;
        if(led_status > 0x1000){
            uint8_t buf[64] = { 0x00, 0x1b, 0x01, 0xcf, 0xa8 };
            //LED3_TOGGLE;
            LED1_TOGGLE;
            led_status = 0;
            //nrf_enter_tx_mode();
            //nrf_tx_packet_no_wait(buf, 5);
        }
        if(!NRF_IRQ){
            nrf_irq_handler();
        }
    }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
