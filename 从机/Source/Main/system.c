/*******************************************************************************************************
**------------------------------------------------------------------------------------------------------
** 文件	: 	system.c			
** 版本	: 	v1.0 
**------------------------------------------------------------------------------------------------------
** 描述	:	
		系统初始化程序
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "config.h"

/*******************************************************************************************************
** 函数: CpuInit,配置系统信息
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无
** 说明: system_stm32f10x.c内的SystemInit函数已经有对系统一些参数配置，并由汇编启动函数入口调用
		 本处做一些补充,改函数必须在其他函数之前调用
********************************************************************************************************/
void CpuInit( void )
{
  	//-------------------------------------------------------------------------------
	// 关闭调试端口重新映射 设置中断优先级
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);			
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  		// 关闭调试 端口重新映射
  	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);					// 选择第2组的中断优先级
}
/*******************************************************************************************************
** 函数: SysInit，上电初始化
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: 无
********************************************************************************************************/
void SysInit(void)
{
	CpuInit();						// 配置系统信息函数
	UsartInit(115200);				// 串口初始化函数，波特率115200
 	uart2_HMI_init(9600);			// 串口屏初始化
	uart3_ZigBee_init(115200);	    // Zigbee模块硬件初始化
	SysTickInit();					// 系统滴答定时器初始化函数
	Tim2Init();						// 普通定时器初始化函数
 	At24c02Init();					// AT24C02初始化
 	HMISendStart();					// 为确保串口屏进行正常通讯
}

/********************************************************************************************************
**                            End Of File
********************************************************************************************************/

