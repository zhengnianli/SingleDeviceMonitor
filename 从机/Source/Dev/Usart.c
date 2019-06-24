/********************************************************************************************************
**------------------------------------------------------------------------------------------------------
** �ļ�: Usart.c
** �汾: v1.0
**------------------------------------------------------------------------------------------------------
** ����:
**     ϵͳ���Դ����������򻯺�����������ֹ���ڷ���������ѭ��
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "config.h"
#include "Usart.h"

//-------------------------------------------------------------------------------
// ���ж���
#define  RECBUF_MAX  	256									// ���������ݻ�����
static Queue_t 	s_tQueueUsart ; 		                    // ���Դ���
static char 	s_sRecDatBuf[RECBUF_MAX]={0};				// ���н��ջ���Buff
static char 	s_sUsartBuf [RECBUF_MAX]={0};				// ���ڴ�����Buff

/*******************************************************************************************************
** ����: UsartInit, ����1��ʼ������
**------------------------------------------------------------------------------------------------------
** ����: USART1 baudrate(������)
** ����: ��
********************************************************************************************************/
void UsartInit(  unsigned long baudrate )
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	//------------------------------------------------------------------------------------------------
	// ����IO��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);	// ʹ��GPIOAʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO  , ENABLE);	// ʹ�ܸ��ù���ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	// ʹ�ܴ���ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;				// USART1��TX��PA9	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			// ���óɸ������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				// USART1��RX��PA10	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	// ���óɸ�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//------------------------------------------------------------------------------------------------
	// ���ý����ж�	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = Usart_PreemptionPriority;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = Usart_SubPriority;	 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

//------------------------------------------------------------------------------------------------
	// ���ڲ�������		
	USART_InitStructure.USART_BaudRate = baudrate ;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			// ʹ�ܶ�Ӧ�Ľ����ж�
	USART_Init(USART1, &USART_InitStructure);				// ��ʼ�����ڲ���
	USART_Cmd(USART1, ENABLE);								// ʹ�ܴ��� 
	USART_ClearFlag(USART1, USART_FLAG_TC|USART_FLAG_TC);	// ������ڽ��շ��ͱ�־λ

	//------------------------------------------------------------------------------------------------
	// ��ʼ�����ն���
	QueueInit(s_sRecDatBuf, RECBUF_MAX, &s_tQueueUsart);		// Usart���ڽ��ն��г�ʼ��
}

/*******************************************************************************************************
** ����: Usart_SendCh, ��Ӧ���ڷ����ֽں���
**------------------------------------------------------------------------------------------------------
** ����: ����(�ֽ�)
** ����: ��
********************************************************************************************************/
int Usart_SendCh( unsigned char ch )
{	
	int cnt = 0;
	while( USART_GetFlagStatus(USART1, USART_FLAG_TC) ==RESET ) 		
		if( cnt++ > 0xfff )		return FALSE;
	USART_SendData(USART1,ch);			
	return TRUE;								
}

/*******************************************************************************************************
** ����: Usart_SendBuf, ���ڷ���֡
**------------------------------------------------------------------------------------------------------
** ����: �ַ���	, ����
** ����: ��
********************************************************************************************************/
int Usart_SendBuf(char *dat , unsigned int len)
{
	while(len--)
	{
		if( Usart_SendCh(*(dat++) ) < 0 )
			return FALSE;
	}			  
	return TRUE;
}	

/*******************************************************************************************************
** ����: fputc, �ض���putc����	��ʹ�ܹ�����printf�����Ӵ��ڴ�ӡ���
**------------------------------------------------------------------------------------------------------
** ����: NONE
** ����: NONE
********************************************************************************************************/
int fputc(int ch, FILE *f)
{
	return 	Usart_SendCh( ch );
}

/*******************************************************************************************************
** ����: USART1_IRQHandler, USART1�жϷ������
**------------------------------------------------------------------------------------------------------
** ����: �ַ���	, ����
** ����: ��
********************************************************************************************************/
void USART1_IRQHandler(void)
{	
	if( USART_GetFlagStatus(USART1,USART_IT_RXNE) !=RESET )				// ���ڽ������� 
	{				
		Char_wait=10;													// �������ݽ��ն�ʱ��
		QueueInPut( (char)USART_ReceiveData(USART1),&s_tQueueUsart );	// ��ȡ�����������
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);					// ������ձ�־λ
	}
	
	if( USART_GetFlagStatus(USART1,USART_FLAG_ORE)==SET ) 				// �����������
	{
		USART_ReceiveData(USART1); 										// �����������
		USART_ClearFlag(USART1,USART_FLAG_ORE); 						// ��������־
	}

}

/*******************************************************************************************************
** ����: UsartReceive, ���ڽ��ղ�����
**------------------------------------------------------------------------------------------------------
** ����: NULL
** ����: �ɹ�orʧ��
********************************************************************************************************/
int UsartReceive(void)
{
	unsigned char i=0,len=0;

	//----------------------------------------------------------------
	// �ȴ����ݽ������
	if( !s_tQueueUsart.unResidue )			return FALSE;	// ���յ���������
	if( Char_wait )							return FALSE;	// ���ݽ��ն�ʱ����Ϊ0���˳�

	//----------------------------------------------------------------
	// �������ݻ�ȡ
	len=s_tQueueUsart.unResidue;							// ȡ����
	for( i=0;i<len;i++ )									// ���������ݶ�ȡ������ָ�����
	{
		QueueOutPut(&s_sUsartBuf[i] ,&s_tQueueUsart); 	
	}
	s_sUsartBuf[len]=0;										// ��ָ��ΪASCII�ַ�����Ϊָ����ӽ���λ
	
	//----------------------------------------------------------------
	// 
	if( (strstr(s_sUsartBuf,"Get(U)")) != NULL ){ 
		
		
	}
	else goto Error ;										// �޴˹ؼ����˳�
	
	//----------------------------------------------------------------
	// ��ȷ��Ӧ��ʾ
	
	return TRUE;
		
Error:
	//----------------------------------------------------------------
	// �����Ӧ��ʾ
	
	return FALSE; 	
}
/********************************************************************************************************
**											End Of File												   
********************************************************************************************************/
