/*******************************************************************************************************
**------------------------------------------------------------------------------------------------------
** �ļ�	: 	usart2_HMI.c			
** �汾	: 	v1.0 
**------------------------------------------------------------------------------------------------------
** ����	:	
		���ļ���������������
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "config.h"
#include "usart2_HMI.h"

unsigned char HMI_receive_len = 0;	// ���ڽ�����������ĳ���
unsigned char HMI_receive[32];	    // ���ڽ��ջ�������

/*******************************************************************************************************
** ����: uart2_HMI_init��������Ӳ����ʼ��
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: ��
** ˵��: 
********************************************************************************************************/
void uart2_HMI_init(u32 bound)
{  	 
	  //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��UGPIOAʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//ʹ��USART2ʱ��
	//USART2_TX  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA2
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);
   
    //USART2_RX	  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //Usart2 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
   //USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USART2, &USART_InitStructure);     //��ʼ������2
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
    USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2
}

/*******************************************************************************************************
** ����: USART2_IRQHandler������2�жϺ���
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: ��
** ˵��: 
********************************************************************************************************/
void USART2_IRQHandler(void)
{
	unsigned char key_receive=0;//������������ز���
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //���յ�����
	{
		USART_ClearITPendingBit(USART2,USART_IT_RXNE); //����жϱ�־. 
		key_receive=USART_ReceiveData(USART2); 
		HMI_receive[HMI_receive_len++]=USART_ReceiveData(USART2); 
		switch(key_receive)
		{
			case 1:	 
				printf("���յ�����1���ź�\r\n");
			break;
			case 2:	 
				printf("���յ�����2���ź�\r\n");
			break;
			case 3:	 
				printf("���յ�����3���ź�\r\n");
			break;
			case 4:	 
				printf("���յ�����4���ź�\r\n");
			break;
			default:
			break;
		}	  	 	 
	}	
}

/*******************************************************************************************************
** ����: HMISendByte���������������ֽ�����
**------------------------------------------------------------------------------------------------------
** ����: k
** ����: ��
** ˵��: ����������������3����0xFF���������
********************************************************************************************************/
void HMISendByte(u8 k)
{
	u8 i;
	for (i = 0; i < 3; i++)
	{
		if (k != 0)
		{
			USART_SendData(USART2, k);                                    //�����ֽ�����
			while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){} //�ȴ����ͽ���   
		}
		else
			return;
	}
}

/*******************************************************************************************************
** ����: HMISendStart��Ϊȷ������HMI����ͨ��
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: ��
** ˵��: 
********************************************************************************************************/
void HMISendStart(void)
{
	SysTickDelayMS(200);
	HMISendByte(0xff);
	SysTickDelayMS(200);
	//��ӡ��page0ҳ��
	HMISendStr("page main");
 	HMISendByte(0xff);
}

/*******************************************************************************************************
** ����: HMISendStr���򴮿��������ַ��� 
**------------------------------------------------------------------------------------------------------
** ����: k
** ����: ��
** ˵��: 
********************************************************************************************************/
void HMISendStr(char *buf)
{
	u8 i=0;
	while(1)
	{
		if(buf[i]!=0)
		{	
		
			 USART_SendData(USART2, buf[i]);   //����һ���ֽ�
			 while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET){} //�ȴ����ͽ���
			i++;
		}
		else
			return;
	}
}
/********************************************************************************************************
**                            End Of File
********************************************************************************************************/
