/********************************************************************************************************
**------------------------------------------------------------------------------------------------------
** �ļ�: ds18b20.c
** �汾: v1.0
**------------------------------------------------------------------------------------------------------
** ����:
**     �˲���Ϊ18B20����������
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "config.h"


//-------------------------------------------------------------------------------
// ��ʱ��־λ
static int s_iIsTimer=0;
extern vu16 USART3_RX_STA;
/*******************************************************************************************************
** ����: Tim2VarInit, Tim2���ó�ʼ��
**------------------------------------------------------------------------------------------------------
** ����: ��ʱ���������
** ����: void
********************************************************************************************************/
static void Tim2VarInit(int Var)
{
	//-------------------------------------------------------------------------------
	// ����TIM��TIMʱ��
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	TIM_DeInit(TIM2);										//���½�Timer����Ϊȱʡֵ
	TIM_InternalClockConfig(TIM2);							//�����ڲ�ʱ�Ӹ�TIM2�ṩʱ��Դ
	TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;			//Ԥ��Ƶϵ��Ϊ36000-1������������ʱ��Ϊ72MHz/36000 = 2kHz
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	//����ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���ü�����ģʽΪ���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period = (Var<2)?1:Var-1;		//���ü��������С��ÿ��Var�����Ͳ���һ�������¼�
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);			//������Ӧ�õ�TIM2��
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);					//�������жϱ�־
	TIM_ARRPreloadConfig(TIM2, DISABLE);					//��ֹARRԤװ�ػ�����
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);				//����TIM2���ж�
	TIM_Cmd(TIM2,ENABLE);
}
/*******************************************************************************************************
** ����: Tim2NvicInit, Tim2�жϳ�ʼ��
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: void
********************************************************************************************************/
static void Tim2NvicInit(void)
{
	//------------------------------------------------------------------------------------------------
	// ����TIM�ж�	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;      	// ѡ��TIM2���ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	// ��ռʽ�ж����ȼ�����Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		// ��Ӧʽ�ж����ȼ�����Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			// �ж�ʹ��
	NVIC_Init(&NVIC_InitStructure);							// �����ж�

}
/*******************************************************************************************************
** ����: Tim2Init, Tim��ʱ����ʱ����
**------------------------------------------------------------------------------------------------------
** ����: us ��ʱʱ��
** ����: void
********************************************************************************************************/
void Tim2Init(void)
{
	Tim2VarInit(1000);
	Tim2NvicInit( );	
}

/*******************************************************************************************************
** ����: Timer1DelayUs, ��ʱ��us��ʱ����
**------------------------------------------------------------------------------------------------------
** ����: us ��ʱʱ��
** ����: void
********************************************************************************************************/
void TimDelayUs(unsigned int us)
{
	s_iIsTimer=1;				// ��ʱ��־λ��λ
	CPU_INT_ENABLE() 			// �����жϡ�ȷ���жϴ�
	Tim2VarInit(us);			// ���ö�ʱʱ��
	while(s_iIsTimer);			// �ȴ���ʱ��־λ��λ
}

/*******************************************************************************************************
** ����: TIM2_IRQHandler, ��ʱ��TIM2�жϺ���
**------------------------------------------------------------------------------------------------------
** ����: us ��ʱʱ��
** ����: void
********************************************************************************************************/
void TIM2_IRQHandler(void)
{
	//------------------------------------------------------------------------------------------------
	// ����Ƿ�����������¼�
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		//------------------------------------------------------------------------------------------------
		// TIM2���жϴ���
		
		s_iIsTimer=0;										// ��ʱ��־λ��λ
		TIM_ITConfig(TIM2,TIM_IT_Update,DISABLE);			// �ر��ж�
		//------------------------------------------------------------------------------------------------
		// ���TIM2���жϴ�����λ
		TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);

	}
}



//��ʱ��7�жϷ������		    
void TIM3_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)//�Ǹ����ж�
	{	 	
			
		USART3_RX_STA|=1<<15;	//��ǽ������
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  );  //���TIM3�����жϱ�־ 
	
	
		TIM_Cmd(TIM3, DISABLE);  //�ر�TIM3 
	}	    
}
 
//ͨ�ö�ʱ��7�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ42M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz 
//ͨ�ö�ʱ���жϳ�ʼ��
//����ʼ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��		 
void TIM3_Int_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//TIM3ʱ��ʹ��    
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�
	
	TIM_Cmd(TIM3,ENABLE);//������ʱ��7
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
}
/********************************************************************************************************
**                            End Of File
********************************************************************************************************/
