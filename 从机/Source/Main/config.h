/*******************************************************************************************************
**------------------------------------------------------------------------------------------------------
** �ļ�: config.h 
** �汾: v1.0
**------------------------------------------------------------------------------------------------------
** ����:
**      ���ڴ�ſ������̵�ȫ�ֱ���
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef __CONFIG__H__
#define __CONFIG__H__

//----------------------------------------------------------------
//�޷������ͺ궨��
#define uchar 	unsigned char													 
#define uint 	unsigned int

#define uint8 	unsigned char 
#define uint16 	unsigned short int
#define uint32 	unsigned int

//----------------------------------------------------------------
// MAIN_GLOBAL��	main.c ����#define  MAIN_GLOBAL
#ifdef 	 MAIN_CONFIG										// ͷ�ļ������C�ļ�����,���������ͻ������ 
	#define  EXT											// ���ļ��ж���ı���
#else
	#define  EXT	extern									// ȫ���ļ�����������
#endif
	
//----------------------------------------------------------------
//��������ͷ�ļ�  
#include "stm32f10x.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "system.h"
#include "RemapGPIO.h"

//---------------------------------------------------------------
//��������ͷ�ļ�
#include "Usart.h"
#include "Queue.h"
#include "Timer.h"
#include "SysTick.h"
#include "Button.h"
#include "usart2_HMI.h"	
#include "I2C.h"
#include "AT24C02.h"
#include "usart3_ZigBee.h"
//---------------------------------------------------------------
//����ȫ�ֱ���
extern const uchar	 gImage_Back[192000];

//---------------------------------------------------------------
//����ȫ�ֺ궨��
#define CPU_INT_DISABLE()   { __set_PRIMASK(1); }		// �ر����ж� 
#define CPU_INT_ENABLE()    { __set_PRIMASK(0);	} 		// �������ж�

//---------------------------------------------------------------
//����ȫ�����Ͷ���
typedef enum{FALSE=0,TRUE=!FALSE,}BOOL;					// �ɹ�orʧ��

//---------------------------------------------------------------
//����ȫ��ϵͳ��ʱ��
#define  MAX_TIMER  		4							// ���ʱ������
EXT volatile unsigned long  g_dwSysTimer[MAX_TIMER];	// ����ϵͳ��ʱ�� 
#define  Char_wait		g_dwSysTimer[0]					// �������ݽ��ն�ʱ��
#define  DelayTimer  	g_dwSysTimer[1]	 				// ��ʱ��ʱ��
#define  ViewTimer  	g_dwSysTimer[2]	 				// Һ����ʱ��
#define  PutEmtTimer  	g_dwSysTimer[3]	 				// ��ʱ��1

#define  TIMER_SEC  		(1000)							// ��
#define  TIMER_MIN  		(SEC*60)						// ��
#define  TIMER_HOUR  		(MIN*60)						// ʱ
#define  TIMER_DAY  		(HOUR*24)						// ��
#endif

/********************************************************************************************************
**                            End Of File
********************************************************************************************************/

