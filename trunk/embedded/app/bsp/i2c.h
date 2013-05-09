//     Copyright (c) 2013 js200300953@qq.com All rights reserved.
//         ========圆点博士微型四轴飞行器配套程序声明========
// 
// 圆点博士微型四轴飞行器配套程序包括上位机程序、下位机Bootloader和
//     下位机App，及它们的源代码，以下总称“程序”。
// 程序由js200300953编写。
// 程序仅为使用者提供参考，js200300953不对程序提供任何明示或暗含的担保，
//     不对在使用该程序中出现的意外或者损失负责，
//     也不对因使用该程序而引起的第三方索赔负责。
// 使用者可以以学习为目的修改和使用该程序，请勿以商业的目的使用该程序。
// 修改该程序时，必须保留原版权声明，并且不能修改原版权声明。
// 
// 更多资料见：
//     http://blog.sina.com.cn/js200300953
//     http://www.etootle.com/
//     http://www.amobbs.com/thread-5504090-1-1.html
//     圆点博士微型四轴飞行器QQ群：276721324

// bsp/i2c.h
// 2013-1-7 18:07:22
// js200300953

#ifndef __FILE_BSP_I2C_H__
#define __FILE_BSP_I2C_H__

#include "stm32f10x.h"

void i2cInit(void);
int32_t i2cRead(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf);
int32_t i2cWriteBuffer(uint8_t addr, uint8_t reg, uint8_t len, uint8_t * data);

#define i2c_init() i2cInit()
#define i2c_write(dev,reg,data,len) (!i2cWriteBuffer((dev)>>1,reg,len,(uint8_t *)data))
#define i2c_read(dev,reg,data,len) (!i2cRead((dev)>>1,reg,len,(uint8_t *)data))

/*
// bw for busy wait
void i2c_bw_init(void);
uint32_t i2c_bw_write(uint8_t dev,uint8_t reg,const void * data,uint32_t len);
uint32_t i2c_bw_read(uint8_t dev,uint8_t reg,void * data,uint32_t len);
*/

#endif // __FILE_BSP_I2C_H__
