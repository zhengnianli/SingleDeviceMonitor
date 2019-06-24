/*******************************************************************************************************
**------------------------------------------------------------------------------------------------------
** 文件	: 	usart3_ZigBee.c			
** 版本	: 	v1.0 
**------------------------------------------------------------------------------------------------------
** 描述	:	
		本文件为ZigBee驱动程序
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "config.h"
#include "usart3_ZigBee.h"
#include "stdarg.h"	 

//一些全局变量定义 
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 			//接收缓冲,最大USART3_MAX_RECV_LEN个字节.
u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 			//发送缓冲,最大USART3_MAX_SEND_LEN字节
u8 wait_time = 0;
vu16 USART3_RX_STA = 0;  
u8 ElecNum=1;  //用电器编号
char STa_Store[20],U_Store[20],I_Store[20],P_Store[20],Q_Store[20],ElecNum_Store[20];
float U,I,P,Q;

/*******************************************************************************************************
** 函数: uart3_ZigBee_init，zigbee模块初始化
**------------------------------------------------------------------------------------------------------
** 参数: bound：波特率
** 返回: 无
** 说明: 
********************************************************************************************************/
void uart3_ZigBee_init(u32 bound)
{  	 
	//GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能UGPIOB时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//使能USART3时钟
	//USART3_TX  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);
   
    //USART3_RX	  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //Usart2 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
   //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART3, &USART_InitStructure);     //初始化串口3
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_Cmd(USART3, ENABLE);                    //使能串口3
	
	TIM3_Int_Init(99,7199);		//10ms中断
	USART3_RX_STA=0;		//清零
	TIM_Cmd(TIM3,DISABLE);			//关闭定时器7
}

/*******************************************************************************************************
** 函数: u3_printf，串口3 ,printf 函数
**------------------------------------------------------------------------------------------------------
** 参数: 变参函数
** 返回: 无
** 说明: 确保一次发送数据不超过USART3_MAX_SEND_LEN字节
********************************************************************************************************/
void u3_printf(char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);		//此次发送数据的长度
	for (j = 0; j < i; j++)							//循环发送数据
	{
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
		USART_SendData(USART3,USART3_TX_BUF[j]); 
	} 
}