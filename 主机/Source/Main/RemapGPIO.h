/*******************************************************************************************************
**------------------------------------------------------------------------------------------------------
** �ļ�: RemapGPIO.h
** �汾: v1.0
**------------------------------------------------------------------------------------------------------
** ����:
**     GPIO��ӳ���
**	   �ο�ԭ�����;����������ж˿ڵ�����,��ĳЩ�Բ���Ч����Ҫ�����������������ַ�ʽ
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "stm32f10x.h"

//-------------------------------------------------------------------------------
// STM32λ�������
/*	
**						 
**				
**							  λ����					
**	STM32�о���������ʵ����λ����SRAM	0x20000000	-	0x200FFFFF(1MB)
**								 FLASH 	0x40000000	-	0x400FFFFF(1MB)
**							
**							 λ��������
**	λ�����еĵ�ַ���˿���ͬRAMһ��ʹ���⣬�������Լ��ġ�λ������������ÿ��
**λ��չΪ32λ���֣��ȳ���λ��������
**
**	�磺0x20000000��һ���ֽڣ���չ��32λ�ɵ�8��32λ���ֱַ��ǣ�
**		0x22000000	0x22000004	0x22000008	0x2200000c
**		0x22000010	0x22000014	0x22000018	0x2200001c
**
**	�ɴ˿ɵá�λ������ �������ַ�� ��λ��������ַ�� 
**
**	SRAM  :		0x22000000 +((A�\0x20000000)*8+n)*4
**	FLASH :		0x42000000 +((A�\0x40000000)*8+n)*4
**	��	  :		
**				0x22000000+ (A�\0x20000000)*32 + n*4
**				0x42000000+ (A�\0x40000000)*32 + n*4
**				��������ַ = λ����������ʼ��ַ+ƫ����
**
**	��ע  ��	�ֽڵĵ�ַ��	A
**				λ���	  ��	n	��0<n<7��
**	
**	��	  ��	0x22000000 + ((0x20000000 - 0x20000000 )*8+4)*4
**			 =  0x22000010 		
**	��	  :		0x20000000��4λ����������ַ
**
**  "λ����ַ��λ���" ת��������ַ 
**	#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
*/
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// IO������
#define BITBAND(addr, bitnum) 		((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))		// ����bit����ַƫ��
#define MEM_ADDR(addr)  			(*((vu32  *)(addr) ))												  	// ǿ��ת��Ϊָ��
#define BIT_ADDR(addr, bitnum)  	MEM_ADDR( BITBAND(addr, bitnum)  )

//-------------------------------------------------------------------------------
// IO�ڵ�ַӳ��
#define GPIOA_ODR_Addr  (GPIOA_BASE+12) 					// 0x4001080C 
#define GPIOB_ODR_Addr  (GPIOB_BASE+12) 					// 0x40010C0C 
#define GPIOC_ODR_Addr  (GPIOC_BASE+12) 					// 0x4001100C 
#define GPIOD_ODR_Addr  (GPIOD_BASE+12) 					// 0x4001140C 
#define GPIOE_ODR_Addr  (GPIOE_BASE+12) 					// 0x4001180C 
#define GPIOF_ODR_Addr  (GPIOF_BASE+12) 					// 0x40011A0C    
#define GPIOG_ODR_Addr  (GPIOG_BASE+12) 					// 0x40011E0C    

#define GPIOA_IDR_Addr  (GPIOA_BASE+8) 						// 0x40010808 
#define GPIOB_IDR_Addr  (GPIOB_BASE+8) 						// 0x40010C08 
#define GPIOC_IDR_Addr  (GPIOC_BASE+8) 						// 0x40011008 
#define GPIOD_IDR_Addr  (GPIOD_BASE+8) 						// 0x40011408 
#define GPIOE_IDR_Addr  (GPIOE_BASE+8) 						// 0x40011808 
#define GPIOF_IDR_Addr  (GPIOF_BASE+8) 						// 0x40011A08 
#define GPIOG_IDR_Addr	(GPIOG_BASE+8) 						// 0x40011E08 

//-------------------------------------------------------------------------------
// IO�ڲ���,ֻ�Ե�һ��IO��;ȷ��n��ֵС��16!
#define PAout(n)   		BIT_ADDR(GPIOA_ODR_Addr,n)  		// ��� 
#define PAin(n)    		BIT_ADDR(GPIOA_IDR_Addr,n)  		// ���� 

#define PBout(n)   		BIT_ADDR(GPIOB_ODR_Addr,n)  		// ��� 
#define PBin(n)    		BIT_ADDR(GPIOB_IDR_Addr,n)  		// ���� 

#define PCout(n)   		BIT_ADDR(GPIOC_ODR_Addr,n)  		// ��� 
#define PCin(n)    		BIT_ADDR(GPIOC_IDR_Addr,n)  		// ���� 

#define PDout(n)   		BIT_ADDR(GPIOD_ODR_Addr,n)  		// ��� 
#define PDin(n)    		BIT_ADDR(GPIOD_IDR_Addr,n)  		// ���� 

#define PEout(n)   		BIT_ADDR(GPIOE_ODR_Addr,n)  		// ��� 
#define PEin(n)    		BIT_ADDR(GPIOE_IDR_Addr,n)  		// ����

#define PFout(n)   		BIT_ADDR(GPIOF_ODR_Addr,n)  		// ��� 
#define PFin(n)    		BIT_ADDR(GPIOF_IDR_Addr,n)  		// ����

#define PGout(n)   		BIT_ADDR(GPIOG_ODR_Addr,n)  		// ��� 
#define PGin(n)    		BIT_ADDR(GPIOG_IDR_Addr,n)  		// ����

/********************************************************************************************************
**                            End Of File
********************************************************************************************************/
