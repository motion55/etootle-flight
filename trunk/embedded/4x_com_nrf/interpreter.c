/* interpreter.c
 * 2012-9-13 16:23:12
 * js200300953
 */

#include "interpreter.h"
#include "user_uart.h"
#include "nrf24l01.h"
#include "led.h"

#define INTERPRETER_BUFFER_SIZE 200
#define INTERPRETER_ST_FINDING_55 100	 //0x64
#define INTERPRETER_ST_NEED_AA 101		 //0x65
#define INTERPRETER_ST_MAX_LEN 32		 //0x20

static volatile uint8_t interpreter_buffer[INTERPRETER_BUFFER_SIZE]; // 缓冲区。
static volatile uint8_t interpreter_bufferHead = 0; // 缓冲区头索引，指向下一个空闲单元。
static volatile uint8_t interpreter_bufferTail = 0; // 缓冲区尾索引，指向最后一个有效单元。
static volatile uint8_t interpreter_status = INTERPRETER_ST_FINDING_55; //状态。

void serial_received(uint8_t data);
void interpreter_rf2serial(const void * rfData,uint8_t len);
uint8_t interpreter_getBufferValidDataLen(void);
void inpertreter_checkFrame(void);

void serial_received(uint8_t data)
{
	// 计算下一个head的值，用来判断缓冲区是否满。。
	uint8_t head = interpreter_bufferHead + 1;
	if(head >= INTERPRETER_BUFFER_SIZE)
		head = 0;
	//
	// 如果计算出的head等于tail，则缓冲区满，丢弃数据。
	if(head == interpreter_bufferTail)
		return;
	//
	// 把值放入缓冲区。
	interpreter_buffer[interpreter_bufferHead] = data;
	interpreter_bufferHead = head;
}

// 把nRF24L01+接收到的数据发到串口。
void interpreter_rf2serial(const void * rfData,uint8_t len)
{
	uint8_t frameHead[] = {0x55,0xAA,0x00};

	frameHead[2]=len;
	serial_transmit(frameHead,sizeof(frameHead));
	serial_transmit(rfData,len);
}

// 获取缓冲区中，有效数据的长度。
uint8_t interpreter_getBufferValidDataLen(void)
{
	if(interpreter_bufferTail <= interpreter_bufferHead)
		return interpreter_bufferHead - interpreter_bufferTail;
	return INTERPRETER_BUFFER_SIZE - interpreter_bufferTail + interpreter_bufferHead;
}

// 重缓冲区获取一个字节。调用时必须保证有数据。
uint8_t interpreter_getAByte(void)
{
	uint8_t rt =  interpreter_buffer[interpreter_bufferTail];
	interpreter_bufferTail ++;
	if(interpreter_bufferTail >= INTERPRETER_BUFFER_SIZE)
		interpreter_bufferTail = 0;
	//
	return rt;
}

// 检查是否收到一个帧，在main里循环调用。
void inpertreter_checkFrame(void)
{
uint8_t i;
uint8_t rfBuf[INTERPRETER_ST_MAX_LEN];

	const uint8_t validLen = interpreter_getBufferValidDataLen();
	//
	if(interpreter_status == INTERPRETER_ST_FINDING_55)
	{
		// 先要判断是否有足够的数据。
		if(validLen < 1)
			return;
		if(interpreter_getAByte() == (uint8_t)0x55)
			interpreter_status = INTERPRETER_ST_NEED_AA;
	}
	else if(interpreter_status == INTERPRETER_ST_NEED_AA)
	{
		if(validLen < 2)
			return;
		if(interpreter_getAByte() == (uint8_t)0xAA)
		{
			// 获取帧长度。
			interpreter_status = interpreter_getAByte();
			//
			// 判断长度有效性。
			if(interpreter_status > INTERPRETER_ST_MAX_LEN)
				interpreter_status = INTERPRETER_ST_FINDING_55;
		}
		else
		// 如果状态是NEED_AA，而实际不是，就重新FINDING_55。
			interpreter_status = INTERPRETER_ST_FINDING_55;
	}
	else if(interpreter_status <= INTERPRETER_ST_MAX_LEN)
	{
		if(validLen < interpreter_status)
			return;

		for(i=0;i<interpreter_status;i++)
			rfBuf[i] = interpreter_getAByte();

		rf_transmit(rfBuf,interpreter_status);
		interpreter_status = INTERPRETER_ST_FINDING_55;
	}
	else

		interpreter_status = INTERPRETER_ST_FINDING_55;
}
