


#include "uart.h"
#include "wireless.h"
#include "get_key.h"


#define soft_rest() IAP_CONTR |= 0x20;	 //软件重启

bit flag; 
unsigned int button_cnt;
unsigned int button_cnt1;

sbit ext_out = P3^3;   //学习指示灯
sbit button = P5^5;	   //学习按键


void Delay1ms(unsigned char ms);
void led_flash_100ms(unsigned char flash_times);
  
/********************************************************/

void main(void)
{
    ext_out = 0;
	ext0_init();
	timer0_init();
   timer2_init();
	channel_init();
	init_addr();

//	IP = 0X20;	 		//IP中断寄存器设置
	EA = 1;
	while (1)
	{	 
	
	    if(key==1)
		   { 
		   get_addr();
		    EX0=1; 
		    data0 = 0;
         data1 = 0;
         key = 0 ;
			}



		if (!button)	   //按键按下
		{
			Delay1ms(20);
			if (!button)
			{
//				EA = 0;
				flag = 1;
				button_cnt++;
				if (button_cnt == 100)  //200*20=4S，长按
				{
//					button_cnt = 0;
//					EA = 0;
					IapEraseSector(IAP_ADDRESS_0);    //扇区擦除
					IapEraseSector(IAP_ADDRESS_1);    //扇区擦除
					IapEraseSector(IAP_ADDRESS_2);    //扇区擦除
					IapEraseSector(IAP_ADDRESS_3);    //扇区擦除
					IapEraseSector(IAP_ADDRESS_4);    //扇区擦除
					IapEraseSector(IAP_ADDRESS_5);    //扇区擦除
					led_flash_100ms(5);
					channel_init();
			    start = 0;
					alarm=0 ;
					init_addr();	  //更新E2PROOM
					ext_out = 0;
					study = 0;
					
//					EA = 1;
				}
				if (button_cnt < 100)				  //短按
				{					
					ext_out = 1;
          study = 1;	
				}
			}		
		}
		else
		{
			button_cnt = 0;
			if (flag)
			{
				Delay1ms(20);
				button_cnt1++;
				if (button_cnt1 > 200)	
				{
					ext_out = 0;
					button_cnt1 = 0;
					flag = 0;
					study = 0;

				}
			}			
		}	
	}		
}

void Delay1ms(unsigned char ms)     //@18.432MHz
{
    unsigned char i, j;
    while(ms--)
    {
//        WDT_CONTR |= 0x10;
        i = 18;
        j = 235;
        do
        {
            while (--j);
        } while (--i);
    }
}

void led_flash_100ms(unsigned char flash_times)
{
    while(flash_times--)
    {
        ext_out = 0;
        Delay1ms(100);
        ext_out = 1;
        Delay1ms(100);
    }
}



