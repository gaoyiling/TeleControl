#include "get_key.h"
#include "wireless.h"

//typedef unsigned char BYTE;
//typedef unsigned int WORD;


//sbit channel_1  = P5^4;
//sbit channel_2  = P5^5;
//sbit channel_3  = P1^5;
//sbit channel_4  = P1^4;
//sbit channel_5  = P1^3;
//sbit channel_6  = P1^2;


sbit ext_out = P3^3;   //学习指示灯


/****变量定义*********/
bit study;
bit start;
bit key;
unsigned char alarm;
unsigned char mode;
unsigned char n;
unsigned char num;
unsigned char Date;
unsigned long ADDR_0, ADDR_1, ADDR_2, ADDR_3;
unsigned long data0, data1;




/*****函数声明***********/
void self_lock();
void inter_lock();
void point_control();
void mixed_model_1();
void mixed_model_2();
void mixed_model_3();
void mixed_model_4();
void mixed_model_5();
void get_addr(void);
void Delay10us();

void IapIdle();
unsigned char IapReadByte(unsigned int addr);
void IapWriteByte(unsigned int addr, unsigned char dat);


/**********************************************************
********单片机端口设置************************************/
void channel_init()
{						  
    P1M1 = 0x00;	 //P1M1 0000 0000	
    P1M0 = 0xFF;	 //P1M0 1111 1111   P1.0-P1.7 上下组合01表示推挽输出
    P3M1 = 0x04;	//P3M1 	0000 0100	前5位01表示推挽输出,后4位00表示准双向口
    P3M0 = 0xFC;	//P3M0	1111 1100	 和传统8051一样,11为高阻态
//	P5M1 = 0x00;	//推挽输出
//	P5M0 = 0xFF;
    P1 = 0x00;
    P3 &= 0x0F;		//0111 1111
}

/****重新上电后读取E2PROOM中的地址信息************/
void init_addr()
{
	mode = 0;
	n = 0;					 //记录学习的个数
	ADDR_0 = 0;				 //0x00 01 11 11
	ADDR_1 = 0;
	ADDR_2 = 0;
	ADDR_3 = 0;
	
	mode = IapReadByte(IAP_ADDRESS_4 + 0);	 //第五扇区：存储模式信息
	n = IapReadByte(IAP_ADDRESS_5 + 0);		//第六扇区：存储个数信息		

	ADDR_0 |= IapReadByte(IAP_ADDRESS_0 + 3);
	ADDR_0 = ADDR_0 << 8;
	ADDR_0 |= IapReadByte(IAP_ADDRESS_0 + 2);
	ADDR_0 = ADDR_0 << 8;
    ADDR_0 |= IapReadByte(IAP_ADDRESS_0 + 1);

	ADDR_1 |= IapReadByte(IAP_ADDRESS_1 + 3);
	ADDR_1 = ADDR_1 << 8;
	ADDR_1 |= IapReadByte(IAP_ADDRESS_1 + 2);
	ADDR_1 = ADDR_1 << 8;
    ADDR_1 |= IapReadByte(IAP_ADDRESS_1 + 1);

	ADDR_2 |= IapReadByte(IAP_ADDRESS_2 + 3);
	ADDR_2 = ADDR_2 << 8;
	ADDR_2 |= IapReadByte(IAP_ADDRESS_2 + 2);
	ADDR_2 = ADDR_2 << 8;
    ADDR_2 |= IapReadByte(IAP_ADDRESS_2 + 1);

	ADDR_3 |= IapReadByte(IAP_ADDRESS_3 + 3);
	ADDR_3 = ADDR_3 << 8;
	ADDR_3 |= IapReadByte(IAP_ADDRESS_3 + 2);
	ADDR_3 = ADDR_3 << 8;
    ADDR_3 |= IapReadByte(IAP_ADDRESS_3 + 1);
}

/**************************************************
******函数名：void get_addr(void)
******功  能：存储地址
******输  入：
******输  出：
**************************************************/
void get_addr(void)
{
	unsigned long ADDR = 0;   //读取地址
							//AD1501一位就是一位，不再是两位表示一位了
	if (data0 == data1)		 //前而20位为地址，后4位为数据
	{		
		Date = (unsigned char)(data0 & 0x0000000f);	 //取出4位数据
		ADDR = (data0 & 0x00fffff0) >> 4;	 //取出20位地址

	    if (!study)		//(!study)
		{
			if ((ADDR_0 == ADDR) || (ADDR_1 == ADDR) ||(ADDR_2 == ADDR) ||(ADDR_3 == ADDR)) //(ADDR_0 == ADDR)
			{		
				if (Date == 0x0C)  	//如果按下启动键，则start置位
				{
					start = 1;
					channel_1 = 1;	//启动继电器通电
					self_lock();	//让alarm自加1，并且对num清零
					if (alarm > 1)
					{
	//					point_control();
	//					alarm = 0;
						channel_5 = 1;
					
					//	ring=1;	 //警铃继电器通电，第一次按启动键时开始启动，
					}					//第2，3...次按下时，警铃响，点动控制
				}
				
				if (start)		 //启动标志位置位时执行
				{
					switch (mode)
					{	
					 	case 0x04:		            //0000 0011   0001
							mixed_model_1();		
							break;
					 	case 0x02:					//0000 1100	  0010	  
							mixed_model_2();
							break;
					 	case 0x06:					//0011 0000	  0100
							mixed_model_3();
							break;
						case 0x01:					//0011 0000	  0100
							mixed_model_4();
							break;
						case 0x0E:
						   	mixed_model_5();
						default:
			                break;
					}	
				}						
			}			
		}
		else 
		{
			if(Date==0x04||Date==0x02||Date==0x01||Date==0x06||Date==0x0e)
			{
			IapEraseSector(IAP_ADDRESS_4 + 0);    //扇区擦除
			IapWriteByte(IAP_ADDRESS_4 + 0, Date); //存储模式信息

			if (n == 1)
			{
				IapEraseSector(IAP_ADDRESS_5);    //扇区擦除
				IapWriteByte(IAP_ADDRESS_5, 2);
				IapEraseSector(IAP_ADDRESS_0);    //扇区擦除				
				Delay10us();					//20位地址需要3个字节存放				
				IapWriteByte(IAP_ADDRESS_0 + 1, ADDR & 0xff);	
				IapWriteByte(IAP_ADDRESS_0 + 2, (ADDR >> 8) & 0xff);
				IapWriteByte(IAP_ADDRESS_0 + 3, (ADDR >> 16) & 0x0f);			
				Delay10us();
				init_addr();	          //对ADDR的值进行更新	
			}
		    else if (n == 2)
			{
				IapEraseSector(IAP_ADDRESS_5);    //扇区擦除
				IapWriteByte(IAP_ADDRESS_5, 3);
				IapEraseSector(IAP_ADDRESS_1);    //扇区擦除
				Delay10us();					//20位地址需要3个字节存放
				IapWriteByte(IAP_ADDRESS_1 + 1, ADDR & 0xff);	
				IapWriteByte(IAP_ADDRESS_1 + 2, (ADDR >> 8) & 0xff);
				IapWriteByte(IAP_ADDRESS_1 + 3, (ADDR >> 16) & 0x0f);			
				Delay10us();
				init_addr();	          //对ADDR的值进行更新
			}
			else if (n == 3)
			{
				IapEraseSector(IAP_ADDRESS_5);    //扇区擦除
				IapWriteByte(IAP_ADDRESS_5, 0);
				IapEraseSector(IAP_ADDRESS_2);    //扇区擦除								//0x00 0f ff ff
				Delay10us();					//20位地址需要3个字节存放
				IapWriteByte(IAP_ADDRESS_2 + 1, ADDR & 0xff);	
				IapWriteByte(IAP_ADDRESS_2 + 2, (ADDR >> 8) & 0xff);
				IapWriteByte(IAP_ADDRESS_2 + 3, (ADDR >> 16) & 0x0f);			
				Delay10us();
				init_addr();	          //对ADDR的值进行更新
			}			
			else
			{
				IapEraseSector(IAP_ADDRESS_5);    //扇区擦除
				IapWriteByte(IAP_ADDRESS_5, 1);
				IapEraseSector(IAP_ADDRESS_3);    //扇区擦除
				Delay10us();			//0x00 0f ff ff20位地址需要3个字节存放
				IapWriteByte(IAP_ADDRESS_3 + 1, ADDR & 0xff);	
				IapWriteByte(IAP_ADDRESS_3 + 2, (ADDR >> 8) & 0xff);
				IapWriteByte(IAP_ADDRESS_3 + 3, (ADDR >> 16) & 0x0f);			
				Delay10us();
				init_addr();	          //对ADDR的值进行更新										
			}
			study = 0;
			ext_out = 0;
		}
	}
	}		
}

/***************************************************
*******函数名：void self_lock()
*******功  能：电路自锁
*******输  入：
*******输  出：
***************************************************/
void self_lock()
{
//	uart_send_byte(2);
	if (num > 22)
	{
//		IE2 &= ~0x04;
		switch (Date)
		{	
//		 	case 0xc0:		            //0000 0011   0001
////				Delay1ms(20);
//				channel_1 = !channel_1;		
//				break;
		 	case 0x04:					//0000 1100	  0010	
//			    Delay1ms(20);
				channel_6 = !channel_6;
				break;
		 	case 0x0C:					//0011 0000	  0100
				alarm++;
				break;
		 	case 0x02:					//1100 0000	  1000
//				Delay1ms(10);
				channel_3 = !channel_3;
				break;
			case 0x0A:					//1100 0000	  1000
				channel_8 = !channel_8;
				break;
			case 0x06:					//1100 0000	  1000
				channel_2 = !channel_2;
				break;
			case 0x0E:					//1100 0000	  1000
				channel_7 = !channel_7;
				break;
			case 0x01:					//1100 0000	  1000
				channel_4 = !channel_4;
				break;
			default:
                break;			
//			IE2 &= ~0x04;			
	 	}			
	}

	num = 0;	 //只要和自锁和点动相关的，每进一次函数不管满不满足要求都要清零
}				 //重新计时
/***************************************************
*******函数名：void inter_lock()
*******功  能：电路互锁
*******输  入：
*******输  出：
***************************************************/
/*
void inter_lock()
{
	switch (Date)
	{	
	 	case 0xc0:		            //0000 0011   0001
			channel_init();
			channel_1 = 1;		
			break;
	 	case 0x30:					//0000 1100	  0010	
			channel_init();  
			channel_2 = 1;
			break;
	 	case 0xf0:					//0011 0000	  0100
			channel_init();
			channel_3 = 1;
			break;
	 	case 0x0c:					//1100 0000	  1000
			channel_init();
			channel_4 = 1;
			break;
		case 0xcc:		            //0000 0011   0001
			channel_init();
			channel_5 = 1;		
			break;
	 	case 0x3C:					//0000 1100	  0010	
			channel_init();  
			channel_6 = 1;
			break;
	 	case 0xfc:					//0011 0000	  0100
			channel_init();
			channel_7 = 1;
			break;
	 	case 0x03:					//1100 0000	  1000
			channel_init();
			channel_8 = 1;
			break;
		default:
	        break;
		
	//			IE2 &= ~0x04;			
	}			
}
*/
/***************************************************
*******函数名：void point_control()
*******功  能：电路点动
*******输  入：
*******输  出：
***************************************************/
void point_control()
{
	if (num > 20)
	{
//		IE2 &= ~0x04;
		switch (Date)
		{	
//		 	case 0xc0:		            //0000 0011   0001
//				channel_1 = 1;		
//				break;
		 	case 0x04:					//0000 1100	  0010	  
				channel_6 = 1;
				break;
		 	case 0x0C:					//0011 0000	  0100
				channel_5 = 1;
				break;
		 	case 0x02:					//1100 0000	  1000
				channel_3 = 1;
				break;
			case 0x0A:		            //0000 0011   0001
				channel_8 = 1;		
				break;
		 	case 0x06:					//0000 1100	  0010	  
				channel_2 = 1;
				break;
		 	case 0x0E:					//0011 0000	  0100
				channel_7 = 1;
				break;
		 	case 0x01:					//1100 0000	  1000
				channel_4 = 1;
				break;
			default:
                break;			
//			IE2 &= ~0x04;			
	 	}			
	}
	num = 0;
}

/***************************************************
*******函数名：void mixed_model_1()
*******功  能：电路混合-上下左右自锁
*******输  入：
*******输  出：
***************************************************/
void mixed_model_1()
{
	switch (Date)
	{	
	 	case 0x08:		 //停止按键，按下所有的继电器断电           
			channel_init();	
			start = 0;
			alarm = 0;	
			break;
	 	case 0x04:					
			//channel_3 = 0;	
			self_lock();
			break;
//	 	case 0xf0:			//启动键第二次按下时，电铃响		
//			channel_3 = 1;	//这里继电器接电铃
//			break;
	 	case 0x02:					
			//channel_6 = 0;
			self_lock();
			break;
	 	case 0x0A:						
			self_lock();
			break;
	 	case 0x06:						
			//channel_4 = 0;
			self_lock();
			break;
	 	case 0x0E:					//1100 0000	  1000
			self_lock();
			break;
		case 0x01:					//1100 0000	  1000
			//channel_2 = 0;
			self_lock();
			break;
		default:
	        break;
		
	//			IE2 &= ~0x04;			
	}			
}



/***************************************************
*******函数名：void mixed_model_2()
*******功  能：电路混合-上下左右点动
*******输  入：
*******输  出：
***************************************************/
void mixed_model_2()
{
	switch (Date)
	{	
	 	case 0x08:		            //停止按键，按下所有的继电器断电
			channel_init();	
			start = 0;				//启动标志位清零
			alarm = 0;				//启动按键次数清零
			break;
	 	case 0x04:					//0000 1100	  0010
			point_control();
			break;
//	 	case 0xf0:					//0011 0000	  0100
//			channel_3 = 1;			//这里继电器接电铃
//			break;
	 	case 0x02:					//1100 0000	  1000
			point_control();
			break;
	 	case 0x0A:					//0000 1100	  0010	
			self_lock();
			break;
	 	case 0x06:					//0000 1100	  0010	
			point_control();
			break;
	 	case 0x0E:					//1100 0000	  1000
			self_lock();
			break;
		case 0x01:					//1100 0000	  1000
			point_control();
			break;
		default:
	        break;
		
	//			IE2 &= ~0x04;			
	}			
}


/***************************************************
*******函数名：void mixed_model_3()
*******功  能：电路混合-上下点动左右自锁(左右中带互锁功能)
*******输  入：
*******输  出：
***************************************************/
void mixed_model_3()
{
	switch (Date)
	{	
	 	case 0x08:		            //停止按键，按下所有的继电器断电
			channel_init();
			start = 0;			   //启动标志位清零
			alarm = 0;			   //启动按键次数清零
			break;
	 	case 0x04:					//0000 1100	  0010
			point_control();
			break;
//	 	case 0xf0:					//0011 0000	  0100
//			channel_3 = 1;			//这里继电器接电铃
//			break;
	 	case 0x02:					//1100 0000	  1000
			point_control();
			break;
	 	case 0x0A:					//0000 1100	  0010	
			self_lock();
			break;
	 	case 0x06:					//0000 1100	  0010	
			channel_4 = 0;
			self_lock();
			break;
	 	case 0x0E:					//1100 0000	  1000
			self_lock();
			break;
		case 0x01:					//1100 0000	  1000
			channel_2 = 0;
			self_lock();
			break;
		default:
	        break;
	//			IE2 &= ~0x04;			
	}			
}


/***************************************************
*******函数名：void mixed_model_4()
*******功  能：电路混合-上下点动锁左右互锁
*******输  入：
*******输  出：
***************************************************/
void mixed_model_4()
{
	switch (Date)
	{	
	 	case 0x08:		            //停止按键，按下所有的继电器断电
			channel_init();
			start = 0;
			alarm = 0;		
			break;
	 	case 0x04:					//0000 1100	  0010
			channel_3 = 0;	
			self_lock();
			break;
//	 	case 0xf0:					//0011 0000	  0100
//			channel_3 = 1;			//这里继电器接电铃
//			break;
	 	case 0x02:					//1100 0000	  1000
			channel_6 = 0;
			self_lock();
			break;
	 	case 0x0A:					//0000 1100	  0010	
			self_lock();
			break;
	 	case 0x06:
			channel_4 = 0;					//0000 1100	  0010	
			self_lock();
			break;
	 	case 0x0E:					//1100 0000	  1000
			self_lock();
			break;
		case 0x01:
			channel_2 = 0;					//1100 0000	  1000
			self_lock();
			break;
		default:
	        break;
	//			IE2 &= ~0x04;			
	}			
}

 /***************************************************
*******函数名：void mixed_model_5()
*******功  能：电路混合-上下点动锁左右自锁
*******输  入：
*******输  出：
***************************************************/
void mixed_model_5()
{
	switch (Date)
	{	
	 	case 0x08:		            //停止按键，按下所有的继电器断电
			channel_init();
			start = 0;
			alarm = 0;		
			break;
	 	case 0x04:					//0000 1100	  0010
		point_control();
			break;
//	 	case 0xf0:					//0011 0000	  0100
//			channel_3 = 1;			//这里继电器接电铃
//			break;
	 	case 0x02:					//1100 0000	  1000
		point_control();
			break;
	 	case 0x0A:					//0000 1100	  0010	
			self_lock();
			break;
	 	case 0x06:					//0000 1100	  0010	
			self_lock();
			break;
	 	case 0x0E:					//1100 0000	  1000
			self_lock();
			break;
		case 0x01:					//1100 0000	  1000
			self_lock();
			break;
		default:
	        break;
	//			IE2 &= ~0x04;			
	}			
}
/*----------------------------
软件延时
----------------------------*/

void Delay10us()		//@18.432MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 43;
	while (--i);
}
/**********************************************************
** 函数名称: void IapIdle()
** 功能描述: 退出IAP功能
**********************************************************/
void IapIdle()
{
    IAP_CONTR = 0;                  //关闭IAP功能
    IAP_CMD = 0;                    //清除命令寄存器进入待机模式
    IAP_TRIG = 0;                   //清除触发寄存器
    IAP_ADDRH = 0x80;               //将地址设置到非IAP区域
    IAP_ADDRL = 0;
}

/**********************************************************
** 函数名称: unsigned char IapReadByte(unsigned int addr);
** 功能描述: 读EEPROM
** 输　  入: unsigned int addr  地址
** 输　  出: unsigned char dat  数据
**********************************************************/
unsigned char IapReadByte(unsigned int addr)
{
    unsigned char dat;                       //数据缓冲区

    IAP_CONTR = ENABLE_IAP;         //使能IAP
    IAP_CMD = CMD_READ;             //设置 ISP/IAP/EEPROM 为读命令
    IAP_ADDRL = addr;               //设置 ISP/IAP/EEPROM 低位地址
    IAP_ADDRH = addr >> 8;          //设置 ISP/IAP/EEPROM 高位地址
    IAP_TRIG = 0x5a;                //写触发命令(0x5a)
    IAP_TRIG = 0xa5;                //写触发命令(0xa5)
    _nop_();                        //等待ISP/IAP/EEPROM操作完成
    dat = IAP_DATA;                 //读ISP/IAP/EEPROM数据
    IapIdle();                      //关闭ISP/IAP/EEPROM过程

    return dat;                     //返回 Flash 数据
}

/**********************************************************
** 函数名称: void IapWriteByte(unsigned int addr, unsigned char dat);
** 功能描述: 写EEPROM扇区
** 输　  入: unsigned int addr  写地址
             unsigned char dat  写数据
**********************************************************/
void IapWriteByte(unsigned int addr, unsigned char dat)
{
    IAP_CONTR = ENABLE_IAP;         //使能IAP
    IAP_CMD = CMD_WRITE;            //设置 ISP/IAP/EEPROM 为写命令
    IAP_ADDRL = addr;               //设置IAP低地址
    IAP_ADDRH = addr >> 8;          //设置IAP高地址
    IAP_DATA = dat;                 //写ISP/IAP/EEPROM数据
    IAP_TRIG = 0x5a;                //写触发命令(0x5a)
    IAP_TRIG = 0xa5;                //写触发命令(0xa5)
    _nop_();                        //等待ISP/IAP/EEPROM操作完成
    IapIdle();
}

/**********************************************************
** 函数名称: void IapEraseSector(unsigned int addr);
** 功能描述: 擦除EEPROM扇区
** 输　  入: unsigned int addr 擦除地址
**********************************************************/
void IapEraseSector(unsigned int addr)
{
    IAP_CONTR = ENABLE_IAP;         //使能IAP
    IAP_CMD = CMD_ERASE;            //设置 ISP/IAP/EEPROM 为擦除命令
    IAP_ADDRL = addr;               //设置 ISP/IAP/EEPROM 低位地址
    IAP_ADDRH = addr >> 8;          //设置 ISP/IAP/EEPROM 高位地址
    IAP_TRIG = 0x5a;                //写触发命令(0x5a)
    IAP_TRIG = 0xa5;                //写触发命令(0xa5)
    _nop_();                        //等待ISP/IAP/EEPROM操作完成
//	_nop_();
    IapIdle();						// 关闭 ISP/IAP/EEPROM 过程
}














