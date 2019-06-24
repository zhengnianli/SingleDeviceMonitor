/*******************************************************************************************************
**------------------------------------------------------------------------------------------------------
** 文件: RemapGPIO.h
** 版本: v1.0
**------------------------------------------------------------------------------------------------------
** 描述:
**     GPIO重映射宏
**	   参考原点电子;添加其余所有端口的配置,在某些对操作效率有要求的驱动建议采用这种方式
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "stm32f10x.h"

//-------------------------------------------------------------------------------
// STM32位带区详解
/*	
**						 
**				
**							  位带区					
**	STM32中具有两个区实现了位带：SRAM	0x20000000	-	0x200FFFFF(1MB)
**								 FLASH 	0x40000000	-	0x400FFFFF(1MB)
**							
**							 位带别名区
**	位带区中的地址除了可以同RAM一样使用外，还带有自己的“位带别名”。将每个
**位拓展为32位的字，既成了位带别名。
**
**	如：0x20000000（一个字节）拓展到32位可得8个32位的字分别是：
**		0x22000000	0x22000004	0x22000008	0x2200000c
**		0x22000010	0x22000014	0x22000018	0x2200001c
**
**	由此可得“位带区” 上任意地址的 “位带别名地址” 
**
**	SRAM  :		0x22000000 +((A‐0x20000000)*8+n)*4
**	FLASH :		0x42000000 +((A‐0x40000000)*8+n)*4
**	既	  :		
**				0x22000000+ (A‐0x20000000)*32 + n*4
**				0x42000000+ (A‐0x40000000)*32 + n*4
**				别名区地址 = 位带别名区起始地址+偏移量
**
**	备注  ：	字节的地址：	A
**				位序号	  ：	n	（0<n<7）
**	
**	例	  ：	0x22000000 + ((0x20000000 - 0x20000000 )*8+4)*4
**			 =  0x22000010 		
**	既	  :		0x20000000第4位带别名区地址
**
**  "位带地址＋位序号" 转换别名地址 
**	#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
*/
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// IO操作宏
#define BITBAND(addr, bitnum) 		((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))		// 计算bit区地址偏移
#define MEM_ADDR(addr)  			(*((vu32  *)(addr) ))												  	// 强制转换为指针
#define BIT_ADDR(addr, bitnum)  	MEM_ADDR( BITBAND(addr, bitnum)  )

//-------------------------------------------------------------------------------
// IO口地址映射
#define GPIOA_ODR_Addr  (GPIOA_BASE+12) 					// 0x4001080C 
#define GPIOB_ODR_Addr  (GPIOB_BASE+12) 					// 0x40010C0C 
#define GPIOC_ODR_Addr  (GPIOC_BASE+12) 					// 0x4001100C 
#define GPIOD_ODR_Addr  (GPIOD_BASE+12) 					// 0x4001140C 
#define GPIOE_ODR_Addr  (GPIOE_BASE+12) 					// 0x4001180C 
#define GPIOF_ODR_Addr  (GPIOF_BASE+12) 					// 0x40011A0C    
#define GPIOG_ODR_Addr  (GPIOG_BASE+12) 					// 0x40011E0C    

#define GPIOA_IDR_Addr  (GPIOA_BASE+8) 						// 0x40010808 
#define GPIOB_IDR_Addr  (GPIOB_BASE+8) 						// 0x40010C08 
#define GPIOC_IDR_Addr  (GPIOC_BASE+8) 						// 0x40011008 
#define GPIOD_IDR_Addr  (GPIOD_BASE+8) 						// 0x40011408 
#define GPIOE_IDR_Addr  (GPIOE_BASE+8) 						// 0x40011808 
#define GPIOF_IDR_Addr  (GPIOF_BASE+8) 						// 0x40011A08 
#define GPIOG_IDR_Addr	(GPIOG_BASE+8) 						// 0x40011E08 

//-------------------------------------------------------------------------------
// IO口操作,只对单一的IO口;确保n的值小于16!
#define PAout(n)   		BIT_ADDR(GPIOA_ODR_Addr,n)  		// 输出 
#define PAin(n)    		BIT_ADDR(GPIOA_IDR_Addr,n)  		// 输入 

#define PBout(n)   		BIT_ADDR(GPIOB_ODR_Addr,n)  		// 输出 
#define PBin(n)    		BIT_ADDR(GPIOB_IDR_Addr,n)  		// 输入 

#define PCout(n)   		BIT_ADDR(GPIOC_ODR_Addr,n)  		// 输出 
#define PCin(n)    		BIT_ADDR(GPIOC_IDR_Addr,n)  		// 输入 

#define PDout(n)   		BIT_ADDR(GPIOD_ODR_Addr,n)  		// 输出 
#define PDin(n)    		BIT_ADDR(GPIOD_IDR_Addr,n)  		// 输入 

#define PEout(n)   		BIT_ADDR(GPIOE_ODR_Addr,n)  		// 输出 
#define PEin(n)    		BIT_ADDR(GPIOE_IDR_Addr,n)  		// 输入

#define PFout(n)   		BIT_ADDR(GPIOF_ODR_Addr,n)  		// 输出 
#define PFin(n)    		BIT_ADDR(GPIOF_IDR_Addr,n)  		// 输入

#define PGout(n)   		BIT_ADDR(GPIOG_ODR_Addr,n)  		// 输出 
#define PGin(n)    		BIT_ADDR(GPIOG_IDR_Addr,n)  		// 输入

/********************************************************************************************************
**                            End Of File
********************************************************************************************************/
