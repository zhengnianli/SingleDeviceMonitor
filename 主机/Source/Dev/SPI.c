/*******************************************************************************************************
**------------------------------------------------------------------------------------------------------
** �ļ�	: 	SPI.c			
** �汾	: 	v1.0 
**------------------------------------------------------------------------------------------------------
** ����	:	
		���ļ�ΪSTM32��SPI��������
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "config.h"

//-----------------------------------------------------------------------------------
//SPI�����Ŷ���   ע��SPIΪ���ý��CS���Զ���
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
** ����: SPI_Del, SPI��ʱ����
**------------------------------------------------------------------------------------------------------
** ����: NONE 
** ����: NONE
*******************************************************************************************************/
void SPI_Del(unsigned long ms)
{
	while(ms--);
}

/*******************************************************************************************************
** ����: SPI1_Init, SPI2��ʼ��
**------------------------------------------------------------------------------------------------------
** ����: NONE 
** ����: NONE
*******************************************************************************************************/
void SPI1_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;	
	//------------------------------------------------------------------------------------------------
	// ����IO��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA  , ENABLE);	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5 ;					// SPI1_SCLKʱ��   	->PA5
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;				// �������ģʽ 	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_ResetBits(GPIOA,GPIO_Pin_5);								// ��ʼ���͵�ƽ
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 ;					// SPI1_MISO����ӳ�	->PA6
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;			// ��������ģʽ 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);				   
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;						// SPI1_MOSI��������	->PA7
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;				// �������ģʽ 	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_ResetBits(GPIOA,GPIO_Pin_7);								// ��ʼ���͵�ƽ
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}


/*******************************************************************************************************
** ����: SPI1_ReadWrite, SPI����һ���ֽڣ�ͬʱ����һ���ֽ�
**------------------------------------------------------------------------------------------------------
** ����: dat �������� 
** ����: ��������
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
** ����: SPI2_Init, SPI2��ʼ��
**------------------------------------------------------------------------------------------------------
** ����: NONE 
** ����: NONE
*******************************************************************************************************/
void SPI2_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;	
	//------------------------------------------------------------------------------------------------
	// ����IO��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  , ENABLE);	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 ;					// SPI2_SCLKʱ��   	->PB13
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;				// �������ģʽ	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;				
	GPIO_ResetBits(GPIOB,GPIO_Pin_13);								// ��ʼ���͵�ƽ
	GPIO_Init(GPIOB, &GPIO_InitStructure);							
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14 ;					// SPI2_MISO����ӳ�	->PB14
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;			// ��������ģʽ 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);				   
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;					// SPI2_MOSI��������	->PB15
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;				// �������ģʽ	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;		
	GPIO_Init(GPIOB, &GPIO_InitStructure);							// ��ʼ���͵�ƽ
	GPIO_ResetBits(GPIOB,GPIO_Pin_15);
}

/*******************************************************************************************************
** ����: SPI2_ReadWrite, SPI����һ���ֽڣ�ͬʱ����һ���ֽ�
**------------------------------------------------------------------------------------------------------
** ����: dat �������� 
** ����: ��������
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
** ����: SPI3_Init, SPI2��ʼ��
**------------------------------------------------------------------------------------------------------
** ����: NONE 
** ����: NONE
*******************************************************************************************************/
void SPI3_Init(void)
{
 	GPIO_InitTypeDef GPIO_InitStructure;	
	//------------------------------------------------------------------------------------------------
	// ����IO��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB  , ENABLE);	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3 ;					// SPI2_SCLKʱ��   	->PB13
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;				// �������ģʽ	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_ResetBits(GPIOB,GPIO_Pin_3);								// ��ʼ���͵�ƽ
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4 ;					// SPI2_MISO����ӳ�	->PB14
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;			// ��������ģʽ 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);				   
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;						// SPI2_MOSI��������	->PB15
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;				// �������ģʽ		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_ResetBits(GPIOB,GPIO_Pin_5);								// ��ʼ���͵�ƽ
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/*******************************************************************************************************
** ����: SPI3_ReadWrite, SPI����һ���ֽڣ�ͬʱ����һ���ֽ�
**------------------------------------------------------------------------------------------------------
** ����: dat �������� 
** ����: ��������
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
