/*******************************************************************************************************
**------------------------------------------------------------------------------------------------------
** 文件	: 	SPI.c			
** 版本	: 	v1.0 
**------------------------------------------------------------------------------------------------------
** 描述	:	
		本文件为STM32—SPI驱动程序
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "config.h"

//-----------------------------------------------------------------------------------
//SPI口引脚定义   注：SPI为复用借口CS各自定义
#define  SPI1_SCLK 		PAout(5)
#define  SPI1_MISO		PAin (6)   
#define  SPI1_MOSI 		PAout(7)

#define  SPI2_SCLK 		PBout(13)
#define  SPI2_MISO		PBin (14)   
#define  SPI2_MOSI 		PBout(15)

#define  SPI3_SCLK 		PBout(3)
#define  SPI3_MISO		PBin (4)   
#define  SPI3_MOSI 		PBout(5)

/*******************************************************************************************************
** 函数: SPI_Del, SPI延时函数
**------------------------------------------------------------------------------------------------------
** 参数: NONE 
** 返回: NONE
*******************************************************************************************************/
void SPI_Del(unsigned long ms)
{
	while(ms--);
}

/*******************************************************************************************************
** 函数: SPI1_Init, SPI2初始化
**------------------------------------------------------------------------------------------------------
** 参数: NONE 
** 返回: NONE
*******************************************************************************************************/
void SPI1_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;	
	//------------------------------------------------------------------------------------------------
	// 配置IO和时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  , ENABLE);	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5 ;					// SPI1_SCLK时钟   	->PA5
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;				// 推完输出模式 	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);								// 初始化低电平
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 ;					// SPI1_MISO主入从出	->PA6
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;			// 浮空输入模式 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);				   
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;						// SPI1_MOSI主出从入	->PA7
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;				// 推完输出模式 	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);								// 初始化低电平
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


/*******************************************************************************************************
** 函数: SPI1_ReadWrite, SPI发送一个字节，同时接收一个字节
**------------------------------------------------------------------------------------------------------
** 参数: dat 发送数据 
** 返回: 接收数据
*******************************************************************************************************/
unsigned char SPI1_ReadWrite(unsigned char  dat)
{
	unsigned char i,temp=0 ;											 
	for(i=0;i<8;i++)
	{
		SPI1_MOSI=(  dat<<i )& 0x80 ? 1:0;
		SPI1_SCLK = 1;		
		temp=( temp<<1 )| SPI1_MISO;
		SPI1_SCLK = 0;		 
	}
	return temp;
}
/*******************************************************************************************************
** 函数: SPI2_Init, SPI2初始化
**------------------------------------------------------------------------------------------------------
** 参数: NONE 
** 返回: NONE
*******************************************************************************************************/
void SPI2_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;	
	//------------------------------------------------------------------------------------------------
	// 配置IO和时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  , ENABLE);	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 ;					// SPI2_SCLK时钟   	->PB13
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;				// 推完输出模式	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;				
	GPIO_ResetBits(GPIOB,GPIO_Pin_13);								// 初始化低电平
	GPIO_Init(GPIOB, &GPIO_InitStructure);							
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14 ;					// SPI2_MISO主入从出	->PB14
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;			// 浮空输入模式 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);				   
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;					// SPI2_MOSI主出从入	->PB15
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;				// 推完输出模式	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		
	GPIO_Init(GPIOB, &GPIO_InitStructure);							// 初始化低电平
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);
}

/*******************************************************************************************************
** 函数: SPI2_ReadWrite, SPI发送一个字节，同时接收一个字节
**------------------------------------------------------------------------------------------------------
** 参数: dat 发送数据 
** 返回: 接收数据
*******************************************************************************************************/
unsigned char SPI2_ReadWrite(unsigned char  dat)
{
	unsigned char i,temp=0 ;											 
	for(i=0;i<8;i++)
	{
		SPI2_MOSI=(  dat<<i )& 0x80 ? 1:0;
		SPI2_SCLK = 1;		
		SPI_Del(0x10);
		temp=( temp<<1 )| SPI2_MISO;
		SPI2_SCLK = 0;		 
		SPI_Del(0x10);
	}
	return temp;
}


/*******************************************************************************************************
** 函数: SPI3_Init, SPI2初始化
**------------------------------------------------------------------------------------------------------
** 参数: NONE 
** 返回: NONE
*******************************************************************************************************/
void SPI3_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;	
	//------------------------------------------------------------------------------------------------
	// 配置IO和时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  , ENABLE);	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3 ;					// SPI2_SCLK时钟   	->PB13
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;				// 推完输出模式	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_ResetBits(GPIOB,GPIO_Pin_3);								// 初始化低电平
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4 ;					// SPI2_MISO主入从出	->PB14
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;			// 浮空输入模式 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);				   
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;						// SPI2_MOSI主出从入	->PB15
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;				// 推完输出模式		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);								// 初始化低电平
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************************************
** 函数: SPI3_ReadWrite, SPI发送一个字节，同时接收一个字节
**------------------------------------------------------------------------------------------------------
** 参数: dat 发送数据 
** 返回: 接收数据
*******************************************************************************************************/
unsigned char SPI3_ReadWrite(unsigned char  dat)
{
	unsigned char i,temp=0 ;											 
	for(i=0;i<8;i++)
	{
		SPI3_MOSI=(  dat<<i )& 0x80 ? 1:0;
		SPI3_SCLK = 1;		
		SPI_Del(0x10);
		temp=( temp<<1 )| SPI3_MISO;
		SPI3_SCLK = 0;		 
		SPI_Del(0x10);
	}
	return temp;
}

/********************************************************************************************************
**                            End Of File
********************************************************************************************************/
