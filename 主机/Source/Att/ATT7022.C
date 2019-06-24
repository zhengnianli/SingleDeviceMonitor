/*******************************************************************************************************
**------------------------------------------------------------------------------------------------------
** 文件	: 	ATT7022.c		
** 版本	: 	v1.0 
**------------------------------------------------------------------------------------------------------
** 描述	:	
		ATT7022驱动程序
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "config.h"  
#include "ATT7022.h"  
// 加权递推平均滤波法
#define FILTER_N 12
int coe[FILTER_N] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};    // 加权系数表
int sum_coe = 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 + 11 + 12; // 加权系数和
double filter_buf[FILTER_N + 1];

typedef struct ReviseTypeDef
{
	long	MAC;
	long   	HFConst;           	        //高频脉冲常数 	  		0x000080 
	double  UKrms[3];             		//A相电压校正	  		1
	double 	IKrms[3];             		//A相电流校正	  		1
	long   	Uos;						//电压offset校正	 	0
	long   	Ios;						//电流offset校正	 	0
	long   	Pgain[3];        	        //有功功率校正 	 		0x000000
	long   	Qgain;        	        //无功功率校正  		0x000000	
	long 	Phsreg[3];					//相位矫正	 			0x000000
}ReviseTypeDef;
//-------------------------------------------------------------------------------
// ATT7022 SPI宏定义
#define  	ATTCS     					PAout(4)						// SPI片选使能引脚
#define		ATT_SPI_Init()				SPI1_Init()										
#define		ATT_SPI_ReadWrite(dat)		SPI1_ReadWrite(dat)						

//-------------------------------------------------------------------------------
// ATT7022校准数据
static Energy_t	s_tEmt = {0};
//static F_Energy_t Fs_tEmt = {0};  //存储滤波后的值
struct ReviseTypeDef	RTS ={0}; 
//-------------------------------------------------------------------------------
// ATT7022校准数据
static Revise_t	s_tRts ={0xDC,{1.51,1.51,1.67},{33.5,34.3,34.3}};  
//-------------------------------------------------------------------------------
//用电器相关定义     
char STA_ON[]="ON";
char STA_OFF[]="OFF";


/*******************************************************************************************************
** 函数: Filter:加权递推平均滤波法
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: filter_sum
********************************************************************************************************/
double Filter(double data)
{
	int i;
	double filter_sum = 0;
	filter_buf[FILTER_N] = data;
	for(i = 0; i < FILTER_N; i++)
	{
		filter_buf[i] = filter_buf[i + 1]; // 所有数据左移，低位仍掉
		filter_sum += filter_buf[i] * coe[i];
	}
	filter_sum /= sum_coe;
	
// 	printf("有功功率（滤波）%0.3fW\n", filter_sum-0.2);
	return filter_sum;
}
/*******************************************************************************************************
** 函数: ReadData_ATT7022,  读参数寄存器
**------------------------------------------------------------------------------------------------------
** 参数: addr:被读数据的地址(最高位为0)	
** 返回: temp:读出数据(24位) 
********************************************************************************************************/
unsigned long AttReadData(unsigned char addr)
{
	unsigned long temp=0;
	ATTCS=0;
	ATT_SPI_ReadWrite(addr&0x7f);
	TimDelayUs(2);						// SPI时钟频率过高，读数据时需要等待2us
	temp=(temp<<8)|ATT_SPI_ReadWrite(0);
	temp=(temp<<8)|ATT_SPI_ReadWrite(0);
	temp=(temp<<8)|ATT_SPI_ReadWrite(0);
	ATTCS=1;
	return temp;   							 		
}

/*******************************************************************************************************
** 函数: WriteData_ATT7022,  写入命令或校表数据
**------------------------------------------------------------------------------------------------------
** 参数: addr:被写数据的地址 dat 被写的数据
** 返回: void 
********************************************************************************************************/
void WriteData_ATT7022(unsigned char addr,unsigned long dat)
{	  
	ATTCS=0;
	ATT_SPI_ReadWrite(addr|0x80);
	ATT_SPI_ReadWrite(dat>>16);
	ATT_SPI_ReadWrite(dat>>8);
	ATT_SPI_ReadWrite(dat>>0);
	ATTCS=1;
}

/*******************************************************************************************************
** 函数: Att7022GpioInit,  ATT7022  GPIO初始化
**------------------------------------------------------------------------------------------------------
** 参数: NULL
** 返回: NULL
********************************************************************************************************/
void ATT7022GpioInit(void)	
{
	GPIO_InitTypeDef GPIO_InitStructure;	
	//------------------------------------------------------------------------------------------------
	// 配置IO和时钟	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB , ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 ;					// RESET复位
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 ;					// CS使能
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*******************************************************************************************************
** 函数: AttVarInit, ATT7022寄存器配置
**------------------------------------------------------------------------------------------------------
** 参数: NOLL
** 返回: NOLL
********************************************************************************************************/
void ATT7022VarInit(void)
{	
	WriteData_ATT7022( 0xC3,0x000000); 							// 清校表数据
	WriteData_ATT7022( 0xC9,0x00005A); 							// 打开校准数据写
	WriteData_ATT7022( 0x01,0xB97E);							// 模式配置寄存器
	WriteData_ATT7022( 0x03,0xF804);							// EMU 单元配置寄存器
	WriteData_ATT7022( 0x31,0x3427);							// 模拟模块使能寄存器
	WriteData_ATT7022( 0x6D,0xFF00); 							// Vrefgain 的补偿曲线系数
	WriteData_ATT7022( 0x6E,0x0DB8); 							// Vrefgain 的补偿曲线系数
	WriteData_ATT7022( 0x6F,0xD1DA); 							// Vrefgain 的补偿曲线系数
	WriteData_ATT7022( 0x16,0x0000);							// 无功相位校正寄存器
	WriteData_ATT7022( 0x1E,HFconst);							// HFConst高频脉冲参数校准
	WriteData_ATT7022( 0xC9,0x0001); 							// 关闭校准数据写
// 	printf("%x\n\n",(u32)AttReadData(0));						// 测试指令 打印 7122A0 芯片工作 SPI通信正常
}

/********************************************************************************************************
** 函数: Att7022Init
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: void
*******************************************************************************************************/
void Att7022Init(void)
{
	ATT_SPI_Init();
	ATT7022GpioInit();
	ATT7022VarInit();
//	printf("success\n");
}

/*******************************************************************************************************
** 函数: AttGetVolue 
**------------------------------------------------------------------------------------------------------
** 参数: addr 地址 
** 返回: 有符号数据
********************************************************************************************************/
long AttGetVolue( unsigned char Value)
{	
	long temp;
	temp=AttReadData(Value);
	if(AttReadData(0X2d)!=temp) return -1;
	if(temp&0x800000) temp = 0x1000000 - temp ;  
	return temp;
}

/*******************************************************************************************************
** 函数: AttGetUnVolue 
**------------------------------------------------------------------------------------------------------
** 参数: addr 地址 
** 返回: 无符号数据
********************************************************************************************************/
long AttGetUnVolue(  unsigned char Value)
{	
	long temp;
	temp=AttReadData(Value);
	if(AttReadData(0X2d)!=temp) return -1;
	return 	temp;
}

/*******************************************************************************************************
** 函数: AttGetAllVolue, 获取ATT7022UE内部电能计量值
**------------------------------------------------------------------------------------------------------
** 参数: Energy_t *dat
** 返回: NONE
********************************************************************************************************/
void AttGetAllVolue( void )
{	
	//----------------------------------------------------------------------------------------------- 
	// 单相用电器电能参数采集
	s_tEmt.dMU [ItemA]   = AttGetVolue(VOLTAGE+ItemA)/UKrmsA;				   		// A相电压
	s_tEmt.dMI [ItemA]   = AttGetVolue(CURRENT+ItemA)/IKrmsA;				  		// A相电流
	s_tEmt.dMP [ItemA]   = AttGetVolue(POWERS+ItemA)*KCoefficient;				// A相有功功率
	s_tEmt.dMQ [ItemA]   = AttGetVolue(QPOWERS+ItemA)*KCoefficient;		   		// A相无功功率
	s_tEmt.dMPF[ItemA]   = AttGetVolue(PF+ItemA)/8388608.0;				   		// A相功率因数
	s_tEmt.dwMPoEpt[ItemA] = AttGetUnVolue(EP+ItemA);								// A相有功电能
	s_tEmt.dwMQoEpt[ItemA] = AttGetUnVolue(EQ+ItemA);								// A相无功电能
	
	//----------------------------------------------------------------------------------------------- 
	// 总电能参数采集
	s_tEmt.dMF  = AttGetVolue(FREQUENCY)/8191.0;									// 总频率
	s_tEmt.dMP [ItemALL]   = AttGetVolue(POWERS+ItemALL)*KCoefficient*2;			// 总有功功率
	s_tEmt.dMQ [ItemALL]   = AttGetVolue(QPOWERS+ItemALL)*KCoefficient*2;			// 总无功功率
 	s_tEmt.dMPF[ItemALL] = AttGetVolue(PF+ItemALL)/8388608.0;						// 总功率因数
	s_tEmt.dwMPoEpt[ItemALL] = AttGetUnVolue(EP+ItemALL);							// 总有功电能
	s_tEmt.dwMQoEpt[ItemALL] = AttGetUnVolue(EQ+ItemALL);							// 总无功电能
}

/*******************************************************************************************************
** 函数: AttGetAllVolue, 获取ATT7022UE内部电能计量值
**------------------------------------------------------------------------------------------------------
** 参数: Energy_t *dat
** 返回: NONE
********************************************************************************************************/
void AttFilterAllVolue( void )
{
// 	Fs_tEmt.FdMU [ItemA]=Filter(s_tEmt.dMU [ItemA]);  //滤波后A相电压
// 	Fs_tEmt.FdMI [ItemA]=Filter(s_tEmt.dMI [ItemA]);  //滤波后A相电流
//	Fs_tEmt.FdMP [ItemA]=Filter(s_tEmt.dMP [ItemA]);  //滤波后A相有功功率
// 	Fs_tEmt.FdMQ [ItemA]=Filter(s_tEmt.dMQ [ItemA]);  //滤波后A相无功功率
}
/********************************************************************************************************
** 函数: Appear_data
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: void
*******************************************************************************************************/
void Appear_U_Data (void){	printf("电压：%0.1fV\n",s_tEmt.dMU[ItemA]);}
void Appear_I_Data (void){	printf("电流：%0.3fA\n",s_tEmt.dMI[ItemA]);}
void Appear_P_Data (void){	printf("有功功率：%0.1fW\n", s_tEmt.dMP[ItemA]-0.3);}
void Appear_Q_Data (void){	printf( "无功功率：%0.1fVar\n", s_tEmt.dMQ[ItemA]);}
void Appear_F_Data (void){	printf("频率：%0.1fHz\n",s_tEmt.dMF);}

/********************************************************************************************************
** 函数: PutAllData
**------------------------------------------------------------------------------------------------------
** 参数: void
** 返回: void
*******************************************************************************************************/
void PutAllData(void)
{
	printf("-----------------电源线参数-----------------------\n");
	Appear_U_Data();		// 串口显示 电压数据
	Appear_I_Data();		// 串口显示 电流数据
	Appear_P_Data();		// 串口显示 有功功率数据
	Appear_Q_Data();		// 串口显示 无功功率数据
	Appear_F_Data();    	// 串口显示 频率
	
	printf("\n\n");
}

/*******************************************************************************************************
** 函数: JudgeSta:判断用电器工作状态
**------------------------------------------------------------------------------------------------------
** 说明: 本函数可能在各用电器具体功率参数时，根据实际测得功率，可能产生多种组合，
**		 此时，结合 参数排除，比如电风扇一般是感性负载，如果测得某种组合满足，且包含
**		 电风扇，但读取的无功功率为零，可排除这种组合
** 参数: NOLL
** 返回: NOLL 
********************************************************************************************************/        
void JudgeSta(void)     
{
	int ElectricalSta[7]={0}; // 存储用电器状态（1或0）
	int len=7;
	int i=0,j=0;
	float p_sta[7];  //各个用电器工作状态对应的功率，要么为0要么为该用电器已知功率
	float p_sum;     //实测的总功率
	//LED 灯、节能灯、USB 充电器（带负载）、无线路由器、机顶盒、电风扇、热水壶 功率 单位W
	float power[7]={70,4.8,1.8,7.5,45,100,1800};  //已知的各用电器功率参数
	int times=0,timestmp=0;
	for(times=0;times<128;times++)
	{	 	
		timestmp=times;
		for(i=0;i<7;i++) 
		ElectricalSta[i]=0; 
		i=0;
		while(timestmp)
		{
			ElectricalSta[i]=timestmp%2;
			timestmp=timestmp/2;
			i++;
		}
		/* 二进制调试
		for(i=6;i>=0;i--)
		printf("%d ",a[i]);
		printf("\n");	
		*/
		for(i = 0; i < len; i ++)//遍历数组。
		{
			p_sta[i] = ElectricalSta[i]*power[i];  
		}
		for(i=0;i<7;i++) 
		{
			p_sum+=p_sta[i];
		}
		if(abs(p_sum-(float)PowerSum)<=0.2)  //进行匹配 假设匹配PowerSum
		{
			for( i=0;i<7;i++)
			{
				printf("%d\n",ElectricalSta[i]); 	 
			}
			p_sum=0;
			break;
		}
		p_sum=0;
	}

	// 往从机发送固定字符串格式的数据
	u3_printf("U:%f,I:%f,P:%f,Q:%f,UEA1:%1d,UEA2:%1d,UEA3:%1d,UEA4:%1d,UEA5:%1d,UEA6:%1d,UEA7:%1d\r\n",
			s_tEmt.dMU[ItemA],		// 电压数据
			s_tEmt.dMI[ItemA],		// 电流数据
			s_tEmt.dMP[ItemA]-0.3,	// 有功功率
			s_tEmt.dMQ[ItemA],		// 无功功率
			ElectricalSta[0],		// 1号用电器状态（0为关，1为开）
			ElectricalSta[1],		// 2号用电器状态（0为关，1为开）
			ElectricalSta[2],		// 3号用电器状态（0为关，1为开）
			ElectricalSta[3],		// 4号用电器状态（0为关，1为开）
			ElectricalSta[4],		// 5号用电器状态（0为关，1为开）
			ElectricalSta[5],		// 6号用电器状态（0为关，1为开）
			ElectricalSta[6]		// 7号用电器状态（0为关，1为开）
			);
}

 /********************************************************************************************************
 ** 函数: PutAllVolue,输出电能参数至串口
 **------------------------------------------------------------------------------------------------------
 ** 参数: NULL
 ** 返回: 成功 1 失败 0
 *******************************************************************************************************/
int PutAllVolue(void)
{
  	if(PutEmtTimer)	return FALSE;
  	PutEmtTimer=TIMER_SEC*5;
 	AttGetAllVolue();
 	PutAllData();    //打印到串口
 	return TRUE;
 }

/********************************************************************************************************
**                            End Of File
********************************************************************************************************/

