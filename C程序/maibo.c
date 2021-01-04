#include<reg52.h>
#define uchar unsigned char
#define uint  unsigned	int

unsigned int s=0;	  //下降沿累加中间计数变量
unsigned int maibo=0;	  //一分钟的脉搏频率
unsigned int num;	  //中间计数每50ms累加一次
unsigned char idata mc_num[6];	//用来做中值滤波算法的中间变量
unsigned char t=0;	   //中间变量 
sbit P32=P3^2;  
uchar Set_maiboL=50;
uchar Set_maiboH=100;
uchar flag=0;		 //工作状态标志位
uchar maobo_key=0;

bit biaozhi=1;	 //报警标志
sbit FMQ =P1^0;		//蜂鸣器控制引脚

uchar num1;
//按键扫描程序 设置上下限
void Key_Check()
{
  
	 if(!key4){
		while(!key4);
		flag++;
		FMQ=1;
		maobo_key=1;
		KEY_flag=0;
		biaozhi=1;	
	  if(flag==3){
			flag=0; 
			Lcd_ShowSring(4,7," ");
			    Sector_erase(0x2000);	      
			    Byte_program(0x2000,Set_maiboH);
				Sector_erase(0x2200);
				Byte_program(0x2200,Set_maiboL);
		} 
		if(flag==1)			  //上限
		{	
		    Lcd_ShowSring(4,7,"H"); 
		    biaozhi=0;			   //清零
		   	maibo=0; 
		}
 
		if(flag==2) 	   //下限
		{
		  Lcd_ShowSring(4,7,"L");

		}
		
	}
	if(flag==1){
		if(!key2&&maobo_key==1){
			//while(!key2);
			Delay1(100);  //软件消除按键抖动
			if(!key2&&maobo_key==1){
				
				Set_maiboH++;
				if(Set_maiboH>180){	//超过上限就赋值
					Set_maiboH=20;

				}
				biaozhi=1;	// 上限标志
			}
		}
		if(!key3&&maobo_key==1){
			Delay1(100); //软件消除按键抖动
			if(!key3&&maobo_key==1){
			 
 				 Set_maiboH--;
				if(Set_maiboH<20){	//低于下限就赋值
					Set_maiboH=180;
				}
					biaozhi=1;	// 上限标志
			
			}
		}	
		//Lcd_Display_3Date(2,6,Set_maiboH);
		  Lcd_Display33data(4,3,Set_maiboH);
	}
////////////////////////////////////////////////////////////////	
	 if(flag==2){
		if(!key2&&maobo_key==1){
			//while(!key2);
			Delay1(100);  //软件消除按键抖动
			if(!key2&&maobo_key==1){
				
				Set_maiboL++;
				if(Set_maiboL>180){	//超过上限就赋值
					Set_maiboL=20;

				}
				biaozhi=1;	// 下限标志
			}
		}
		if(!key3&&maobo_key==1){
			Delay1(100); //软件消除按键抖动
			if(!key3&&maobo_key==1){
 				 Set_maiboL--;
				if(Set_maiboL<20){	//低于下限就赋值
					Set_maiboL=180;
				}
				
				biaozhi=1;	// 下限标志
			}
		}
		//	Lcd_Display_3Date(2,6,Set_maiboL);
		    Lcd_Display33data(4,3,Set_maiboL);
	}


}

//中断初始化
void init()
  {	  

	//定时器设置
    TMOD=0x11; 		//设置定时器0，方式1:16位定时器	
	TH0=(65535-50000)/255;	  //定时器1高位装初值
	TL0=(65535-50000)%255;;	  //定时器1高位装初值
	ET0=1;   ////定时器0	  使能位
	TR0=1;

//	TH1=(65535-50000)/255;	  //定时器1高位装初值
//	TL1=(65535-50000)%255;;	  //定时器1高位装初值

//	ET1=1;	//定时器1	  使能位

//	TR1=0;

    IT0=1,//下降沿触发
	EX0=1;//外部中断0     使能位

	EA=1;//	总中断
 }
/////////////////////////////////////////////////////////////////
   //定时器0 
 void time0()interrupt 1   
 { 	
		 //每50ms进行一次中断
	 	TH0=(65535-50000)/256;	 //定时器0高位装初值
		TL0=(65535-50000)%256; //定时器0低位装初值
	 	num++; //每次中断num 累加

 }
   /*/定时器0 
 void time1()interrupt 3   
 { 	
		 //每50ms进行一次中断
	 	TH1=(65535-50000)/256;	 //定时器0高位装初值
		TL1=(65535-50000)%256; //定时器0低位装初值
	 	num1++; //每次中断num 累加
		if(num1==10){
		 num1=0;
		 FMQ=~FMQ;
		}

 }	*/
//外部中断0 下降沿计数
void int0() interrupt 0
{	 
		unsigned char temp,p,g;
		unsigned int ii=0;

		ii=0;
		if(P32==0) {

			//	Delay(1);
				if(P32==0) {
	
				s++;
			
				//第一个下降沿清楚定时器
				//打开定时器
				//等待第二次中断的时候 计算俩次中断所用的时间
				//
				if(s==1){
						TR0 = 1;
						//初始化计数器的值每50ms 进一次中断
					 	TH0=(65535-50000)/256;	 //定时器0高位装初值
						TL0=(65535-50000)%256; //定时器0低位装初�
						num =0;	
				} 
				if(s==2){
						
						s =0;
						TR0 = 0;
		
				//		TH0 =0;
				//		TL0 =0;	
					//	maibo	= 1/ (num *5/1000);
					//一次脉冲的宽度有 num  个 50ms  num*50ms 转化一下  num*50/1000 s	 
					//一秒钟就有  1/（ num*50/1000） 个脉冲数
					//一分钟内就有  60 * 1 /（ num*50 /1000）
					//化简得一分钟内的脉冲数为   1200/num 
						temp 	= 1200/ num ;	//将脉冲宽度的时间 （即俩次下降沿中断的时间） 对应的频率给中间变量
						if(  temp >30 && temp <120 )
					   {	
					   		mc_num[t] =temp ;
							t++;	 //一共有6个中间变量

						    if(t==6){ 
		
								t=0;
								// 中值滤波 
								//将6次采集到的脉冲的频率 从小到大的 排列 取中间值
								for(p=0;p<6;p++){
										   for(g=0;g<6-p;g++){
											   if(mc_num[g]>mc_num[g+1] )
												{   temp  = mc_num[g];
												   mc_num[g] =  mc_num[g+1];
												   mc_num[g+1] = temp ;
												}			                                                                                                            		
										   }
								 }	
		
///								maibo =	mc_num[5];	//取数组中的第个数作为一分钟的脉搏数
							maibo=mc_num[1]+mc_num[2]+mc_num[3] ;
					   			  maibo =maibo/3;  



					    }

							
					   }

					
				} 
			 }  
	   }
		
}

