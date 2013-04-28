#if !defined( _NRF24L01_H )
#define _NRF24L01_H

#define NRF24_TX_Mode  TX_Mode
#define NRF24_RX_Mode  RX_Mode

void init_NRF24L01(void);
void TX_Mode(void);
void RX_Mode(void);

void NRF_Send(void);
void NRF_Receive(void);
void NRF_Receive_and_Send(void);
void nRF24L01_TxPacket(unsigned char * tx_buf);
u8 nRF24L01_RxPacket(unsigned char* rx_buf);

u8 SPI_WR_Reg(u8 reg, u8 value);
u8 SPI_Read_Buf(u8 reg, u8 *pBuf, u8 Len);
u8 SPI_Write_Buf(u8 reg, u8 *pBuf, u8 Len);
u8 nRF24L01_RxPacket(unsigned char* rx_buf);
void nRF24L01_TxPacket(unsigned char * tx_buf);
u8 SPI_RD_Reg(u8 reg);

u8 nRF24L01_Rx_With_ACK(unsigned char* rx_buf);
u8 rf_transmit(u8 * data,u8 len);
void rf_checkEvent(void);

#endif

