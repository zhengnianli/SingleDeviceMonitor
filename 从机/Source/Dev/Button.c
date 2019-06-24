/*******************************************************************************************************
**------------------------------------------------------------------------------------------------------
** 文件: Button.c 
** 版本: v1.0
**------------------------------------------------------------------------------------------------------
** 描述:
**      按键检测文件
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "config.h"
#include "Button.h"

//-----------------------------------------------------------------------------------------------
// 按键定义
static Button_t s_tBtnKey1;			
static Button_t s_tBtnKey2;		
static Button_t s_tBtnKey3;		
static Button_t s_tBtnKey4;		

//-----------------------------------------------------------------------------------------------
// 按键事件队列
static char s_byBtnBuf[256];
static Queue_t st_BtnQueue;


//-----------------------------------------------------------------------------------------------
// 获取按键按下函数	
unsigned char IsKey1Down(void) 		{if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4) == Bit_SET) return 0; return 1;}
unsigned char IsKey2Down(void) 		{if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == Bit_SET) return 0; return 1;}
unsigned char IsKey3Down(void) 		{if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == Bit_SET) return 0; return 1;}
unsigned char IsKey4Down(void) 		{if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == Bit_SET) return 0; return 1;}

/*******************************************************************************************************
** 函数: ButtonGpioInit,Button按键初始化函数
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: void
********************************************************************************************************/
static void ButtonGpioInit(void)
{
	//-----------------------------------------------------------------------------------------------
	// 配置按键 GPIO和时钟		
	GPIO_InitTypeDef GPIO_InitStructure;					// 定义一个GPIO的结构体变量	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);	// 打开GPIOB、GPIOC的时钟
	
	GPIO_InitStructure.GPIO_Pin	  = GPIO_Pin_4 | GPIO_Pin_5;// 设置按键K1=PC4、按键K2=PC5		
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;			// 设置为上拉输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// 设置GPIO工作的最大频率50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin	  = GPIO_Pin_0 | GPIO_Pin_1;// 设置按键K3=PB0、按键K4=PB1		
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;			// 设置为上拉输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// 设置GPIO工作的最大频率50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
}

/*******************************************************************************************************
** 函数: ButtonVarInit,初始化按键变量
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: void
********************************************************************************************************/
static void ButtonVarInit(void)
{
	s_tBtnKey1.IsKeyDownFunc=IsKey1Down;		// 检测按键按下函数
	s_tBtnKey1.wFilterCount=0;					// 滤波器计数器 
	s_tBtnKey1.wFilterTime =BUTTON_FILTER_TIME;	// 滤波时间 
	s_tBtnKey1.wLongCount =0;					// 长按计数器 
	s_tBtnKey1.wLongTime=BUTTON_LONG_TIME;		// 长按时间 	
	s_tBtnKey1.byState=0;						// 按键当前状态（按下还是弹起）
	s_tBtnKey1.byKeyCodeUp=KEY1_UP;				// 按键弹起的键值代码, 0表示不检测按键弹起 
	s_tBtnKey1.byKeyCodeDown=KEY1_DOWN;			// 按键按下的键值代码, 0表示不检测按键按下
	s_tBtnKey1.byKeyCodeLong=KEY1_LONG;			// 按键长按的键值代码, 0表示不检测长按 
	
	s_tBtnKey2.IsKeyDownFunc=IsKey2Down;		// 检测按键按下函数
	s_tBtnKey2.wFilterCount=0;					// 滤波器计数器 
	s_tBtnKey2.wFilterTime =BUTTON_FILTER_TIME;	// 滤波时间 
	s_tBtnKey2.wLongCount =0;					// 长按计数器 
	s_tBtnKey2.wLongTime=BUTTON_LONG_TIME;		// 长按时间 	
	s_tBtnKey2.byState=0;						// 按键当前状态（按下还是弹起）
	s_tBtnKey2.byKeyCodeUp=KEY2_UP;				// 按键弹起的键值代码, 0表示不检测按键弹起 
	s_tBtnKey2.byKeyCodeDown=KEY2_DOWN;			// 按键按下的键值代码, 0表示不检测按键按下
	s_tBtnKey2.byKeyCodeLong=KEY2_LONG;			// 按键长按的键值代码, 0表示不检测长按 

	s_tBtnKey3.IsKeyDownFunc=IsKey3Down;		// 检测按键按下函数
	s_tBtnKey3.wFilterCount=0;					// 滤波器计数器 
	s_tBtnKey3.wFilterTime =BUTTON_FILTER_TIME;	// 滤波时间 
	s_tBtnKey3.wLongCount =0;					// 长按计数器 
	s_tBtnKey3.wLongTime=BUTTON_LONG_TIME;		// 长按时间 	
	s_tBtnKey3.byState=0;						// 按键当前状态（按下还是弹起）
	s_tBtnKey3.byKeyCodeUp=KEY3_UP;				// 按键弹起的键值代码, 0表示不检测按键弹起 
	s_tBtnKey3.byKeyCodeDown=KEY3_DOWN;			// 按键按下的键值代码, 0表示不检测按键按下
	s_tBtnKey3.byKeyCodeLong=KEY3_LONG;			// 按键长按的键值代码, 0表示不检测长按 

	s_tBtnKey4.IsKeyDownFunc=IsKey4Down;		// 检测按键按下函数
	s_tBtnKey4.wFilterCount=0;					// 滤波器计数器 
	s_tBtnKey4.wFilterTime =BUTTON_FILTER_TIME;	// 滤波时间 
	s_tBtnKey4.wLongCount =0;					// 长按计数器 
	s_tBtnKey4.wLongTime=BUTTON_LONG_TIME;		// 长按时间 	
	s_tBtnKey4.byState=0;						// 按键当前状态（按下还是弹起）
	s_tBtnKey4.byKeyCodeUp=KEY4_UP;				// 按键弹起的键值代码, 0表示不检测按键弹起 
	s_tBtnKey4.byKeyCodeDown=KEY4_DOWN;			// 按键按下的键值代码, 0表示不检测按键按下
	s_tBtnKey4.byKeyCodeLong=KEY4_LONG;			// 按键长按的键值代码, 0表示不检测长按 
	
	QueueInit(s_byBtnBuf, 256, &st_BtnQueue);	// 按键事件队列初始化
}

/*******************************************************************************************************
** 函数: ButtonDetect,Button按键检测函数
**------------------------------------------------------------------------------------------------------
** 参数: ptBtn 按键结构体指针
** 返回: void
********************************************************************************************************/
static void ButtonDetect(Button_t *ptBtn)
{
	if(ptBtn->IsKeyDownFunc && ptBtn->IsKeyDownFunc()) 				// 检测按键函数是否存在，按键是否按下
	{

		if(ptBtn->wFilterCount < ptBtn->wFilterTime)				// 滤波检测，滤波计数器到达滤波时间
		{
			ptBtn->wFilterCount++;									// 计数器加一
			return;													// 退出检测函数
		}
		
		if(ptBtn->byState ==0 ) 									// 检测是否是按键按下							
		{
			ptBtn->byState = 1;
			QueueInPut(ptBtn->byKeyCodeDown,&st_BtnQueue);			// 存入按键事件队列
			return;
		}
		if( ptBtn->wLongCount++ == ptBtn->wLongTime) 				// 检测是否是按键长按，长按计数器是否到达长按时间
		{
			QueueInPut(ptBtn->byKeyCodeLong,&st_BtnQueue);			// 存入按键事件队列
			return;
		}			
	}
	else 
	{		
		if(ptBtn->wFilterCount) 									// 滤波检测，滤波计数器是否为0
		{
			ptBtn->wFilterCount--;									// 计数器减一
			return;													// 退出检测函数
		}			
		
		if(ptBtn->byState ==1 )										// 检测是否是按键弹起
		{
			ptBtn->byState =0; 										
			QueueInPut(ptBtn->byKeyCodeUp,&st_BtnQueue);			// 存入按键事件队列
		}
		
		ptBtn->wLongCount = 0;										// 按键长按计数器清零
	}
	
}

/*******************************************************************************************************
** 函数: ButtonInit,Button初始化
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: void
********************************************************************************************************/
void ButtonInit(void)
{
	ButtonGpioInit();				// Button 按键GPIO初始化
	ButtonVarInit();				// Button 按键配置初始化
}

/*******************************************************************************************************
** 函数: ButtonProj,按键处理函数
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: void
********************************************************************************************************/
void ButtonProj(void)
{
	ButtonDetect(&s_tBtnKey1);		// 检测 K1 键 
	ButtonDetect(&s_tBtnKey2);		// 检测 K2 键 
	ButtonDetect(&s_tBtnKey3);		// 检测 K3 键 
	ButtonDetect(&s_tBtnKey4);		// 检测 K4 键 
}


/*******************************************************************************************************
** 函数: ButtonGetKey,获取按键事件函数
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: 按键事件
********************************************************************************************************/
int ButtonGetKey(void)
{
	char byRet=0;
	if(!st_BtnQueue.unResidue) return FALSE;	// 按键事件队列为空，退出
	QueueOutPut(&byRet,&st_BtnQueue);			// 获取按键事件 
	return byRet;								
}



/********************************************************************************************************
**                            End Of File
********************************************************************************************************/
