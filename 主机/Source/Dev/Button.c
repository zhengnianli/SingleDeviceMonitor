/*******************************************************************************************************
**------------------------------------------------------------------------------------------------------
** �ļ�: Button.c 
** �汾: v1.0
**------------------------------------------------------------------------------------------------------
** ����:
**      ��������ļ�
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "config.h"
#include "Button.h"

//-----------------------------------------------------------------------------------------------
// ��������
static Button_t s_tBtnKey1;			
static Button_t s_tBtnKey2;		
static Button_t s_tBtnKey3;		
static Button_t s_tBtnKey4;		

//-----------------------------------------------------------------------------------------------
// �����¼�����
static char s_byBtnBuf[256];
static Queue_t st_BtnQueue;


//-----------------------------------------------------------------------------------------------
// ��ȡ�������º���	
unsigned char IsKey1Down(void) 		{if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4) == Bit_SET) return 0; return 1;}
unsigned char IsKey2Down(void) 		{if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5) == Bit_SET) return 0; return 1;}
unsigned char IsKey3Down(void) 		{if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == Bit_SET) return 0; return 1;}
unsigned char IsKey4Down(void) 		{if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == Bit_SET) return 0; return 1;}

/*******************************************************************************************************
** ����: ButtonGpioInit,Button������ʼ������
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: void
********************************************************************************************************/
static void ButtonGpioInit(void)
{
	//-----------------------------------------------------------------------------------------------
	// ���ð��� GPIO��ʱ��		
	GPIO_InitTypeDef GPIO_InitStructure;					// ����һ��GPIO�Ľṹ�����	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);	// ��GPIOB��GPIOC��ʱ��
	
	GPIO_InitStructure.GPIO_Pin	  = GPIO_Pin_4 | GPIO_Pin_5;// ���ð���K1=PC4������K2=PC5		
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;			// ����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// ����GPIO���������Ƶ��50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin	  = GPIO_Pin_0 | GPIO_Pin_1;// ���ð���K3=PB0������K4=PB1		
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPD;			// ����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// ����GPIO���������Ƶ��50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
}

/*******************************************************************************************************
** ����: ButtonVarInit,��ʼ����������
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: void
********************************************************************************************************/
static void ButtonVarInit(void)
{
	s_tBtnKey1.IsKeyDownFunc=IsKey1Down;		// ��ⰴ�����º���
	s_tBtnKey1.wFilterCount=0;					// �˲��������� 
	s_tBtnKey1.wFilterTime =BUTTON_FILTER_TIME;	// �˲�ʱ�� 
	s_tBtnKey1.wLongCount =0;					// ���������� 
	s_tBtnKey1.wLongTime=BUTTON_LONG_TIME;		// ����ʱ�� 	
	s_tBtnKey1.byState=0;						// ������ǰ״̬�����»��ǵ���
	s_tBtnKey1.byKeyCodeUp=KEY1_UP;				// ��������ļ�ֵ����, 0��ʾ����ⰴ������ 
	s_tBtnKey1.byKeyCodeDown=KEY1_DOWN;			// �������µļ�ֵ����, 0��ʾ����ⰴ������
	s_tBtnKey1.byKeyCodeLong=KEY1_LONG;			// ���������ļ�ֵ����, 0��ʾ����ⳤ�� 
	
	s_tBtnKey2.IsKeyDownFunc=IsKey2Down;		// ��ⰴ�����º���
	s_tBtnKey2.wFilterCount=0;					// �˲��������� 
	s_tBtnKey2.wFilterTime =BUTTON_FILTER_TIME;	// �˲�ʱ�� 
	s_tBtnKey2.wLongCount =0;					// ���������� 
	s_tBtnKey2.wLongTime=BUTTON_LONG_TIME;		// ����ʱ�� 	
	s_tBtnKey2.byState=0;						// ������ǰ״̬�����»��ǵ���
	s_tBtnKey2.byKeyCodeUp=KEY2_UP;				// ��������ļ�ֵ����, 0��ʾ����ⰴ������ 
	s_tBtnKey2.byKeyCodeDown=KEY2_DOWN;			// �������µļ�ֵ����, 0��ʾ����ⰴ������
	s_tBtnKey2.byKeyCodeLong=KEY2_LONG;			// ���������ļ�ֵ����, 0��ʾ����ⳤ�� 

	s_tBtnKey3.IsKeyDownFunc=IsKey3Down;		// ��ⰴ�����º���
	s_tBtnKey3.wFilterCount=0;					// �˲��������� 
	s_tBtnKey3.wFilterTime =BUTTON_FILTER_TIME;	// �˲�ʱ�� 
	s_tBtnKey3.wLongCount =0;					// ���������� 
	s_tBtnKey3.wLongTime=BUTTON_LONG_TIME;		// ����ʱ�� 	
	s_tBtnKey3.byState=0;						// ������ǰ״̬�����»��ǵ���
	s_tBtnKey3.byKeyCodeUp=KEY3_UP;				// ��������ļ�ֵ����, 0��ʾ����ⰴ������ 
	s_tBtnKey3.byKeyCodeDown=KEY3_DOWN;			// �������µļ�ֵ����, 0��ʾ����ⰴ������
	s_tBtnKey3.byKeyCodeLong=KEY3_LONG;			// ���������ļ�ֵ����, 0��ʾ����ⳤ�� 

	s_tBtnKey4.IsKeyDownFunc=IsKey4Down;		// ��ⰴ�����º���
	s_tBtnKey4.wFilterCount=0;					// �˲��������� 
	s_tBtnKey4.wFilterTime =BUTTON_FILTER_TIME;	// �˲�ʱ�� 
	s_tBtnKey4.wLongCount =0;					// ���������� 
	s_tBtnKey4.wLongTime=BUTTON_LONG_TIME;		// ����ʱ�� 	
	s_tBtnKey4.byState=0;						// ������ǰ״̬�����»��ǵ���
	s_tBtnKey4.byKeyCodeUp=KEY4_UP;				// ��������ļ�ֵ����, 0��ʾ����ⰴ������ 
	s_tBtnKey4.byKeyCodeDown=KEY4_DOWN;			// �������µļ�ֵ����, 0��ʾ����ⰴ������
	s_tBtnKey4.byKeyCodeLong=KEY4_LONG;			// ���������ļ�ֵ����, 0��ʾ����ⳤ�� 
	
	QueueInit(s_byBtnBuf, 256, &st_BtnQueue);	// �����¼����г�ʼ��
}

/*******************************************************************************************************
** ����: ButtonDetect,Button������⺯��
**------------------------------------------------------------------------------------------------------
** ����: ptBtn �����ṹ��ָ��
** ����: void
********************************************************************************************************/
static void ButtonDetect(Button_t *ptBtn)
{
	if(ptBtn->IsKeyDownFunc && ptBtn->IsKeyDownFunc()) 				// ��ⰴ�������Ƿ���ڣ������Ƿ���
	{

		if(ptBtn->wFilterCount < ptBtn->wFilterTime)				// �˲���⣬�˲������������˲�ʱ��
		{
			ptBtn->wFilterCount++;									// ��������һ
			return;													// �˳���⺯��
		}
		
		if(ptBtn->byState ==0 ) 									// ����Ƿ��ǰ�������							
		{
			ptBtn->byState = 1;
			QueueInPut(ptBtn->byKeyCodeDown,&st_BtnQueue);			// ���밴���¼�����
			return;
		}
		if( ptBtn->wLongCount++ == ptBtn->wLongTime) 				// ����Ƿ��ǰ��������������������Ƿ񵽴ﳤ��ʱ��
		{
			QueueInPut(ptBtn->byKeyCodeLong,&st_BtnQueue);			// ���밴���¼�����
			return;
		}			
	}
	else 
	{		
		if(ptBtn->wFilterCount) 									// �˲���⣬�˲��������Ƿ�Ϊ0
		{
			ptBtn->wFilterCount--;									// ��������һ
			return;													// �˳���⺯��
		}			
		
		if(ptBtn->byState ==1 )										// ����Ƿ��ǰ�������
		{
			ptBtn->byState =0; 										
			QueueInPut(ptBtn->byKeyCodeUp,&st_BtnQueue);			// ���밴���¼�����
		}
		
		ptBtn->wLongCount = 0;										// ������������������
	}
	
}

/*******************************************************************************************************
** ����: ButtonInit,Button��ʼ��
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: void
********************************************************************************************************/
void ButtonInit(void)
{
	ButtonGpioInit();				// Button ����GPIO��ʼ��
	ButtonVarInit();				// Button �������ó�ʼ��
}

/*******************************************************************************************************
** ����: ButtonProj,����������
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: void
********************************************************************************************************/
void ButtonProj(void)
{
	ButtonDetect(&s_tBtnKey1);		// ��� K1 �� 
	ButtonDetect(&s_tBtnKey2);		// ��� K2 �� 
	ButtonDetect(&s_tBtnKey3);		// ��� K3 �� 
	ButtonDetect(&s_tBtnKey4);		// ��� K4 �� 
}


/*******************************************************************************************************
** ����: ButtonGetKey,��ȡ�����¼�����
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: �����¼�
********************************************************************************************************/
int ButtonGetKey(void)
{
	char byRet=0;
	if(!st_BtnQueue.unResidue) return FALSE;	// �����¼�����Ϊ�գ��˳�
	QueueOutPut(&byRet,&st_BtnQueue);			// ��ȡ�����¼� 
	return byRet;								
}



/********************************************************************************************************
**                            End Of File
********************************************************************************************************/
