/****************************************Copyright (c)**************************************************
**                               福州卓群杰能信息科技有限公司
**------------------------------------------------------------------------------------------------------
** 文件	: 	ILI9327_lcd.C			
** 版本	: 	v1.0 
**------------------------------------------------------------------------------------------------------
** 描述	:	
		本文件为ILI9327_lcd.c驱动程序
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "config.h"
#include "ILI9327_lcd.h"
#include "Font_Lib.h"	 		//字库文件库

#define HIGH	240
#define WIDTH	400

/*******************************************************************************************************
** 函数: LCD_GPIOConfig,  LCD GPIO初始化
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无
********************************************************************************************************/
void LCD_GPIOConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//------------------------------------------------------------------------------------------------
	// 配置IO和时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE , ENABLE); 	
	// 设置 PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14), PD.10(D15), PD.14(D0), PD.15(D1) 为复用推挽输出	 
	GPIO_InitStructure.GPIO_Pin = 	GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	// 设置 PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),PE.14(D11), PE.15(D12) 为复用推挽输出
	GPIO_InitStructure.GPIO_Pin = 	GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	/* PA12(Light) 为推挽输出 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_11);

	/*PD4(RD)，PD5(WR)，PD7(CS)，PD11(RS) 为推挽输出 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_PP; 	 
	GPIO_Init(GPIOD, &GPIO_InitStructure); 

}
/*******************************************************************************************************
** 函数: FSMC_LCD_Init, LCD FSMC初始化
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无
********************************************************************************************************/
void FSMC_LCD_Init(void)
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  FSMC_TimingInitStructure;
	//------------------------------------------------------------------------------------------------
	// 配置FSMC和FSMC时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);  
	FSMC_TimingInitStructure.FSMC_AddressSetupTime = 0x02;
	FSMC_TimingInitStructure.FSMC_AddressHoldTime = 0x00;
	FSMC_TimingInitStructure.FSMC_DataSetupTime = 0x05;
	FSMC_TimingInitStructure.FSMC_BusTurnAroundDuration = 0x00;
	FSMC_TimingInitStructure.FSMC_CLKDivision = 0x00;
	FSMC_TimingInitStructure.FSMC_DataLatency = 0x00;
	FSMC_TimingInitStructure.FSMC_AccessMode = FSMC_AccessMode_B;
	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_NOR;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &FSMC_TimingInitStructure;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &FSMC_TimingInitStructure;	  
	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure); 
	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);  
}

/*******************************************************************************************************
** 函数: LCD_WriteRAM, 写数据到GRAM
**------------------------------------------------------------------------------------------------------
** 参数: val 写数据
** 返回: 无
********************************************************************************************************/
void LCD_WriteRAM(unsigned int val)
{    	
	LCD_Data_Addr = val;
}
/*******************************************************************************************************
** 函数: LCD_WriteCmd,  写索引寄存器
**------------------------------------------------------------------------------------------------------
** 参数: index 寄存器
** 返回: 无
********************************************************************************************************/
void LCD_WriteCmd(unsigned int index)
{
	LCD_Reg_Addr = index;
}

/*******************************************************************************************************
** 函数: LCD_WriteReg, 给指定寄存器写值
**------------------------------------------------------------------------------------------------------
** 参数: 寄存器、值
** 返回: 无
********************************************************************************************************/
void LCD_WriteReg(unsigned int LCD_Reg, unsigned int LCD_RegValue)
{
	LCD_Reg_Addr  = LCD_Reg;
	LCD_Data_Addr = LCD_RegValue;
}
/*******************************************************************************************************
** 函数: LCD_WriteRAM_Prepare,  写数据准备
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无
********************************************************************************************************/
void LCD_WriteRAM_Prepare(void)   
{
	LCD_WriteCmd(0x002c);
}

/*******************************************************************************************************
** 函数: LCD_SetCursor,  设定坐标点
**------------------------------------------------------------------------------------------------------
** 参数: x、y
** 返回: 无
********************************************************************************************************/
void LCD_SetCursor(u16 Xpos, u16 Ypos)
{
	LCD_WriteCmd(0x002A);
	LCD_WriteRAM(Xpos>>8);
	LCD_WriteRAM(Xpos);
	LCD_WriteCmd(0x002B);
	LCD_WriteRAM(Ypos>>8);
	LCD_WriteRAM(Ypos);
}
/*******************************************************************************************************
** 函数: LCD_Set_Window, 设定窗口
**------------------------------------------------------------------------------------------------------
** 参数: wX1,wY1,x2,y2
** 返回: 无
********************************************************************************************************/
void LCD_Set_Window(u16 wX0,u16 wY0,u16 wX1,u16 wY1)
{

	LCD_WriteCmd(0x002A);
	LCD_WriteRAM(wX0>>8);//X开始地址
	LCD_WriteRAM(wX0);
	LCD_WriteRAM(wX1>>8);//X结束地址
	LCD_WriteRAM(wX1);	//X结束地址

	LCD_WriteCmd(0x002B);
	LCD_WriteRAM(wY0>>8);//Y开始地址
	LCD_WriteRAM(wY0);
	LCD_WriteRAM(wY1>>8);//Y结束地址
	LCD_WriteRAM(wY1);
}

/*******************************************************************************************************
** 函数: LCD_DrawPoint,  画点
**------------------------------------------------------------------------------------------------------
** 参数: x、y、color
** 返回: 无
********************************************************************************************************/
void LCD_DrawPoint(unsigned short Xpos,unsigned short Ypos,unsigned short RGB_Code)
{
	LCD_SetCursor(Xpos,Ypos);
	LCD_WriteRAM_Prepare();					//写RAM准备
	LCD_WriteRAM(RGB_Code);					//设置颜色
}

/*******************************************************************************************************
** 函数: WriteOneASCII, 显示一个指定大小的字符
**------------------------------------------------------------------------------------------------------
** 参数: 字符、x、y、color、back color
** 返回: 无
********************************************************************************************************/
u16 WriteOneASCII(u8 *pucMsk,u16 wX0,u16 wY0,u16 color,u16 back_color)
{
    u16 i,j;   
	LCD_Set_Window(wX0,wY0,wX0+7,wY0+15);       //设置写数据地址指针        
	LCD_WriteRAM_Prepare();        			//开始写入GRAM	    		
    for(i=0; i<16; i++) 
	{                                 		//16行                     
        for(j=0; j<8; j++) 
		{                     				//8列                     
            if((*(pucMsk+i) << j) & 0x80) 
                LCD_WriteRAM(color);
            else
                LCD_WriteRAM(back_color);
        }
    }
    return (8);                             // 返回8位列宽             
}
/*******************************************************************************************************
** 函数: WriteOneASC, 显示一个指定大小的字符,以底纹为背景
**------------------------------------------------------------------------------------------------------
** 参数: 字符、x、y、color、back color
** 返回: 无
********************************************************************************************************/
u16 WriteOneASC(u8 *pucMsk,u16 wX0,u16 wY0,u16 color)
{
    u16 i,j;   
	LCD_Set_Window(wX0,wY0,wX0+7,wY0+15);       //设置写数据地址指针        
	LCD_WriteRAM_Prepare();        			//开始写入GRAM	    		
    for(i=0; i<16; i++) 
	{                                 		//16行                     
        for(j=0; j<8; j++) 
		{                     				//8列                     
            if((*(pucMsk+i) << j) & 0x80) 
                LCD_WriteRAM(color);
            else
                LCD_WriteRAM(((unsigned int)gImage_Back[(wX0+j)*2+(wY0+i)*800+1]<<8)|gImage_Back[(wX0+j)*2+(wY0+i)*800]);
        }
    }
    return (8);                             // 返回8位列宽             
}

/*******************************************************************************************************
** 函数: LCD_ShowString, 在指定位置开始显示一个字符串
**------------------------------------------------------------------------------------------------------
** 参数: x、y、字符串、color、back color
** 返回: 无
********************************************************************************************************/
void LCD_ShowString(u16 wX, u16 wY,char *pcStr, u16 color,u16 back_color)
{
	while(*pcStr)
    {
        if((u8)*pcStr > 0x80) break;         // 判断为汉字            
		if((wX + 8) > LCD_W) wX = 0;         // 检查剩余空间是否足够        
		if((wY +16) > LCD_H) wY = 0;         // 检查剩余空间是否足够        
		wX += WriteOneASCII((u8 *)&ASCII_1608[(*pcStr++ - 0x20)][0], wX, wY, color,back_color);									
	}												  	  
}


/*******************************************************************************************************
** 函数: LCD_Image,  图片显示400X240
**------------------------------------------------------------------------------------------------------
** 参数: color
** 返回: 无
********************************************************************************************************/
void LCD_Image400x240(const unsigned  char *Image)                  
{	
	u32 i=0; 
	LCD_WriteReg(0x0036,0x0028); 			//显示方向从左往右 从上向下 即横屏
	LCD_Set_Window(0,0,WIDTH-1,HIGH-1);
	LCD_WriteRAM_Prepare();    		 		//开始写入GRAM	 	  
	for(i=0;i<240*400;i++)
	{
		LCD_WriteRAM(((unsigned int)Image[i*2+1]<<8)|Image[i*2]);
	}
}
/*******************************************************************************************************
** 函数: LCD_Image,  图片显示
**------------------------------------------------------------------------------------------------------
** 参数: 图片长、宽、图
** 返回: 无
********************************************************************************************************/
void LCD_Image(u16 wX,u16 wY,const unsigned char *byImage)                  
{	
	u32 i=0; 
	if(wX>400 || wY>240)return ;
	LCD_WriteReg(0x0036,0x0028); 			//显示方向从左往右 从上向下 即横屏
	LCD_Set_Window((WIDTH-wX)/2,(WIDTH+wX)/2,(HIGH-wY)/2,(HIGH+wY)/2);
	LCD_WriteRAM_Prepare();    		 		//开始写入GRAM	 	  
	for(i=0;i<wX*wY;i++)
	{
		LCD_WriteRAM(((unsigned int)byImage[i*2+1]<<8)|byImage[i*2]);
	}
}
/*******************************************************************************************************
** 函数: LCD_DrawRectangle,  画矩形
**------------------------------------------------------------------------------------------------------
** 参数: x轴1、2,y轴1、2,color
** 返回: 无
********************************************************************************************************/
void LCD_DrawRectangle(u16 wX0,u16 wX1,u16 wY0,u16 wY1,u16 Color)                  
{	
	u32 i=0; 
	LCD_Set_Window(wX0,wY0,wX1,wY1);
	LCD_WriteRAM_Prepare();    		 		//开始写入GRAM	 	  
	for(i=0;i<(wX1-wX0+1)*(wY1-wY0+1);i++)
	{
		LCD_WriteRAM(Color);
	}
}
/*******************************************************************************************************
** 函数: LCD_DrawRectangle,  画表
**------------------------------------------------------------------------------------------------------
** 参数: 表格4个顶点：wX0,wX1,wY0,y1,表格
** 返回: 无
********************************************************************************************************/
void LCD_DrawChart(u16 wX0,u16 wX1,u16 wY0,u16 wY1,u16 wX,u16 wY,u16 Color)
{
	unsigned char i=0;	
	for(i=0;i<=wX;i++)
	{
		LCD_DrawRectangle( wX0+i*(wX1-wX0)/wX,wX0+i*(wX1-wX0)/wX+1,wY0,wY1,Color );
	}
	for(i=0;i<=wY;i++)
	{
		LCD_DrawRectangle( wX0,wX1,wY0+i*(wY1-wY0)/wY,wY0+i*(wY1-wY0)/wY+1,Color );
	}
}

/*******************************************************************************************************
** 函数: LCD_DrawTooltip,提示框
**------------------------------------------------------------------------------------------------------
** 参数: 对话框长、宽、底色
** 返回: 无
********************************************************************************************************/
void LCD_DrawTooltip(char *pcStr,u16 Color,u16 back_color)
{
	unsigned int x,y;
	x=strlen(pcStr)*8+44;	y=44;
	LCD_DrawRectangle((WIDTH-x)/2,(WIDTH+x)/2,(HIGH-y)/2,(HIGH+y)/2,BLACK);
	
	x=strlen(pcStr)*8+40;	y=40;
	LCD_DrawRectangle((WIDTH-x)/2,(WIDTH+x)/2,(HIGH-y)/2,(HIGH+y)/2,back_color);
	
	LCD_ShowString((WIDTH-strlen(pcStr)*8)/2,(HIGH-16)/2,pcStr,WHITE,back_color);
}
  	
/*******************************************************************************************************
** 函数: LCD_DrawText,显示文本
**------------------------------------------------------------------------------------------------------
** 参数: 文本位置x,y,文本长度 l, 文本内容 pcStr,文字颜色 Color
** 返回: 无
********************************************************************************************************/
void LCD_DrawText(unsigned int x, unsigned int y,unsigned int l,char *pcStr,u16 Color)
{
	while(*pcStr && l--)
    {
		
        if((u8)*pcStr > 0x80) break;         // 判断为汉字            
		if((x + 8) > LCD_W) x = 0;         // 检查剩余空间是否足够        
		if((y +16) > LCD_H) y = 0;         // 检查剩余空间是否足够        
		x += WriteOneASC((u8 *)&ASCII_1608[(*pcStr++ - 0x20)][0], x, y, Color);									
	}
	while(!(*pcStr) && l--)
	{
		if((x + 8) > LCD_W) x = 0;         // 检查剩余空间是否足够        
		if((y +16) > LCD_H) y = 0;         // 检查剩余空间是否足够        
		x += WriteOneASC((u8 *)&ASCII_1608[(' ' - 0x20)][0], x, y, Color);									

	}
}
/*******************************************************************************************************
** 函数: LCD_Clear,  清屏
**------------------------------------------------------------------------------------------------------
** 参数: color
** 返回: 无
********************************************************************************************************/
void LCD_Clear(u16 Color)                  
{	
	u32 i=0; 
	LCD_WriteReg(0x0036,0x0028); 			//显示方向从左往右 从上向下 即横屏
	LCD_Set_Window(0,0,WIDTH-1,HIGH-1);
	LCD_WriteRAM_Prepare();    		 		//开始写入GRAM	 	  
	for(i=0;i<240*400;i++)
	{
		LCD_WriteRAM(Color);
	}
}
/*******************************************************************************************************
** 函数: LCD_Init, 初始化ILI9327
**------------------------------------------------------------------------------------------------------
** 参数: 无
** 返回: 无
********************************************************************************************************/
void LCD_Init(void)
{
	//------------------------------------------------------------------------------------------------
	// CD液晶FSMC初始化
	FSMC_LCD_Init();
	
	//------------------------------------------------------------------------------------------------
	// LCD液晶GPIO初始化
	LCD_GPIOConfig();	

	//------------------------------------------------------------------------------------------------
	// LCD液晶寄存器初始化
	LCD_WriteCmd(0x00e9);
    LCD_WriteRAM(0x0020);
    LCD_WriteCmd(0x0011);
	SysTickDelayMS(500);
	
    LCD_WriteCmd(0x00d1);
    LCD_WriteRAM(0x0000);
    LCD_WriteRAM(0x0071);
    LCD_WriteRAM(0x0019);

    LCD_WriteCmd(0x00d0);
    LCD_WriteRAM(0x0007);
    LCD_WriteRAM(0x0001);
    LCD_WriteRAM(0x0008);
    LCD_WriteCmd(0x0036);
    LCD_WriteRAM(0x0048);	 
    LCD_WriteCmd(0x003a);
    LCD_WriteRAM(0x0005);
    LCD_WriteCmd(0x00C1);
    LCD_WriteRAM(0x0010);
    LCD_WriteRAM(0x0010);
    LCD_WriteRAM(0x0002);
    LCD_WriteRAM(0x0002);
    LCD_WriteCmd(0x00C0);
    LCD_WriteRAM(0x0000);
    LCD_WriteRAM(0x0035);
    LCD_WriteRAM(0x0000);
    LCD_WriteRAM(0x0000);
    LCD_WriteRAM(0x0001);
    LCD_WriteRAM(0x0002);
    LCD_WriteCmd(0x00C5);
    LCD_WriteRAM(0x0004);
    LCD_WriteCmd(0x00D2);
    LCD_WriteRAM(0x0001);
    LCD_WriteRAM(0x0044);
    LCD_WriteCmd(0x00C8);
    LCD_WriteRAM(0x0004);
    LCD_WriteRAM(0x0067);
    LCD_WriteRAM(0x0035);
    LCD_WriteRAM(0x0004);
    LCD_WriteRAM(0x0008);
    LCD_WriteRAM(0x0006);
    LCD_WriteRAM(0x0024);
    LCD_WriteRAM(0x0001);
    LCD_WriteRAM(0x0037);
    LCD_WriteRAM(0x0040);
    LCD_WriteRAM(0x0003);
    LCD_WriteRAM(0x0010);
    LCD_WriteRAM(0x0008);
    LCD_WriteRAM(0x0080);
    LCD_WriteRAM(0x0000);
    LCD_WriteCmd(0x002A);
    LCD_WriteRAM(0x0000);                
    LCD_WriteRAM(0x0000); 
    LCD_WriteRAM(0x0000);
    LCD_WriteRAM(0x00ef);
    LCD_WriteCmd(0x002b); 
    LCD_WriteRAM(0x0000);
    LCD_WriteRAM(0x0000);
    LCD_WriteRAM(0x0001);
    LCD_WriteRAM(0x008f); 
    LCD_WriteCmd(0x0029);  
	LCD_WriteCmd(0x002C);
}

/********************************************************************************************************
**                            End Of File
********************************************************************************************************/
