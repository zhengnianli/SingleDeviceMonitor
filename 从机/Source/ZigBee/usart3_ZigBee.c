/*******************************************************************************************************
**------------------------------------------------------------------------------------------------------
** �ļ�	: 	usart3_ZigBee.c			
** �汾	: 	v1.0 
**------------------------------------------------------------------------------------------------------
** ����	:	
		���ļ�ΪZigBee��������
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "config.h"
#include "usart3_ZigBee.h"
#include "stdarg.h"	 

//һЩȫ�ֱ������� 
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; 			//���ջ���,���USART3_MAX_RECV_LEN���ֽ�.
u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; 			//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
u8 wait_time = 0;
vu16 USART3_RX_STA = 0;  
u8 ElecNum=1;  //�õ������
char STa_Store[20],U_Store[20],I_Store[20],P_Store[20],Q_Store[20],ElecNum_Store[20];
float U,I,P,Q;

/*******************************************************************************************************
** ����: uart3_ZigBee_init��zigbeeģ���ʼ��
**------------------------------------------------------------------------------------------------------
** ����: bound��������
** ����: ��
** ˵��: 
********************************************************************************************************/
void uart3_ZigBee_init(u32 bound)
{  	 
	//GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��UGPIOBʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//ʹ��USART3ʱ��
	//USART3_TX  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
    GPIO_Init(GPIOB, &GPIO_InitStructure);
   
    //USART3_RX	  
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PB11
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //Usart2 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
   //USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
    USART_Init(USART3, &USART_InitStructure);     //��ʼ������3
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
    USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���3
	
	TIM3_Int_Init(99,7199);		//10ms�ж�
	USART3_RX_STA=0;		//����
	TIM_Cmd(TIM3,DISABLE);			//�رն�ʱ��7
}

/*******************************************************************************************************
** ����: u3_printf������3 ,printf ����
**------------------------------------------------------------------------------------------------------
** ����: ��κ���
** ����: ��
** ˵��: ȷ��һ�η������ݲ�����USART3_MAX_SEND_LEN�ֽ�
********************************************************************************************************/
void u3_printf(char* fmt,...)  
{  
	u16 i,j; 
	va_list ap; 
	va_start(ap,fmt);
	vsprintf((char*)USART3_TX_BUF,fmt,ap);
	va_end(ap);
	i=strlen((const char*)USART3_TX_BUF);		//�˴η������ݵĳ���
	for (j = 0; j < i; j++)							//ѭ����������
	{
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
		USART_SendData(USART3,USART3_TX_BUF[j]); 
	} 
}

/*******************************************************************************************************
** ����: USART3_IRQHandler������3�жϺ���
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: void
** ˵��: 
********************************************************************************************************/
void USART3_IRQHandler(void)
{
	u8 res;
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)//���յ�����
	{	 
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		res = USART_ReceiveData(USART3);	
		if (USART3_RX_STA < 598)
		{
			USART3_RX_BUF[USART3_RX_STA++] = res;	//��¼���յ���ֵ	
			USART3_RX_BUF[USART3_RX_STA] = 0;
		}
		wait_time=5;	
	}  				 											 
}   

/*******************************************************************************************************
** ����: DataHandle������3�������ݴ�����
**------------------------------------------------------------------------------------------------------
** ����: void
** ����: void
** ˵��: 
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
	printf("���ݰ���%s", USART3_RX_BUF);
	printf("\n");
	if(sscanf((char*)USART3_RX_BUF,"A:U:%f,I:%f,P:%f,Q:%f,UEA1:%1d,UEA2:%1d,UEA3:%1d,UEA4:%1d,UEA5:%1d,UEA6:%1d,UEA7:%1d\r\n",
			&U,&I,&P,&Q,&uea1_sta,&uea2_sta,&uea3_sta,&uea4_sta,&uea5_sta,&uea6_sta,&uea7_sta)==11)	
	{
		printf("��Դ�߲�����U=%0.1fV,I=%0.2fA,P=%0.1fW,Q=%0.1fVar\n",U,I,P,Q);
		printf("7���õ���״̬��%d,%d,%d,%d,%d,%d,%d\n",uea1_sta,uea2_sta,uea3_sta,uea4_sta,uea5_sta,uea6_sta,uea7_sta);
		if (P > 1000)
		{
			uea7_sta = 1;
		}
		//----------------------------------------------------------------------------------------------- 
		//����������������������ʾ
		//��ѹ
		sprintf(U_Store,"t11.txt=\"%0.1fV\"",U);
		HMISendStr(U_Store);
		HMISendByte(0xff);
		// ����
		sprintf(I_Store,"t12.txt=\"%0.3fA\"",I);
		HMISendStr(I_Store);
		HMISendByte(0xff);
		//�й�����
		sprintf(P_Store,"t13.txt=\"%0.3fW\"",P);
		HMISendStr(P_Store);
		HMISendByte(0xff);
		//�޹�����
		sprintf(Q_Store,"t14.txt=\"%0.2fVar\"",Q);
		HMISendStr(Q_Store);
		HMISendByte(0xff);
		
		//�õ���1
		sprintf(STa_Store,"t0.txt=\"%d\"",uea1_sta);
		HMISendStr(STa_Store);
		HMISendByte(0xff);
		//�õ���2
		sprintf(STa_Store,"t1.txt=\"%d\"",uea2_sta);
		HMISendStr(STa_Store);
		HMISendByte(0xff);
		//�õ���3
		sprintf(STa_Store,"t2.txt=\"%d\"",uea3_sta);
		HMISendStr(STa_Store);
		HMISendByte(0xff);
		//�õ���4
		sprintf(STa_Store,"t3.txt=\"%d\"",uea4_sta);
		HMISendStr(STa_Store);
		HMISendByte(0xff);
		//�õ���5
		sprintf(STa_Store,"t4.txt=\"%d\"",uea5_sta);
		HMISendStr(STa_Store);
		HMISendByte(0xff);
		//�õ���6
		sprintf(STa_Store,"t5.txt=\"%d\"",uea6_sta);
		HMISendStr(STa_Store);
		HMISendByte(0xff);
		//�õ���7
		sprintf(STa_Store,"t6.txt=\"%d\"",uea7_sta);
		HMISendStr(STa_Store);
		HMISendByte(0xff);
	}
    else if(sscanf((char*)USART3_RX_BUF,"S:U:%f,I:%f,P:%f,Q:%f,ElecNum:%d\r\n",&U,&I,&P,&Q,&ElecNum)==5)
	{
		printf("ѧϰ��U=%0.1fV,I=%0.2fA,P=%0.1fW,Q=%0.1fVar,ElecNum=%d\n",U,I,P,Q,ElecNum);
		
		//��ѹ
		sprintf(U_Store,"t16.txt=\"%0.1fV\"",U);
		HMISendStr(U_Store);
		HMISendByte(0xff);
		//����
		sprintf(I_Store,"t17.txt=\"%0.3fA\"",I);
		HMISendStr(I_Store);
		HMISendByte(0xff);
		//�й�����
		sprintf(P_Store,"t18.txt=\"%0.3fW\"",P);
		HMISendStr(P_Store);
		HMISendByte(0xff);
		//�޹�����
		sprintf(Q_Store,"t19.txt=\"%0.2fVar\"",Q);
		HMISendStr(Q_Store);
		HMISendByte(0xff);
		//���
		sprintf(ElecNum_Store,"t15.txt=\"%d\"",ElecNum);
		HMISendStr(ElecNum_Store);
		HMISendByte(0xff);
	}
		
	USART3_RX_STA=0;
}


//void StudyMode(void)
//{
//	char U_Buffer[20],I_Buffer[20],P_Buffer[20],Q_Buffer[20];
//	//���ڱ��
//	if(IsKey3Down())  //���޸�
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
//	//ȷ�ϰ������£���ʼѧϰ
//	if(IsKey1Down()) //���޸�
//	{
//		//��ѹ
//		sprintf(U_Store,"t17.txt=\"%0.1fV\"",U);
//		HMISendStr(U_Store);
//		HMISendByte(0xff);
//		//����
//		sprintf(I_Store,"t16.txt=\"%0.3fA\"",I);
//		HMISendStr(I_Store);
//		HMISendByte(0xff);
//		//�й�����
//		sprintf(P_Store,"t18.txt=\"%0.3fW\"",P);
//		HMISendStr(P_Store);
//		HMISendByte(0xff);
//		sprintf(P_Buffer,"%0.3f",P);
//		At24c02Write(0,(unsigned char*)P_Buffer,sizeof(P_Buffer));
//		//�޹�����
//		sprintf(Q_Store,"t19.txt=\"%0.2fVar\"",Q);
//		HMISendStr(Q_Store);
//		HMISendByte(0xff);
//		
//	}
//	
//}
