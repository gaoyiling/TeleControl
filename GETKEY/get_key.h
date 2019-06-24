#ifndef __KEY_H__
#define __KEY_H__

#include <stc15.h>
#include<wireless.h>


//-----------------------------------------------

#define CMD_IDLE    0               //����ģʽ
#define CMD_READ    1               //IAP�ֽڶ�����
#define CMD_WRITE   2               //IAP�ֽ�д����
#define CMD_ERASE   3               //IAP������������

//#define ENABLE_IAP 0x80           //if SYSCLK<30MHz
//#define ENABLE_IAP 0x81           //if SYSCLK<24MHz
#define ENABLE_IAP  0x82            //if SYSCLK<20MHz
//#define ENABLE_IAP 0x83           //if SYSCLK<12MHz
//#define ENABLE_IAP 0x84           //if SYSCLK<6MHz
//#define ENABLE_IAP 0x85           //if SYSCLK<3MHz
//#define ENABLE_IAP 0x86           //if SYSCLK<2MHz
//#define ENABLE_IAP 0x87           //if SYSCLK<1MHz

//��д��ַ
#define IAP_ADDRESS_0 0x0000
#define IAP_ADDRESS_1 0x0200
#define IAP_ADDRESS_2 0x0400
#define IAP_ADDRESS_3 0x0600
#define IAP_ADDRESS_4 0x0800
#define IAP_ADDRESS_5 0x0A00

//define channels
sbit channel_1  = P1^1;		//ͣ0x08
sbit channel_2  = P1^4;		//�� 0X06
sbit channel_3  = P1^3;		//�� 0X02
sbit channel_4  = P1^5;		//�� X001
sbit channel_5  = P1^0;		//���� 0X0C
sbit channel_6  = P1^2;		//�� 0X04
sbit channel_7  = P3^6;		//�� 0X0E
sbit channel_8  = P3^7;		//�� 0X0A


//  //�ϰ���
//sbit channel_1  = P3^6;		//��ͣ
//sbit channel_2  = P1^4;		//��
//sbit channel_3  = P1^3;		//��
//sbit channel_4  = P1^5;		//��
//sbit channel_5  = P3^7;		//����
//sbit channel_6  = P1^2;		//��
//sbit channel_7  = P1^1;		//��
//sbit channel_8  = P1^0;		//��

extern unsigned long data0, data1;
extern unsigned char mode;
extern unsigned char num;
extern unsigned char Date;
extern bit study;
extern bit key;
extern bit start;
extern unsigned char alarm;
void IapEraseSector(unsigned int addr);
void init_addr();
void channel_init();
void get_addr(void);






#endif