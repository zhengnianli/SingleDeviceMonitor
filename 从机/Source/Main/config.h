/*******************************************************************************************************
**------------------------------------------------------------------------------------------------------
** 文件: config.h 
** 版本: v1.0
**------------------------------------------------------------------------------------------------------
** 描述:
**      用于存放开发工程的全局变量
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef __CONFIG__H__
#define __CONFIG__H__

//----------------------------------------------------------------
//无符号类型宏定义
#define uchar 	unsigned char													 
#define uint 	unsigned int

#define uint8 	unsigned char 
#define uint16 	unsigned short int
#define uint32 	unsigned int

//----------------------------------------------------------------
// MAIN_GLOBAL宏	main.c 定义#define  MAIN_GLOBAL
#ifdef 	 MAIN_CONFIG										// 头文件被多个C文件调用,避免变量冲突的问题 
	#define  EXT											// 主文件中定义的变量
#else
	#define  EXT	extern									// 全局文件变量的声明
#endif
	
//----------------------------------------------------------------
//包含常用头文件  
#include "stm32f10x.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "system.h"
#include "RemapGPIO.h"

//---------------------------------------------------------------
//包含驱动头文件
#include "Usart.h"
#include "Queue.h"
#include "Timer.h"
#include "SysTick.h"
#include "Button.h"
#include "usart2_HMI.h"	
#include "I2C.h"
#include "AT24C02.h"
#include "usart3_ZigBee.h"
//---------------------------------------------------------------
//包含全局变量
extern const uchar	 gImage_Back[192000];

//---------------------------------------------------------------
//包含全局宏定义
#define CPU_INT_DISABLE()   { __set_PRIMASK(1); }		// 关闭总中断 
#define CPU_INT_ENABLE()    { __set_PRIMASK(0);	} 		// 开启总中断

//---------------------------------------------------------------
//包含全局类型定义
typedef enum{FALSE=0,TRUE=!FALSE,}BOOL;					// 成功or失败

//---------------------------------------------------------------
//包含全局系统定时器
#define  MAX_TIMER  		4							// 最大定时器个数
EXT volatile unsigned long  g_dwSysTimer[MAX_TIMER];	// 定义系统定时器 
#define  Char_wait		g_dwSysTimer[0]					// 串口数据接收定时器
#define  DelayTimer  	g_dwSysTimer[1]	 				// 延时定时器
#define  ViewTimer  	g_dwSysTimer[2]	 				// 液晶定时器
#define  PutEmtTimer  	g_dwSysTimer[3]	 				// 定时器1

#define  TIMER_SEC  		(1000)							// 秒
#define  TIMER_MIN  		(SEC*60)						// 分
#define  TIMER_HOUR  		(MIN*60)						// 时
#define  TIMER_DAY  		(HOUR*24)						// 天
#endif

/********************************************************************************************************
**                            End Of File
********************************************************************************************************/

