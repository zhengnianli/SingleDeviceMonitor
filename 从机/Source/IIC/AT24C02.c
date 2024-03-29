/*******************************************************************************************************
**------------------------------------------------------------------------------------------------------
** 文件: AT24C02 
** 版本: v1.0
**------------------------------------------------------------------------------------------------------
** 描述:
**      AT24c02驱动函数
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "config.h"
#include "AT24C02.h"


/*******************************************************************************************************
** 函数: At24c02Init, At24c02初始化
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: void
*******************************************************************************************************/
void At24c02Init(void)
{
	I2cGpioInit();
}


/*******************************************************************************************************
** 函数: At24c02WriteByte,向指定地址写入一字节数据
**------------------------------------------------------------------------------------------------------
** 参数: addr 地址 ，buff 数据
** 返回: void
********************************************************************************************************/
void At24c02WriteByte(unsigned char addr,unsigned char buff)
{	
	I2cStart();
	I2cWriteByte(0xa0); I2cNoACK();
	I2cWriteByte(addr); I2cNoACK();
	I2cWriteByte(buff); I2cNoACK();	
 	I2cStop();
	
}

/*******************************************************************************************************
** 函数: At24c02ReadByte,从指定地址读取一字节数据
**------------------------------------------------------------------------------------------------------
** 参数: addr 地址 ，buff 数据指针
** 返回: void
********************************************************************************************************/
void At24c02ReadByte(unsigned char addr,unsigned char *buff)
{
	I2cStart();
	I2cWriteByte(0xa0);I2cRACK(); 
	I2cWriteByte(addr);I2cRACK(); 	
	I2cStart();
	I2cWriteByte(0xa1);I2cRACK(); 	
	I2cReadByte(buff); I2cRACK();
	I2cStop();
}
/*******************************************************************************************************
** 函数: At24c02Write,向指定地址写字符串数据
**------------------------------------------------------------------------------------------------------
** 参数: addr 地址 ，buff 数据指针 ，size 数据长度
** 返回: void
** 说明: AT24C02每页8个字节，每次写操作只能对一页进行，故本函数递归调用写入数据
********************************************************************************************************/
void At24c02Write(unsigned char addr,unsigned char *buff,int size)
{	
	int i,len=size;
	if(size>8) len=8;
	if(size>(8-addr%8)) len=8-addr%8;
	if(size-len)At24c02Write(addr+len,buff+len,size-len);
	I2cStart();
	I2cWriteByte(0xa0); I2cRACK();
	I2cWriteByte(addr); I2cRACK();
	for(i=0;i<len;i++)
	{
		I2cWriteByte(*(buff+i)); I2cRACK();		
	}
 	I2cStop();		
}

/*******************************************************************************************************
** 函数: At24c02Read,从指定地址读取字符串数据
**------------------------------------------------------------------------------------------------------
** 参数: addr 地址 ，buff 数据指针 ，size 数据长度
** 返回: void
********************************************************************************************************/
void At24c02Read(unsigned char addr,unsigned char *buff,int size)
{
	I2cStart();
	I2cWriteByte(0xa0);I2cNoACK(); 
	I2cWriteByte(addr);I2cNoACK(); 	
	I2cStart();
	I2cWriteByte(0xa1);I2cNoACK(); 	
	while(size--)
	{
		I2cReadByte(buff++); 
		if(size)I2cACK();
		else I2cNoACK();
	}
	I2cStop();
}

/********************************************************************************************************
**                            End Of File
********************************************************************************************************/
