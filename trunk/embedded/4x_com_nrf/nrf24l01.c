#include "stm32f10x_lib.h"
#include <stdlib.h>
#include <string.h>
#include "spi.h"
#include "nrf24l01.h"
#include "interpreter.h"

#define TX_PLOAD_WIDTH  32  	// 20 uints TX payload
#define rf_pin_ce_high() 		GPIO_SetBits(GPIOB, GPIO_Pin_10)
#define rf_pin_ce_low() 		GPIO_ResetBits(GPIOB, GPIO_Pin_10)
#define CSN_H() 	GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define CSN_L() 	GPIO_ResetBits(GPIOA, GPIO_Pin_4)

///*********************************************NRF24L01*************************************
#define TX_ADR_WIDTH    4   	// 5 uints TX address width
#define RX_ADR_WIDTH    4   	// 5 uints RX address width

#define RX_PLOAD_WIDTH  32  	// 20 uints TX payload
u8  TX_ADDRESS[TX_ADR_WIDTH]= {0xE7,0xE7,0xE7,0xE7};	//本地地址
u8  RX_ADDRESS[RX_ADR_WIDTH]= {0x76,0x64,0xB7,0x5A};	//接收地址
///***************************************NRF24L01寄存器指令*******************************************************
#define READ_REG        0x00  	// 读寄存器指令
#define WRITE_REG       0x20 	// 写寄存器指令
#define RD_RX_PL_WID    0x60  	// 读取接收数据指令
#define RD_RX_PLOAD     0x61  	// 读取接收数据指令
#define WR_TX_PLOAD     0xA0  	// 写待发数据指令
#define WR_ACK_PLOAD    0xA8    // 写ACK数据指令
#define FLUSH_TX        0xE1 	// 冲洗发送 FIFO指令
#define FLUSH_RX        0xE2  	// 冲洗接收 FIFO指令
#define REUSE_TX_PL     0xE3  	// 定义重复装载数据指令
#define NOP             0xFF  	// 保留
///*************************************SPI(nRF24L01)寄存器地址****************************************************
#define CONFIG          0x00  // 配置收发状态，CRC校验模式以及收发状态响应方式
#define EN_AA           0x01  // 自动应答功能设置
#define EN_RXADDR       0x02  // 可用信道设置
#define SETUP_AW        0x03  // 收发地址宽度设置
#define SETUP_RETR      0x04  // 自动重发功能设置
#define RF_CH           0x05  // 工作频率设置
#define RF_SETUP        0x06  // 发射速率、功耗功能设置
#define NRFRegSTATUS    0x07  // 状态寄存器
#define OBSERVE_TX      0x08  // 发送监测功能
#define CD              0x09  // 地址检测           
#define RX_ADDR_P0      0x0A  // 频道0接收数据地址
#define RX_ADDR_P1      0x0B  // 频道1接收数据地址
#define RX_ADDR_P2      0x0C  // 频道2接收数据地址
#define RX_ADDR_P3      0x0D  // 频道3接收数据地址
#define RX_ADDR_P4      0x0E  // 频道4接收数据地址
#define RX_ADDR_P5      0x0F  // 频道5接收数据地址
#define TX_ADDR         0x10  // 发送地址寄存器
#define RX_PW_P0        0x11  // 接收频道0接收数据长度
#define RX_PW_P1        0x12  // 接收频道1接收数据长度
#define RX_PW_P2        0x13  // 接收频道2接收数据长度
#define RX_PW_P3        0x14  // 接收频道3接收数据长度
#define RX_PW_P4        0x15  // 接收频道4接收数据长度
#define RX_PW_P5        0x16  // 接收频道5接收数据长度
#define FIFO_STATUS     0x17  // FIFO栈入栈出状态寄存器设置
   
#define DYN_PLOAD_WIDTH_P0	   	0x1C
#define DYN_PLOAD_WIDTH     	0x1D  
///**************************************************************************************

#ifdef RF_USE_ACK_PAYLOAD
#warning Usb ACK payload mode, make sure the "remote controller", "bootloader", and "app" use the same rf mode
#endif

u8 NRF24L01_TxBuf[32]={0};
u8 RxBuf[32]={0};
u8 NRF_Dev_Addr[5];

u8 nrf_length;

///******************************************************************************************
///*延时函数,非精确延时
///******************************************************************************************/
void Delay_us(u32 n)
{
	u32 i;
	
	while(n--)
	{
 	   i=2;
 	   while(i--);
  }
}

void NRF24L01_Delay(u32 dly)
{
	while(--dly);	//dly=100: 8.75us; dly=100: 85.58 us (SYSCLK=72MHz)
}

///****************************************************************************************
///*NRF24L01初始化
///***************************************************************************************/
void init_NRF24L01(void)
{
 	u8 buf[5]={0};
 	u8 i;
 
  	Delay_us(100);
  	rf_pin_ce_low();    // chip enable
  	rf_pin_ce_high();   // Spi disable 
  	SPI_Read_Buf(TX_ADDR, buf, TX_ADR_WIDTH);//debug 测试原来的本地地址：复位值是：0xE7 0xE7 0xE7 0xE7 0xE7
  	for(i=0;i<5;i++) NRF_Dev_Addr[i]=buf[i];

//	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, RX_ADDRESS, RX_ADR_WIDTH); 	//写接收端地址

   	SPI_WR_Reg(WRITE_REG + CONFIG, 0x0F);		//k: enter power-up,enable cc, allow int
	SPI_WR_Reg(WRITE_REG + EN_RXADDR, 0x01);  			//允许接收地址只有频道0，如果需要多频道可以参考Page21 
	SPI_WR_Reg(WRITE_REG + EN_AA, 0x01);		      	//频道0自动	ACK应答允许	
 	SPI_WR_Reg(WRITE_REG + SETUP_AW, 0x02);
	SPI_WR_Reg(WRITE_REG + SETUP_RETR, 0x34); 			//设置自动重发时间和次数：500us + 86us, 10 retrans...
	SPI_WR_Reg(WRITE_REG + RF_CH, 0);        			//设置信道工作为2.4GHZ，收发必须一致
	SPI_WR_Reg(WRITE_REG + RF_SETUP, 0x0E);   			//设置发射速率为1MHZ，发射功率为最大值0dB	
	SPI_WR_Reg(WRITE_REG + DYN_PLOAD_WIDTH_P0, 0x01);	//k: enable dynamic payload length in P0 only
#ifdef RF_USE_ACK_PAYLOAD
	SPI_WR_Reg(WRITE_REG + DYN_PLOAD_WIDTH, 0x04 | 0x02);		//k: enable dynamic payload lenth
#else
    SPI_WR_Reg(WRITE_REG + DYN_PLOAD_WIDTH, 0x04);		//k: enable dynamic payload lenth
#endif

	rf_pin_ce_high(); 
}

///****************************************************************************************************
///*函数：uchar SPI_Read(u8 reg)
///*功能：NRF24L01的SPI时序
///****************************************************************************************************/
u8 SPI_RD_Reg(u8 reg)
{
	u8 reg_val;
	
	CSN_L();                		// CSN low, initialize SPI communication...
	SPI1_ReadWrite(reg);            // Select register to read from..
	reg_val = SPI1_ReadWrite(0);    // ..then read registervalue
	CSN_H();                		// CSN high, terminate SPI communication
	
	return(reg_val);        // return register value
}

//****************************************************************************************************/
//*功能：NRF24L01读写寄存器函数
//****************************************************************************************************/
u8 SPI_WR_Reg(u8 reg, u8 value)
{
	u8 status;
	
	CSN_L();                   		// CSN low, init SPI transaction
	status = SPI1_ReadWrite(reg);	// select register
	SPI1_ReadWrite(value);          // ..and write value to it..
	CSN_H();                   		// CSN high again
	
	return(status);            		// return nRF24L01 status u8
}
///****************************************************************************************************/
//*函数：uint SPI_Read_Buf(u8 reg, u8 *pBuf, u8 Len)
//*功能: 用于读数据，reg：为寄存器地址，pBuf：为待读出数据地址，uchars：读出数据的个数
//****************************************************************************************************/
u8 SPI_Read_Buf(u8 reg, u8 *pBuf, u8 Len)
{
	u8 status,i;
	
	CSN_L();                    		// Set CSN low, init SPI tranaction
	status = SPI1_ReadWrite(reg);       		// Select register to write to and read status u8
	
  	for(i=0;i<Len;i++)
  	{
     	pBuf[i] = SPI1_ReadWrite(0);
  	}
	
	CSN_H();                           
	
	return(status);                    // return nRF24L01 status u8
}
//*********************************************************************************************************
//*函数：uint SPI_Write_Buf(u8 reg, u8 *pBuf, u8 Len)
//*功能: 用于写数据：为寄存器地址，pBuf：为待写入数据地址，uchars：写入数据的个数
//*********************************************************************************************************/
u8 SPI_Write_Buf(u8 reg, u8 *pBuf, u8 Len)
{
	u8 status,i;
	
	CSN_L();            //SPI使能       
	status = SPI1_ReadWrite(reg);   
	for(i=0; i<Len; i++) //
	{
	   SPI1_ReadWrite(*pBuf);
		 pBuf ++;
	}
	CSN_H();           //关闭SPI
	return(status);    // 
}


//***********************************************************************************************************
//*函数：void nRF24L01_TxPacket(unsigned char * tx_buf)
//*功能：发送 tx_buf中数据
//**********************************************************************************************************/
void nRF24L01_TxPacket(unsigned char * tx_buf)
{
	rf_pin_ce_low();			//StandBy I模式	
	SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH); 			 // 装载数据	
	rf_pin_ce_high();		 //置高CE，激发数据发送
}

//****************************************************************************************************/
//*函数：void SetRX_Mode(void)
//*功能：数据接收配置 
//****************************************************************************************************/


void rf_startReceive(void)
{
    rf_pin_ce_low();
#ifndef RF_USE_ACK_PAYLOAD
	SPI_WR_Reg(WRITE_REG + CONFIG, 0x0F);
#endif
    rf_pin_ce_high();
}

void rf_stopReceive(void)
{
    rf_pin_ce_low();
}



/*
 * 发送数据。
 * 返回值：
 *  0：已经送到RF芯片。
 *  1：RF芯片缓冲区已满。*/
u8 rf_transmit(u8 * data,u8 len)
{
	u8 status;
    /*
     * 获取状态。*/

//	status=SPI_RD_Reg(NOP);
	status=SPI_RD_Reg(NRFRegSTATUS);

    /*
     * 判断是否有空位。*/
    if(status & 0x01)	   //RF_REG_STATUS_TX_FULL
        return 1;
    //

    rf_pin_ce_low();
#ifdef RF_USE_ACK_PAYLOAD
    SPI_Write_Buf(WR_ACK_PLOAD,data,len);
#else
   	SPI_WR_Reg(WRITE_REG + CONFIG, 0x0E);  //TX


	SPI_Write_Buf(WR_TX_PLOAD,data,len);
#endif
    rf_pin_ce_high();
    //
    return 0;
}



void rf_checkEvent(void)
{
 	u8 status;
	u8 rx_buf[32]={0};

 	
	status=SPI_RD_Reg(NRFRegSTATUS);	// 读取状态寄存其来判断数据接收状况

        /*
         * 收到数据。 */
        if(status & 0x40)
        {


			rf_stopReceive();
			nrf_length=SPI_RD_Reg(RD_RX_PL_WID);
		 	if((nrf_length>0)&&(nrf_length<33))
		 	SPI_Read_Buf(RD_RX_PLOAD,rx_buf,nrf_length);// read receive payload from RX_FIFO buffer

			interpreter_rf2serial(rx_buf,nrf_length);
 
			rf_startReceive();
        }
        /*
         * 发送完数据。 */
        if(status & 0x20)
        {
            /*
             * 没有等待的数据就停止发送。 */

		  if(status & 0x01)	rf_pin_ce_low();      
          rf_startReceive();
        }
        /*
         * 重发次数超额。 */
        if(status & 0x10)
        {
            /*
             * 清空缓冲区。*/
          	SPI_WR_Reg(FLUSH_TX,0xFF);
			rf_startReceive();
        }
        /*
         * 取消中断标识。*/

		SPI_WR_Reg(WRITE_REG+NRFRegSTATUS, status);
			
  
}

