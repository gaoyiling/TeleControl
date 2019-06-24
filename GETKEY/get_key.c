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


sbit ext_out = P3^3;   //ѧϰָʾ��


/****��������*********/
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




/*****��������***********/
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
********��Ƭ���˿�����************************************/
void channel_init()
{						  
    P1M1 = 0x00;	 //P1M1 0000 0000	
    P1M0 = 0xFF;	 //P1M0 1111 1111   P1.0-P1.7 �������01��ʾ�������
    P3M1 = 0x04;	//P3M1 	0000 0100	ǰ5λ01��ʾ�������,��4λ00��ʾ׼˫���
    P3M0 = 0xFC;	//P3M0	1111 1100	 �ʹ�ͳ8051һ��,11Ϊ����̬
//	P5M1 = 0x00;	//�������
//	P5M0 = 0xFF;
    P1 = 0x00;
    P3 &= 0x0F;		//0111 1111
}

/****�����ϵ���ȡE2PROOM�еĵ�ַ��Ϣ************/
void init_addr()
{
	mode = 0;
	n = 0;					 //��¼ѧϰ�ĸ���
	ADDR_0 = 0;				 //0x00 01 11 11
	ADDR_1 = 0;
	ADDR_2 = 0;
	ADDR_3 = 0;
	
	mode = IapReadByte(IAP_ADDRESS_4 + 0);	 //�����������洢ģʽ��Ϣ
	n = IapReadByte(IAP_ADDRESS_5 + 0);		//�����������洢������Ϣ		

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
******��������void get_addr(void)
******��  �ܣ��洢��ַ
******��  �룺
******��  ����
**************************************************/
void get_addr(void)
{
	unsigned long ADDR = 0;   //��ȡ��ַ
							//AD1501һλ����һλ����������λ��ʾһλ��
	if (data0 == data1)		 //ǰ��20λΪ��ַ����4λΪ����
	{		
		Date = (unsigned char)(data0 & 0x0000000f);	 //ȡ��4λ����
		ADDR = (data0 & 0x00fffff0) >> 4;	 //ȡ��20λ��ַ

	    if (!study)		//(!study)
		{
			if ((ADDR_0 == ADDR) || (ADDR_1 == ADDR) ||(ADDR_2 == ADDR) ||(ADDR_3 == ADDR)) //(ADDR_0 == ADDR)
			{		
				if (Date == 0x0C)  	//�����������������start��λ
				{
					start = 1;
					channel_1 = 1;	//�����̵���ͨ��
					self_lock();	//��alarm�Լ�1�����Ҷ�num����
					if (alarm > 1)
					{
	//					point_control();
	//					alarm = 0;
						channel_5 = 1;
					
					//	ring=1;	 //����̵���ͨ�磬��һ�ΰ�������ʱ��ʼ������
					}					//��2��3...�ΰ���ʱ�������죬�㶯����
				}
				
				if (start)		 //������־λ��λʱִ��
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
			IapEraseSector(IAP_ADDRESS_4 + 0);    //��������
			IapWriteByte(IAP_ADDRESS_4 + 0, Date); //�洢ģʽ��Ϣ

			if (n == 1)
			{
				IapEraseSector(IAP_ADDRESS_5);    //��������
				IapWriteByte(IAP_ADDRESS_5, 2);
				IapEraseSector(IAP_ADDRESS_0);    //��������				
				Delay10us();					//20λ��ַ��Ҫ3���ֽڴ��				
				IapWriteByte(IAP_ADDRESS_0 + 1, ADDR & 0xff);	
				IapWriteByte(IAP_ADDRESS_0 + 2, (ADDR >> 8) & 0xff);
				IapWriteByte(IAP_ADDRESS_0 + 3, (ADDR >> 16) & 0x0f);			
				Delay10us();
				init_addr();	          //��ADDR��ֵ���и���	
			}
		    else if (n == 2)
			{
				IapEraseSector(IAP_ADDRESS_5);    //��������
				IapWriteByte(IAP_ADDRESS_5, 3);
				IapEraseSector(IAP_ADDRESS_1);    //��������
				Delay10us();					//20λ��ַ��Ҫ3���ֽڴ��
				IapWriteByte(IAP_ADDRESS_1 + 1, ADDR & 0xff);	
				IapWriteByte(IAP_ADDRESS_1 + 2, (ADDR >> 8) & 0xff);
				IapWriteByte(IAP_ADDRESS_1 + 3, (ADDR >> 16) & 0x0f);			
				Delay10us();
				init_addr();	          //��ADDR��ֵ���и���
			}
			else if (n == 3)
			{
				IapEraseSector(IAP_ADDRESS_5);    //��������
				IapWriteByte(IAP_ADDRESS_5, 0);
				IapEraseSector(IAP_ADDRESS_2);    //��������								//0x00 0f ff ff
				Delay10us();					//20λ��ַ��Ҫ3���ֽڴ��
				IapWriteByte(IAP_ADDRESS_2 + 1, ADDR & 0xff);	
				IapWriteByte(IAP_ADDRESS_2 + 2, (ADDR >> 8) & 0xff);
				IapWriteByte(IAP_ADDRESS_2 + 3, (ADDR >> 16) & 0x0f);			
				Delay10us();
				init_addr();	          //��ADDR��ֵ���и���
			}			
			else
			{
				IapEraseSector(IAP_ADDRESS_5);    //��������
				IapWriteByte(IAP_ADDRESS_5, 1);
				IapEraseSector(IAP_ADDRESS_3);    //��������
				Delay10us();			//0x00 0f ff ff20λ��ַ��Ҫ3���ֽڴ��
				IapWriteByte(IAP_ADDRESS_3 + 1, ADDR & 0xff);	
				IapWriteByte(IAP_ADDRESS_3 + 2, (ADDR >> 8) & 0xff);
				IapWriteByte(IAP_ADDRESS_3 + 3, (ADDR >> 16) & 0x0f);			
				Delay10us();
				init_addr();	          //��ADDR��ֵ���и���										
			}
			study = 0;
			ext_out = 0;
		}
	}
	}		
}

/***************************************************
*******��������void self_lock()
*******��  �ܣ���·����
*******��  �룺
*******��  ����
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

	num = 0;	 //ֻҪ�������͵㶯��صģ�ÿ��һ�κ���������������Ҫ��Ҫ����
}				 //���¼�ʱ
/***************************************************
*******��������void inter_lock()
*******��  �ܣ���·����
*******��  �룺
*******��  ����
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
*******��������void point_control()
*******��  �ܣ���·�㶯
*******��  �룺
*******��  ����
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
*******��������void mixed_model_1()
*******��  �ܣ���·���-������������
*******��  �룺
*******��  ����
***************************************************/
void mixed_model_1()
{
	switch (Date)
	{	
	 	case 0x08:		 //ֹͣ�������������еļ̵����ϵ�           
			channel_init();	
			start = 0;
			alarm = 0;	
			break;
	 	case 0x04:					
			//channel_3 = 0;	
			self_lock();
			break;
//	 	case 0xf0:			//�������ڶ��ΰ���ʱ��������		
//			channel_3 = 1;	//����̵����ӵ���
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
*******��������void mixed_model_2()
*******��  �ܣ���·���-�������ҵ㶯
*******��  �룺
*******��  ����
***************************************************/
void mixed_model_2()
{
	switch (Date)
	{	
	 	case 0x08:		            //ֹͣ�������������еļ̵����ϵ�
			channel_init();	
			start = 0;				//������־λ����
			alarm = 0;				//����������������
			break;
	 	case 0x04:					//0000 1100	  0010
			point_control();
			break;
//	 	case 0xf0:					//0011 0000	  0100
//			channel_3 = 1;			//����̵����ӵ���
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
*******��������void mixed_model_3()
*******��  �ܣ���·���-���µ㶯��������(�����д���������)
*******��  �룺
*******��  ����
***************************************************/
void mixed_model_3()
{
	switch (Date)
	{	
	 	case 0x08:		            //ֹͣ�������������еļ̵����ϵ�
			channel_init();
			start = 0;			   //������־λ����
			alarm = 0;			   //����������������
			break;
	 	case 0x04:					//0000 1100	  0010
			point_control();
			break;
//	 	case 0xf0:					//0011 0000	  0100
//			channel_3 = 1;			//����̵����ӵ���
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
*******��������void mixed_model_4()
*******��  �ܣ���·���-���µ㶯�����һ���
*******��  �룺
*******��  ����
***************************************************/
void mixed_model_4()
{
	switch (Date)
	{	
	 	case 0x08:		            //ֹͣ�������������еļ̵����ϵ�
			channel_init();
			start = 0;
			alarm = 0;		
			break;
	 	case 0x04:					//0000 1100	  0010
			channel_3 = 0;	
			self_lock();
			break;
//	 	case 0xf0:					//0011 0000	  0100
//			channel_3 = 1;			//����̵����ӵ���
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
*******��������void mixed_model_5()
*******��  �ܣ���·���-���µ㶯����������
*******��  �룺
*******��  ����
***************************************************/
void mixed_model_5()
{
	switch (Date)
	{	
	 	case 0x08:		            //ֹͣ�������������еļ̵����ϵ�
			channel_init();
			start = 0;
			alarm = 0;		
			break;
	 	case 0x04:					//0000 1100	  0010
		point_control();
			break;
//	 	case 0xf0:					//0011 0000	  0100
//			channel_3 = 1;			//����̵����ӵ���
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
�����ʱ
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
** ��������: void IapIdle()
** ��������: �˳�IAP����
**********************************************************/
void IapIdle()
{
    IAP_CONTR = 0;                  //�ر�IAP����
    IAP_CMD = 0;                    //�������Ĵ����������ģʽ
    IAP_TRIG = 0;                   //��������Ĵ���
    IAP_ADDRH = 0x80;               //����ַ���õ���IAP����
    IAP_ADDRL = 0;
}

/**********************************************************
** ��������: unsigned char IapReadByte(unsigned int addr);
** ��������: ��EEPROM
** �䡡  ��: unsigned int addr  ��ַ
** �䡡  ��: unsigned char dat  ����
**********************************************************/
unsigned char IapReadByte(unsigned int addr)
{
    unsigned char dat;                       //���ݻ�����

    IAP_CONTR = ENABLE_IAP;         //ʹ��IAP
    IAP_CMD = CMD_READ;             //���� ISP/IAP/EEPROM Ϊ������
    IAP_ADDRL = addr;               //���� ISP/IAP/EEPROM ��λ��ַ
    IAP_ADDRH = addr >> 8;          //���� ISP/IAP/EEPROM ��λ��ַ
    IAP_TRIG = 0x5a;                //д��������(0x5a)
    IAP_TRIG = 0xa5;                //д��������(0xa5)
    _nop_();                        //�ȴ�ISP/IAP/EEPROM�������
    dat = IAP_DATA;                 //��ISP/IAP/EEPROM����
    IapIdle();                      //�ر�ISP/IAP/EEPROM����

    return dat;                     //���� Flash ����
}

/**********************************************************
** ��������: void IapWriteByte(unsigned int addr, unsigned char dat);
** ��������: дEEPROM����
** �䡡  ��: unsigned int addr  д��ַ
             unsigned char dat  д����
**********************************************************/
void IapWriteByte(unsigned int addr, unsigned char dat)
{
    IAP_CONTR = ENABLE_IAP;         //ʹ��IAP
    IAP_CMD = CMD_WRITE;            //���� ISP/IAP/EEPROM Ϊд����
    IAP_ADDRL = addr;               //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8;          //����IAP�ߵ�ַ
    IAP_DATA = dat;                 //дISP/IAP/EEPROM����
    IAP_TRIG = 0x5a;                //д��������(0x5a)
    IAP_TRIG = 0xa5;                //д��������(0xa5)
    _nop_();                        //�ȴ�ISP/IAP/EEPROM�������
    IapIdle();
}

/**********************************************************
** ��������: void IapEraseSector(unsigned int addr);
** ��������: ����EEPROM����
** �䡡  ��: unsigned int addr ������ַ
**********************************************************/
void IapEraseSector(unsigned int addr)
{
    IAP_CONTR = ENABLE_IAP;         //ʹ��IAP
    IAP_CMD = CMD_ERASE;            //���� ISP/IAP/EEPROM Ϊ��������
    IAP_ADDRL = addr;               //���� ISP/IAP/EEPROM ��λ��ַ
    IAP_ADDRH = addr >> 8;          //���� ISP/IAP/EEPROM ��λ��ַ
    IAP_TRIG = 0x5a;                //д��������(0x5a)
    IAP_TRIG = 0xa5;                //д��������(0xa5)
    _nop_();                        //�ȴ�ISP/IAP/EEPROM�������
//	_nop_();
    IapIdle();						// �ر� ISP/IAP/EEPROM ����
}














