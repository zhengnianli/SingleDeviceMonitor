/********************************************************************************************************
**------------------------------------------------------------------------------------------------------
** 文件: ds18b20.c
** 版本: v1.0
**------------------------------------------------------------------------------------------------------
** 描述:
**     此部分为18B20的驱动程序
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "config.h"


//-------------------------------------------------------------------------------
// 定时标志位
static int s_iIsTimer=0;
extern vu16 USART3_RX_STA;
/*******************************************************************************************************
** 函数: Tim2VarInit, Tim2配置初始化
**------------------------------------------------------------------------------------------------------
** 参数: 定时器溢出计数
** 返回: void
********************************************************************************************************/
static void Tim2VarInit(int Var)
{
	//-------------------------------------------------------------------------------
	// 配置TIM和TIM时钟
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	TIM_DeInit(TIM2);										//重新将Timer设置为缺省值
	TIM_InternalClockConfig(TIM2);							//采用内部时钟给TIM2提供时钟源
	TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;			//预分频系数为36000-1，这样计数器时钟为72MHz/36000 = 2kHz
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	//设置时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//设置计数器模式为向上计数模式
	TIM_TimeBaseStructure.TIM_Period = (Var<2)?1:Var-1;		//设置计数溢出大小，每计Var个数就产生一个更新事件
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);			//将配置应用到TIM2中
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);					//清除溢出中断标志
	TIM_ARRPreloadConfig(TIM2, DISABLE);					//禁止ARR预装载缓冲器
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);				//开启TIM2的中断
	TIM_Cmd(TIM2,ENABLE);
}
/*******************************************************************************************************
** 函数: Tim2NvicInit, Tim2中断初始化
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: void
********************************************************************************************************/
static void Tim2NvicInit(void)
{
	//------------------------------------------------------------------------------------------------
	// 配置TIM中断	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;      	// 选择TIM2的中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	// 抢占式中断优先级设置为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		// 响应式中断优先级设置为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			// 中断使能
	NVIC_Init(&NVIC_InitStructure);							// 配置中断

}
/*******************************************************************************************************
** 函数: Tim2Init, Tim定时器延时函数
**------------------------------------------------------------------------------------------------------
** 参数: us 延时时间
** 返回: void
********************************************************************************************************/
void Tim2Init(void)
{
	Tim2VarInit(1000);
	Tim2NvicInit( );	
}

/*******************************************************************************************************
** 函数: Timer1DelayUs, 定时器us延时函数
**------------------------------------------------------------------------------------------------------
** 参数: us 延时时间
** 返回: void
********************************************************************************************************/
void TimDelayUs(unsigned int us)
{
	s_iIsTimer=1;				// 定时标志位置位
	CPU_INT_ENABLE() 			// 开总中断、确保中断打开
	Tim2VarInit(us);			// 设置定时时间
	while(s_iIsTimer);			// 等待定时标志位复位
}

/*******************************************************************************************************
** 函数: TIM2_IRQHandler, 定时器TIM2中断函数
**------------------------------------------------------------------------------------------------------
** 参数: us 延时时间
** 返回: void
********************************************************************************************************/
void TIM2_IRQHandler(void)
{
	//------------------------------------------------------------------------------------------------
	// 检测是否发生溢出更新事件
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		//------------------------------------------------------------------------------------------------
		// TIM2的中断处理
		
		s_iIsTimer=0;										// 定时标志位复位
		TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);			// 关闭中断
		//------------------------------------------------------------------------------------------------
		// 清除TIM2的中断待处理位
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);

	}
}



//定时器7中断服务程序		    
void TIM3_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)//是更新中断
	{	 	
			
		USART3_RX_STA|=1<<15;	//标记接收完成
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //清除TIM3更新中断标志 
	
	
		TIM_Cmd(TIM3, DISABLE);  //关闭TIM3 
	}	    
}
 
//通用定时器7中断初始化
//这里时钟选择为APB1的2倍，而APB1为42M
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz 
//通用定时器中断初始化
//这里始终选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数		 
void TIM3_Int_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//TIM3时钟使能    
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断
	
	TIM_Cmd(TIM3,ENABLE);//开启定时器7
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
}
/********************************************************************************************************
**                            End Of File
********************************************************************************************************/
