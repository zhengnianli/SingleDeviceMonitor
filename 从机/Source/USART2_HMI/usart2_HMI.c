/*******************************************************************************************************
**------------------------------------------------------------------------------------------------------
** 文件	: 	usart2_HMI.c			
** 版本	: 	v1.0 
**------------------------------------------------------------------------------------------------------
** 描述	:	
		本文件串口屏驱动程序
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "config.h"
#include "usart2_HMI.h"

unsigned char HMI_receive_len = 0;	// 串口接收数据数组的长度
unsigned char HMI_receive[32];	    // 串口接收缓冲数组

/*******************************************************************************************************
** 函数: uart2_HMI_init，串口屏硬件初始化
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: 无
** 说明: 
********************************************************************************************************/
void uart2_HMI_init(u32 bound)
{  	 
	  //GPIO端口设置
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能UGPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//使能USART2时钟
	//USART2_TX  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART2_RX	  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //Usart2 NVIC 配置
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
   //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
    USART_Init(USART2, &USART_InitStructure);     //初始化串口2
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_Cmd(USART2, ENABLE);                    //使能串口2
}

/*******************************************************************************************************
** 函数: USART2_IRQHandler，串口2中断函数
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: 无
** 说明: 
********************************************************************************************************/
void USART2_IRQHandler(void)
{
	unsigned char key_receive=0;//串口屏接收相关参数
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收到数据
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE); //清除中断标志. 
		key_receive=USART_ReceiveData(USART2); 
		HMI_receive[HMI_receive_len++]=USART_ReceiveData(USART2); 
		switch(key_receive)
		{
			case 1:	 
				printf("接收到按键1的信号\r\n");
			break;
			case 2:	 
				printf("接收到按键2的信号\r\n");
			break;
			case 3:	 
				printf("接收到按键3的信号\r\n");
			break;
			case 4:	 
				printf("接收到按键4的信号\r\n");
			break;
			default:
			break;
		}	  	 	 
	}	
}

/*******************************************************************************************************
** 函数: HMISendByte，连续发送三个字节数据
**------------------------------------------------------------------------------------------------------
** 参数: k
** 返回: 无
** 说明: 连续往串口屏发送3个“0xFF”代表结束
********************************************************************************************************/
void HMISendByte(u8 k)
{
	u8 i;
	for (i = 0; i < 3; i++)
	{
		if (k != 0)
		{
			USART_SendData(USART2, k);                                    //发送字节数据
			while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){} //等待发送结束   
		}
		else
			return;
	}
}

/*******************************************************************************************************
** 函数: HMISendStart，为确保串口HMI正常通信
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: 无
** 说明: 
********************************************************************************************************/
void HMISendStart(void)
{
	SysTickDelayMS(200);
	HMISendByte(0xff);
	SysTickDelayMS(200);
	//打印到page0页面
	HMISendStr("page main");
 	HMISendByte(0xff);
}

/*******************************************************************************************************
** 函数: HMISendStr，向串口屏发送字符串 
**------------------------------------------------------------------------------------------------------
** 参数: k
** 返回: 无
** 说明: 
********************************************************************************************************/
void HMISendStr(char *buf)
{
	u8 i=0;
	while(1)
	{
		if(buf[i]!=0)
		{	
		
			 USART_SendData(USART2, buf[i]);   //发送一个字节
			 while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){} //等待发送结束
			i++;
		}
		else
			return;
	}
}
/********************************************************************************************************
**                            End Of File
********************************************************************************************************/
