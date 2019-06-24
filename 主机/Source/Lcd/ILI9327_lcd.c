/****************************************Copyright (c)**************************************************
**                               ����׿Ⱥ������Ϣ�Ƽ����޹�˾
**------------------------------------------------------------------------------------------------------
** �ļ�	: 	ILI9327_lcd.C			
** �汾	: 	v1.0 
**------------------------------------------------------------------------------------------------------
** ����	:	
		���ļ�ΪILI9327_lcd.c��������
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "config.h"
#include "ILI9327_lcd.h"
#include "Font_Lib.h"	 		//�ֿ��ļ���

#define HIGH	240
#define WIDTH	400

/*******************************************************************************************************
** ����: LCD_GPIOConfig,  LCD GPIO��ʼ��
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ��
********************************************************************************************************/
void LCD_GPIOConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//------------------------------------------------------------------------------------------------
	// ����IO��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE , ENABLE); 	
	// ���� PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14), PD.10(D15), PD.14(D0), PD.15(D1) Ϊ�����������	 
	GPIO_InitStructure.GPIO_Pin = 	GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	// ���� PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),PE.14(D11), PE.15(D12) Ϊ�����������
	GPIO_InitStructure.GPIO_Pin = 	GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	/* PA12(Light) Ϊ������� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_11);

	/*PD4(RD)��PD5(WR)��PD7(CS)��PD11(RS) Ϊ������� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_AF_PP; 	 
	GPIO_Init(GPIOD, &GPIO_InitStructure); 

}
/*******************************************************************************************************
** ����: FSMC_LCD_Init, LCD FSMC��ʼ��
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ��
********************************************************************************************************/
void FSMC_LCD_Init(void)
{
	FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef  FSMC_TimingInitStructure;
	//------------------------------------------------------------------------------------------------
	// ����FSMC��FSMCʱ��
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
** ����: LCD_WriteRAM, д���ݵ�GRAM
**------------------------------------------------------------------------------------------------------
** ����: val д����
** ����: ��
********************************************************************************************************/
void LCD_WriteRAM(unsigned int val)
{    	
	LCD_Data_Addr = val;
}
/*******************************************************************************************************
** ����: LCD_WriteCmd,  д�����Ĵ���
**------------------------------------------------------------------------------------------------------
** ����: index �Ĵ���
** ����: ��
********************************************************************************************************/
void LCD_WriteCmd(unsigned int index)
{
	LCD_Reg_Addr = index;
}

/*******************************************************************************************************
** ����: LCD_WriteReg, ��ָ���Ĵ���дֵ
**------------------------------------------------------------------------------------------------------
** ����: �Ĵ�����ֵ
** ����: ��
********************************************************************************************************/
void LCD_WriteReg(unsigned int LCD_Reg, unsigned int LCD_RegValue)
{
	LCD_Reg_Addr  = LCD_Reg;
	LCD_Data_Addr = LCD_RegValue;
}
/*******************************************************************************************************
** ����: LCD_WriteRAM_Prepare,  д����׼��
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ��
********************************************************************************************************/
void LCD_WriteRAM_Prepare(void)   
{
	LCD_WriteCmd(0x002c);
}

/*******************************************************************************************************
** ����: LCD_SetCursor,  �趨�����
**------------------------------------------------------------------------------------------------------
** ����: x��y
** ����: ��
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
** ����: LCD_Set_Window, �趨����
**------------------------------------------------------------------------------------------------------
** ����: wX1,wY1,x2,y2
** ����: ��
********************************************************************************************************/
void LCD_Set_Window(u16 wX0,u16 wY0,u16 wX1,u16 wY1)
{

	LCD_WriteCmd(0x002A);
	LCD_WriteRAM(wX0>>8);//X��ʼ��ַ
	LCD_WriteRAM(wX0);
	LCD_WriteRAM(wX1>>8);//X������ַ
	LCD_WriteRAM(wX1);	//X������ַ

	LCD_WriteCmd(0x002B);
	LCD_WriteRAM(wY0>>8);//Y��ʼ��ַ
	LCD_WriteRAM(wY0);
	LCD_WriteRAM(wY1>>8);//Y������ַ
	LCD_WriteRAM(wY1);
}

/*******************************************************************************************************
** ����: LCD_DrawPoint,  ����
**------------------------------------------------------------------------------------------------------
** ����: x��y��color
** ����: ��
********************************************************************************************************/
void LCD_DrawPoint(unsigned short Xpos,unsigned short Ypos,unsigned short RGB_Code)
{
	LCD_SetCursor(Xpos,Ypos);
	LCD_WriteRAM_Prepare();					//дRAM׼��
	LCD_WriteRAM(RGB_Code);					//������ɫ
}

/*******************************************************************************************************
** ����: WriteOneASCII, ��ʾһ��ָ����С���ַ�
**------------------------------------------------------------------------------------------------------
** ����: �ַ���x��y��color��back color
** ����: ��
********************************************************************************************************/
u16 WriteOneASCII(u8 *pucMsk,u16 wX0,u16 wY0,u16 color,u16 back_color)
{
    u16 i,j;   
	LCD_Set_Window(wX0,wY0,wX0+7,wY0+15);       //����д���ݵ�ַָ��        
	LCD_WriteRAM_Prepare();        			//��ʼд��GRAM	    		
    for(i=0; i<16; i++) 
	{                                 		//16��                     
        for(j=0; j<8; j++) 
		{                     				//8��                     
            if((*(pucMsk+i) << j) & 0x80) 
                LCD_WriteRAM(color);
            else
                LCD_WriteRAM(back_color);
        }
    }
    return (8);                             // ����8λ�п�             
}
/*******************************************************************************************************
** ����: WriteOneASC, ��ʾһ��ָ����С���ַ�,�Ե���Ϊ����
**------------------------------------------------------------------------------------------------------
** ����: �ַ���x��y��color��back color
** ����: ��
********************************************************************************************************/
u16 WriteOneASC(u8 *pucMsk,u16 wX0,u16 wY0,u16 color)
{
    u16 i,j;   
	LCD_Set_Window(wX0,wY0,wX0+7,wY0+15);       //����д���ݵ�ַָ��        
	LCD_WriteRAM_Prepare();        			//��ʼд��GRAM	    		
    for(i=0; i<16; i++) 
	{                                 		//16��                     
        for(j=0; j<8; j++) 
		{                     				//8��                     
            if((*(pucMsk+i) << j) & 0x80) 
                LCD_WriteRAM(color);
            else
                LCD_WriteRAM(((unsigned int)gImage_Back[(wX0+j)*2+(wY0+i)*800+1]<<8)|gImage_Back[(wX0+j)*2+(wY0+i)*800]);
        }
    }
    return (8);                             // ����8λ�п�             
}

/*******************************************************************************************************
** ����: LCD_ShowString, ��ָ��λ�ÿ�ʼ��ʾһ���ַ���
**------------------------------------------------------------------------------------------------------
** ����: x��y���ַ�����color��back color
** ����: ��
********************************************************************************************************/
void LCD_ShowString(u16 wX, u16 wY,char *pcStr, u16 color,u16 back_color)
{
	while(*pcStr)
    {
        if((u8)*pcStr > 0x80) break;         // �ж�Ϊ����            
		if((wX + 8) > LCD_W) wX = 0;         // ���ʣ��ռ��Ƿ��㹻        
		if((wY +16) > LCD_H) wY = 0;         // ���ʣ��ռ��Ƿ��㹻        
		wX += WriteOneASCII((u8 *)&ASCII_1608[(*pcStr++ - 0x20)][0], wX, wY, color,back_color);									
	}												  	  
}


/*******************************************************************************************************
** ����: LCD_Image,  ͼƬ��ʾ400X240
**------------------------------------------------------------------------------------------------------
** ����: color
** ����: ��
********************************************************************************************************/
void LCD_Image400x240(const unsigned  char *Image)                  
{	
	u32 i=0; 
	LCD_WriteReg(0x0036,0x0028); 			//��ʾ����������� �������� ������
	LCD_Set_Window(0,0,WIDTH-1,HIGH-1);
	LCD_WriteRAM_Prepare();    		 		//��ʼд��GRAM	 	  
	for(i=0;i<240*400;i++)
	{
		LCD_WriteRAM(((unsigned int)Image[i*2+1]<<8)|Image[i*2]);
	}
}
/*******************************************************************************************************
** ����: LCD_Image,  ͼƬ��ʾ
**------------------------------------------------------------------------------------------------------
** ����: ͼƬ������ͼ
** ����: ��
********************************************************************************************************/
void LCD_Image(u16 wX,u16 wY,const unsigned char *byImage)                  
{	
	u32 i=0; 
	if(wX>400 || wY>240)return ;
	LCD_WriteReg(0x0036,0x0028); 			//��ʾ����������� �������� ������
	LCD_Set_Window((WIDTH-wX)/2,(WIDTH+wX)/2,(HIGH-wY)/2,(HIGH+wY)/2);
	LCD_WriteRAM_Prepare();    		 		//��ʼд��GRAM	 	  
	for(i=0;i<wX*wY;i++)
	{
		LCD_WriteRAM(((unsigned int)byImage[i*2+1]<<8)|byImage[i*2]);
	}
}
/*******************************************************************************************************
** ����: LCD_DrawRectangle,  ������
**------------------------------------------------------------------------------------------------------
** ����: x��1��2,y��1��2,color
** ����: ��
********************************************************************************************************/
void LCD_DrawRectangle(u16 wX0,u16 wX1,u16 wY0,u16 wY1,u16 Color)                  
{	
	u32 i=0; 
	LCD_Set_Window(wX0,wY0,wX1,wY1);
	LCD_WriteRAM_Prepare();    		 		//��ʼд��GRAM	 	  
	for(i=0;i<(wX1-wX0+1)*(wY1-wY0+1);i++)
	{
		LCD_WriteRAM(Color);
	}
}
/*******************************************************************************************************
** ����: LCD_DrawRectangle,  ����
**------------------------------------------------------------------------------------------------------
** ����: ���4�����㣺wX0,wX1,wY0,y1,���
** ����: ��
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
** ����: LCD_DrawTooltip,��ʾ��
**------------------------------------------------------------------------------------------------------
** ����: �Ի��򳤡�����ɫ
** ����: ��
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
** ����: LCD_DrawText,��ʾ�ı�
**------------------------------------------------------------------------------------------------------
** ����: �ı�λ��x,y,�ı����� l, �ı����� pcStr,������ɫ Color
** ����: ��
********************************************************************************************************/
void LCD_DrawText(unsigned int x, unsigned int y,unsigned int l,char *pcStr,u16 Color)
{
	while(*pcStr && l--)
    {
		
        if((u8)*pcStr > 0x80) break;         // �ж�Ϊ����            
		if((x + 8) > LCD_W) x = 0;         // ���ʣ��ռ��Ƿ��㹻        
		if((y +16) > LCD_H) y = 0;         // ���ʣ��ռ��Ƿ��㹻        
		x += WriteOneASC((u8 *)&ASCII_1608[(*pcStr++ - 0x20)][0], x, y, Color);									
	}
	while(!(*pcStr) && l--)
	{
		if((x + 8) > LCD_W) x = 0;         // ���ʣ��ռ��Ƿ��㹻        
		if((y +16) > LCD_H) y = 0;         // ���ʣ��ռ��Ƿ��㹻        
		x += WriteOneASC((u8 *)&ASCII_1608[(' ' - 0x20)][0], x, y, Color);									

	}
}
/*******************************************************************************************************
** ����: LCD_Clear,  ����
**------------------------------------------------------------------------------------------------------
** ����: color
** ����: ��
********************************************************************************************************/
void LCD_Clear(u16 Color)                  
{	
	u32 i=0; 
	LCD_WriteReg(0x0036,0x0028); 			//��ʾ����������� �������� ������
	LCD_Set_Window(0,0,WIDTH-1,HIGH-1);
	LCD_WriteRAM_Prepare();    		 		//��ʼд��GRAM	 	  
	for(i=0;i<240*400;i++)
	{
		LCD_WriteRAM(Color);
	}
}
/*******************************************************************************************************
** ����: LCD_Init, ��ʼ��ILI9327
**------------------------------------------------------------------------------------------------------
** ����: ��
** ����: ��
********************************************************************************************************/
void LCD_Init(void)
{
	//------------------------------------------------------------------------------------------------
	// CDҺ��FSMC��ʼ��
	FSMC_LCD_Init();
	
	//------------------------------------------------------------------------------------------------
	// LCDҺ��GPIO��ʼ��
	LCD_GPIOConfig();	

	//------------------------------------------------------------------------------------------------
	// LCDҺ���Ĵ�����ʼ��
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
