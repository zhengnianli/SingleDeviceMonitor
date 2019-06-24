/********************************************************************************************************
**------------------------------------------------------------------------------------------------------
** 文件: Usart.c
** 版本: v1.0
**------------------------------------------------------------------------------------------------------
** 描述:
**     系统调试串口驱动，简化函数数量，防止串口发送陷入死循环
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "config.h"
#include "Usart.h"

//-------------------------------------------------------------------------------
// 共有定义
#define  RECBUF_MAX  	256									// 最大接收数据缓冲区
static Queue_t 	s_tQueueUsart ; 		                    // 调试串口
static char 	s_sRecDatBuf[RECBUF_MAX]={0};				// 队列接收缓存Buff
static char 	s_sUsartBuf [RECBUF_MAX]={0};				// 串口处理缓存Buff

/*******************************************************************************************************
** 函数: UsartInit, 串口1初始化配置
**------------------------------------------------------------------------------------------------------
** 参数: USART1 baudrate(波特率)
** 返回: 无
********************************************************************************************************/
void UsartInit(  unsigned long baudrate )
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//------------------------------------------------------------------------------------------------
	// 配置IO和时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);	// 使能GPIOA时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO  , ENABLE);	// 使能复用功能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	// 使能串口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				// USART1的TX是PA9	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			// 配置成复用输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				// USART1的RX是PA10	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	// 配置成浮空输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//------------------------------------------------------------------------------------------------
	// 配置接收中断	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = Usart_PreemptionPriority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = Usart_SubPriority;	 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

//------------------------------------------------------------------------------------------------
	// 串口参数配置		
	USART_InitStructure.USART_BaudRate = baudrate ;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			// 使能对应的接收中断
	USART_Init(USART1, &USART_InitStructure);				// 初始化串口参数
	USART_Cmd(USART1, ENABLE);								// 使能串口 
	USART_ClearFlag(USART1, USART_FLAG_TC|USART_FLAG_TC);	// 清除串口接收发送标志位

	//------------------------------------------------------------------------------------------------
	// 初始化接收队列
	QueueInit(s_sRecDatBuf, RECBUF_MAX, &s_tQueueUsart);		// Usart串口接收队列初始化
}

/*******************************************************************************************************
** 函数: Usart_SendCh, 对应串口发送字节函数
**------------------------------------------------------------------------------------------------------
** 参数: 数据(字节)
** 返回: 无
********************************************************************************************************/
int Usart_SendCh( unsigned char ch )
{	
	int cnt = 0;
	while( USART_GetFlagStatus(USART1, USART_FLAG_TC) ==RESET ) 		
		if( cnt++ > 0xfff )		return FALSE;
	USART_SendData(USART1,ch);			
	return TRUE;								
}

/*******************************************************************************************************
** 函数: Usart_SendBuf, 串口发送帧
**------------------------------------------------------------------------------------------------------
** 参数: 字符串	, 长度
** 返回: 无
********************************************************************************************************/
int Usart_SendBuf(char *dat , unsigned int len)
{
	while(len--)
	{
		if( Usart_SendCh(*(dat++) ) < 0 )
			return FALSE;
	}			  
	return TRUE;
}	

/*******************************************************************************************************
** 函数: fputc, 重定义putc函数	，使能够调用printf函数从串口打印输出
**------------------------------------------------------------------------------------------------------
** 参数: NONE
** 返回: NONE
********************************************************************************************************/
int fputc(int ch, FILE *f)
{
	return 	Usart_SendCh( ch );
}

/*******************************************************************************************************
** 函数: USART1_IRQHandler, USART1中断服务程序
**------------------------------------------------------------------------------------------------------
** 参数: 字符串	, 长度
** 返回: 无
********************************************************************************************************/
void USART1_IRQHandler(void)
{	
	if( USART_GetFlagStatus(USART1,USART_IT_RXNE) !=RESET )				// 串口接收数据 
	{				
		Char_wait=10;													// 串口数据接收定时器
		QueueInPut( (char)USART_ReceiveData(USART1),&s_tQueueUsart );	// 将取到的数据入库
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);					// 清除接收标志位
	}
	
	if( USART_GetFlagStatus(USART1,USART_FLAG_ORE)==SET ) 				// 串口溢出错误
	{
		USART_ReceiveData(USART1); 										// 读走溢出数据
		USART_ClearFlag(USART1,USART_FLAG_ORE); 						// 清除溢出标志
	}

}

/*******************************************************************************************************
** 函数: UsartReceive, 串口接收并处理
**------------------------------------------------------------------------------------------------------
** 参数: NULL
** 返回: 成功or失败
********************************************************************************************************/
int UsartReceive(void)
{
	unsigned char i=0,len=0;

	//----------------------------------------------------------------
	// 等待数据接收完毕
	if( !s_tQueueUsart.unResidue )			return FALSE;	// 接收到串口数据
	if( Char_wait )							return FALSE;	// 数据接收定时器不为0、退出

	//----------------------------------------------------------------
	// 串口数据获取
	len=s_tQueueUsart.unResidue;							// 取长度
	for( i=0;i<len;i++ )									// 将队列数据读取到串口指令缓存中
	{
		QueueOutPut(&s_sUsartBuf[i] ,&s_tQueueUsart); 	
	}
	s_sUsartBuf[len]=0;										// 若指令为ASCII字符串，为指令添加结束位
	
	//----------------------------------------------------------------
	// 
	if( (strstr(s_sUsartBuf,"Get(U)")) != NULL ){ 
		
		
	}
	else goto Error ;										// 无此关键字退出
	
	//----------------------------------------------------------------
	// 正确回应提示
	
	return TRUE;
		
Error:
	//----------------------------------------------------------------
	// 错误回应提示
	
	return FALSE; 	
}
/********************************************************************************************************
**											End Of File												   
********************************************************************************************************/
