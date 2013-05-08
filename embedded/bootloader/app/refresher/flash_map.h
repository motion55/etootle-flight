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

// app/refresher/flash_map.h
// 2013-2-21 21:13:53
// js200300953

#ifndef __FILE_APP_REFRESHER_FLASHMAP_H__
#define __FILE_APP_REFRESHER_FLASHMAP_H__

/*

Flash一共64kB，分成5份，开头为15kB的bootloader，紧接着是1kB的param，剩下的是48kB的app+applen+appcrc。
param为供app使用的Flash。
  --------------------------
        Bootloader
   [0,16kB)          : 16kB
  --------------------------
          AppCode
   [16kB,63kB-8B)    : 47kB-8B
  --------------------------
           AppLen
   [63kB-8B,63kB-4B)  : 4B
  --------------------------
            AppCrc
   [63kB-4B,63kB)     : 4B
  --------------------------
           Param
   [63kB,64kB)       : 1kB
  --------------------------

变量和常量名最后一部分为计数类型
 _BIT   => 位计数
 _BYTE  => 字节计数
 _BLOCK => 块计数
 _PAGE  => 页计数

*/

#include <stdint.h>

typedef struct {uint8_t bytes[1024];} page_t;

// FLASH开始地址。
#define MAP_FLASH_BASE_PAGE     ((page_t *)0x08000000)
#define MAP_FLASH_BASE_BLOCK    ((uint32_t *)MAP_FLASH_BASE_PAGE)
#define MAP_FLASH_BASE_BYTE     ((uint8_t *) MAP_FLASH_BASE_PAGE)

// 块大小，固定为4字节，不能改变，因为Flash写入以字为单位。
#define MAP_BLOCK_SIZE_BYTE (4)
#define MAP_BLOCK_SIZE_BIT  (MAP_BLOCK_SIZE_BYTE*8)

// FLASH页大小，擦除以页为单位，STM32F103R8的为1kB。
#define MPA_FLASH_PAGE_SIZE_BYTE (sizeof(page_t))
#define MPA_FLASH_PAGE_SIZE_BLOCK (MPA_FLASH_PAGE_SIZE_BYTE/MAP_BLOCK_SIZE_BYTE)

// bootloader段的基址和大小。
#define MAP_BOOTLOADER_BASE_PAGE    (MAP_FLASH_BASE_PAGE)
#define MAP_BOOTLOADER_BASE_BLOCK   ((uint32_t *)MAP_BOOTLOADER_BASE_PAGE)
#define MAP_BOOTLOADER_BASE_BYTE    ((uint8_t  *)MAP_BOOTLOADER_BASE_PAGE)
#define MAP_BOOTLOADER_SIZE_PAGE    (16) // 16page = 16kB
#define MAP_BOOTLOADER_SIZE_BLOCK   (MAP_BOOTLOADER_SIZE_PAGE*MPA_FLASH_PAGE_SIZE_BLOCK)
#define MAP_BOOTLOADER_SIZE_BYTE    (MAP_BOOTLOADER_SIZE_PAGE*MPA_FLASH_PAGE_SIZE_BYTE)

// app段的基址和大小。
#define MAP_APP_BASE_PAGE    (MAP_BOOTLOADER_BASE_PAGE+MAP_BOOTLOADER_SIZE_PAGE)
#define MAP_APP_BASE_BLOCK   ((uint32_t *)MAP_APP_BASE_PAGE)
#define MAP_APP_BASE_BYTE    ((uint8_t  *)MAP_APP_BASE_PAGE)
#define MAP_APP_SIZE_PAGE    (47) // 47page = 47kB
#define MAP_APP_SIZE_BLOCK   (MAP_APP_SIZE_PAGE*MPA_FLASH_PAGE_SIZE_BLOCK)
#define MAP_APP_SIZE_BYTE    (MAP_APP_SIZE_PAGE*MPA_FLASH_PAGE_SIZE_BYTE)

// app的有效长度和CRC的存放的地址
#define MPA_APP_VALID_BLOCK_AMOUNT_ADDR_UINT32 ((uint32_t *)(MAP_FLASH_BASE_BYTE + 63*1024 - 8))
#define MPA_APP_CRC_ADDR_UINT32                ((uint32_t *)(MAP_FLASH_BASE_BYTE + 63*1024 - 4))

// param段的基址和大小。
#define MAP_PARAM_BASE_PAGE    (MAP_FLASH_BASE_PAGE+63)
#define MAP_PARAM_BASE_BLOCK   ((uint32_t *)MAP_PARAM_BASE_PAGE)
#define MAP_PARAM_BASE_BYTE    ((uint8_t  *)MAP_PARAM_BASE_PAGE)
#define MAP_PARAM_SIZE_PAGE    (1) // 1page = 1kB
#define MAP_PARAM_SIZE_BLOCK   (MAP_PARAM_SIZE_PAGE*MPA_FLASH_PAGE_SIZE_BLOCK)
#define MAP_PARAM_SIZE_BYTE    (MAP_PARAM_SIZE_PAGE*MPA_FLASH_PAGE_SIZE_BYTE)

#endif // __FILE_APP_REFRESHER_FLASHMAP_H__
