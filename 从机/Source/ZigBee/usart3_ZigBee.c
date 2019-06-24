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

/*******************************************************************************************************
** 函数: USART3_IRQHandler，串口3中断函数
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: void
** 说明: 
********************************************************************************************************/
void USART3_IRQHandler(void)
{
	u8 res;
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//接收到数据
	{	 
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		res = USART_ReceiveData(USART3);	
		if (USART3_RX_STA < 598)
		{
			USART3_RX_BUF[USART3_RX_STA++] = res;	//记录接收到的值	
			USART3_RX_BUF[USART3_RX_STA] = 0;
		}
		wait_time=5;	
	}  				 											 
}   

/*******************************************************************************************************
** 函数: DataHandle，串口3接收数据处理函数
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: void
** 说明: 
********************************************************************************************************/
void Disp_Sta(void)
{
	int uea1_sta,uea2_sta,uea3_sta,uea4_sta,uea5_sta,uea6_sta,uea7_sta;	
	
	if (USART3_RX_STA == 0)
	{
		return ;
	}
	if (wait_time != 0)
	{
		return;
	}
	printf("数据包：%s", USART3_RX_BUF);
	printf("\n");
	if(sscanf((char*)USART3_RX_BUF,"A:U:%f,I:%f,P:%f,Q:%f,UEA1:%1d,UEA2:%1d,UEA3:%1d,UEA4:%1d,UEA5:%1d,UEA6:%1d,UEA7:%1d\r\n",
			&U,&I,&P,&Q,&uea1_sta,&uea2_sta,&uea3_sta,&uea4_sta,&uea5_sta,&uea6_sta,&uea7_sta)==11)	
	{
		printf("电源线参数：U=%0.1fV,I=%0.2fA,P=%0.1fW,Q=%0.1fVar\n",U,I,P,Q);
		printf("7个用电器状态：%d,%d,%d,%d,%d,%d,%d\n",uea1_sta,uea2_sta,uea3_sta,uea4_sta,uea5_sta,uea6_sta,uea7_sta);
		if (P > 1000)
		{
			uea7_sta = 1;
		}
		//----------------------------------------------------------------------------------------------- 
		//发送数据至串口屏进行显示
		//电压
		sprintf(U_Store,"t11.txt=\"%0.1fV\"",U);
		HMISendStr(U_Store);
		HMISendByte(0xff);
		// 电流
		sprintf(I_Store,"t12.txt=\"%0.3fA\"",I);
		HMISendStr(I_Store);
		HMISendByte(0xff);
		//有功功率
		sprintf(P_Store,"t13.txt=\"%0.3fW\"",P);
		HMISendStr(P_Store);
		HMISendByte(0xff);
		//无功功率
		sprintf(Q_Store,"t14.txt=\"%0.2fVar\"",Q);
		HMISendStr(Q_Store);
		HMISendByte(0xff);
		
		//用电器1
		sprintf(STa_Store,"t0.txt=\"%d\"",uea1_sta);
		HMISendStr(STa_Store);
		HMISendByte(0xff);
		//用电器2
		sprintf(STa_Store,"t1.txt=\"%d\"",uea2_sta);
		HMISendStr(STa_Store);
		HMISendByte(0xff);
		//用电器3
		sprintf(STa_Store,"t2.txt=\"%d\"",uea3_sta);
		HMISendStr(STa_Store);
		HMISendByte(0xff);
		//用电器4
		sprintf(STa_Store,"t3.txt=\"%d\"",uea4_sta);
		HMISendStr(STa_Store);
		HMISendByte(0xff);
		//用电器5
		sprintf(STa_Store,"t4.txt=\"%d\"",uea5_sta);
		HMISendStr(STa_Store);
		HMISendByte(0xff);
		//用电器6
		sprintf(STa_Store,"t5.txt=\"%d\"",uea6_sta);
		HMISendStr(STa_Store);
		HMISendByte(0xff);
		//用电器7
		sprintf(STa_Store,"t6.txt=\"%d\"",uea7_sta);
		HMISendStr(STa_Store);
		HMISendByte(0xff);
	}
    else if(sscanf((char*)USART3_RX_BUF,"S:U:%f,I:%f,P:%f,Q:%f,ElecNum:%d\r\n",&U,&I,&P,&Q,&ElecNum)==5)
	{
		printf("学习：U=%0.1fV,I=%0.2fA,P=%0.1fW,Q=%0.1fVar,ElecNum=%d\n",U,I,P,Q,ElecNum);
		
		//电压
		sprintf(U_Store,"t16.txt=\"%0.1fV\"",U);
		HMISendStr(U_Store);
		HMISendByte(0xff);
		//电流
		sprintf(I_Store,"t17.txt=\"%0.3fA\"",I);
		HMISendStr(I_Store);
		HMISendByte(0xff);
		//有功功率
		sprintf(P_Store,"t18.txt=\"%0.3fW\"",P);
		HMISendStr(P_Store);
		HMISendByte(0xff);
		//无功功率
		sprintf(Q_Store,"t19.txt=\"%0.2fVar\"",Q);
		HMISendStr(Q_Store);
		HMISendByte(0xff);
		//标号
		sprintf(ElecNum_Store,"t15.txt=\"%d\"",ElecNum);
		HMISendStr(ElecNum_Store);
		HMISendByte(0xff);
	}
		
	USART3_RX_STA=0;
}


//void StudyMode(void)
//{
//	char U_Buffer[20],I_Buffer[20],P_Buffer[20],Q_Buffer[20];
//	//调节编号
//	if(IsKey3Down())  //待修改
//	{
//		ElecNum += 1;
//	}
//	if(IsKey4Down())
//	{
//		ElecNum -= 1;
//	}
//	sprintf(ElecNum_Store,"t15.txt=\"%d\"",ElecNum);
//	HMISendStr(ElecNum_Store);
//	HMISendByte(0xff);
//	//确认按键按下，开始学习
//	if(IsKey1Down()) //待修改
//	{
//		//电压
//		sprintf(U_Store,"t17.txt=\"%0.1fV\"",U);
//		HMISendStr(U_Store);
//		HMISendByte(0xff);
//		//电流
//		sprintf(I_Store,"t16.txt=\"%0.3fA\"",I);
//		HMISendStr(I_Store);
//		HMISendByte(0xff);
//		//有功功率
//		sprintf(P_Store,"t18.txt=\"%0.3fW\"",P);
//		HMISendStr(P_Store);
//		HMISendByte(0xff);
//		sprintf(P_Buffer,"%0.3f",P);
//		At24c02Write(0,(unsigned char*)P_Buffer,sizeof(P_Buffer));
//		//无功功率
//		sprintf(Q_Store,"t19.txt=\"%0.2fVar\"",Q);
//		HMISendStr(Q_Store);
//		HMISendByte(0xff);
//		
//	}
//	
//}
