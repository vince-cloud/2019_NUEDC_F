/***************************************
2019年电子设计大赛F题程序代码
by sustech 柚子 wuyl017@163.com
****************************************/

#include "reg52.h"                    //申明c52的寄存器
#include "lcd.h"                      //定义LCD1602的接口和函数

typedef unsigned int u16;	  //对数据类型进行声明定义
typedef unsigned char u8;

sbit startkey = P3^3;       //测试用按键
sbit remenkey =P3^2;        //存储用按键
sbit trigger =P3^6;         //低电平触发输出端
sbit resever =P3^7;         //高电平信号输入端口
sbit bee=P1^3;              //蜂鸣器端口

u8 order;                   //数据储存序号
u8 page;                    //页数
u16 time;										//脉冲时间
u16 timerefren[10];         //页数对应的脉冲时间，即参考用脉冲时间
u8 refresh;                 //是否要刷新显示
u8 star;                    //是否开始测试

//lcd1602字段储存
u8 title1[]=" beginning...   "; //未开始时的默认显示
u8 title2[]="the number is:  "; //测量结果显示
//u8 title3[]="memory spilled  ";//参考脉冲时间内存溢出时显示
u8 title4[]="remember now..  "; //记录参考时间时显示
u8 title6[]="shourt circuit! "; //短路时显示

/***********数据处理函数**************
将读取到的时间与储存的参考时间作比较
从而得知当前页数
*************************************/
void DataDeal()
{
	u8 j;                //用于比较计数
	page=0;              //页数置零
	for(j=0;j<10;j++)    //j的上限是最大测量值
	{
		if(time>timerefren[j]) //由于页数越多，读数越小，先找出实际读数在哪两个参考值之间
		{
			if(j==0)  page=1;    //页数为1的情况单独列出
			else                 //实际读数与哪个参考值的差值小页数就是响应值
			{
				if(time-timerefren[j]>timerefren[j-1]-time) page=j;
				else page=j+1;
			}
		break;
		}
	}
	time=0;               //实际时间读数置零
}


void TimerInit()        //定时器0的初始化
{
	TMOD|=0X01;          //选择为定时器0模式，工作方式1，仅用TR1打开启动。
	TH0=0X00;	           //给定时器置零
	TL0=0X00;	
	ET0=1;               //打开定时器1中断允许
	EA=1;                //打开总中断
	TR0=0;               //关闭定时器，定时器应该在低电平脉冲发出后启动			
}


void Int0Init()       //中断器0的初始化
{
	IT0=1;              //跳变沿出发方式（下降沿）
	EX0=1;              //打开INT0的中断允许。	
	EA=1;               //打开总中断	
}

void Int1Init()       //中断器1的初始化
{
	IT1=1;              //跳变沿出发方式（下降沿）
	EX1=1;              //打开INT1的中断允许。	
	EA=1;               //打开总中断	
}



void delay(u16 i)     //延时函数，i=1时大约延时10us
{
	while(i--);
}

/******************数据读取函数*********************
控制低电平触发脉冲的输出，同时启动定时器
直到高电平脉冲的输入跳转，若高电平持续时间超过预定值
则判断为短路
上述过程重复50遍求平均值以减少误差
***************************************************/
void Read()
{
	u8 i;    //重复次数计数
	u16 j;   //用于蜂鸣器使用
	u16 timetemple; //脉冲时间临时储存器
	unsigned long tatoltime; //时间总和，用于计算平均值
  u8 stop;                 //用于短路标记
	tatoltime=0;             //时间总和置零
	for(i=0;i<50;i++)        //i的上限为重复次数 
	{
		trigger=1;             //电平触发先置高位，延时1ms后置低位
		delay(100);
		trigger=0;             //置低位时打开计时器，之后保持5ms低电平时间，此时间可以长于高电平维持时间
		EX1=0;                 //emmmm怀疑冗余
		TR0=1;                 //打开定时器
		delay(500);
		trigger=1;             //电平触发输出回复高位
		timetemple=0;          //临时时间储存器置零
		while(resever)         //输入电平保持高位时一直循环
		{	
			if(TH0>0Xdf)         //定时器超过预定时间
			{
				TR0=0;             //定时器关闭
				EX1=1;             //中断允许开启
				EX0=1;  
				stop=1;            //短路标识置1
				LcdWriteCom(0x80); //此处可优化
				for(j=0;j<16;j++)
				{			
					LcdWriteData(title6[j]);
				}
				for(j=0; j<10000; j++)
				{
					bee=~bee;
					delay(10); 
				}
				break;              //跳出循环
			}
			
		}	
		if(TH0!=0x00||TL0!=0x00)//计时器不为零即认为是有效数据（此判断疑似冗余）
		{
			TR0=0;
			timetemple=TH0<<8;
			timetemple|=TL0;
			TH0=0X00;
			TL0=0X00;
		}
		tatoltime+=timetemple;  //计入总时间
		if(stop==1)             //判断是否短路
		{
			stop=0;               //短路标识置0
			star=0;               //测量标识置0
			break;                //跳出循环
		}
	}
	time=tatoltime/50;        //计算平均时间
	refresh=1;                //刷新标识置1
}

/**********************************************
显示函数1
记录参考时间时使用
显示当前参考页数即响应读数
**********************************************/
void DisPlay()
{	
	u8 i;             //lcd1602计数
	u16 t;            //记录时间以及蜂鸣器计时

	LcdWriteCom(0x80);
	for(i=0;i<16;i++)
	{
		LcdWriteData(title4[i]);//显示“记录参考时间”
	}
	LcdWriteCom(0x8e);        //显示当前页数
	LcdWriteData('0'+order/10);
	LcdWriteData('0'+order%10);
		
	t=time;                    //将时间转入临时时间位，使用时不涉及全局变量的变化              
	if(t>9999)                 //此处bug：无论高位是否有数，0都不会显示，由于此处不是本project重点，不给予修复
	{
		LcdWriteCom(0xc1);
		LcdWriteData('0'+t/10000);
		t=t%10000;
	}
	else
	{
		LcdWriteCom(0xc1);
		LcdWriteData(' ');
	}
	if(t>999)
	{
		LcdWriteCom(0xc2);
		LcdWriteData('0'+t/1000);
		t=t%1000;
	}
	else
	{
		LcdWriteCom(0xc2);
		LcdWriteData(' ');
	}
	if(t>99)
	{
		LcdWriteCom(0xc3);
		LcdWriteData('0'+t/100);
		t=t%100;
	}
	else
	{
		LcdWriteCom(0xc3);
		LcdWriteData(' ');
	}
	if(t>9)
	{
		LcdWriteCom(0xc4);
		LcdWriteData('0'+t/10);
		t=t%10;
	}
	else
	{
		LcdWriteCom(0xc4);
		LcdWriteData(' ');
	}
	LcdWriteCom(0xc5);
	LcdWriteData('0'+t);
	
	EX1=1;       //中断置1
	EX0=1;
	refresh=0;   //刷新标识置0
	time=0;      //时间置零
	star=0;      //测试标识置零
	
	for(t=0; t<5000; t++)//蜂鸣器
	{
		bee=~bee;
		delay(10); 
	}
}


/**********************************************
显示函数2
测试页数时使用
显示当前页数
**********************************************/
void DisPlay2()
{	
	u16 j;             //蜂鸣器计时使用
	u8 p;              //lcd1602 计数使用以及临时页数使用
	LcdWriteCom(0x80);
	for(p=0;p<16;p++)
	{
		LcdWriteData(title2[p]); //显示“the page is：”
	}
	p=page;            //将页数page转入临时页数p，使用时不涉及全局变量的变化
	page=0;            //页数置零（此处疑似冗余）
	if(p>9)            //页数显示
	{
		LcdWriteCom(0xc1);
		LcdWriteData('0'+p/10);
		p=p%10;
	}
	else
	{
		LcdWriteCom(0xc1);
		LcdWriteData(' ');
	}
	LcdWriteCom(0xc2);
	LcdWriteData('0'+p);
	LcdWriteCom(0xc3);//此处bug，如果没有下列空格显示，则有乱码现象
	LcdWriteData(' ');
	LcdWriteCom(0xc4);
	LcdWriteData(' ');
	LcdWriteCom(0xc5);
	LcdWriteData(' ');
	EX1=1;            //中断置1
	EX0=1;
	refresh=0;        //刷新标识置0
	star=0;           //测试标识置0
	
	//蜂鸣器
	for(j=0; j<5000; j++)
	{
		bee=~bee;
		delay(10); 
	}
	
}

/********************************************
主方法
各种初始化
根据测试标识，刷新标识进行函数调用
********************************************/
void main()
{
	u8 i;       //计数用
	Int1Init(); //中断器1初始化
	Int0Init(); //中断器0初始化
	TimerInit();//定时器初始化
	LcdInit();  //lcd1602初始化
	trigger=1;  //输出脉冲初始化置1
	order=0;    //页数存储器初始化置0
	
	LcdWriteCom(0x80); //显示“beganning。。。”
	for(i=0;i<16;i++)  
	{
		LcdWriteData(title1[i]);
	}
	
	while(1)
	{
		if(star==1) //测试标识符为1，测试模式
		{
			Read();   //数据读取
			DataDeal();//数据处理
		}
		
		if(star==2)//测试标识为2，记录模式，显示时间读数，同时数据存入参考数据中
		{
			Read(); //数据读取
			timerefren[order]=time;//数据存储
			order++;     //存储位置加1
			if(order==15)//判断是否存储溢出
			{
				LcdWriteCom(0x80);
				for(i=0;i<16;i++)
				{
					LcdWriteData(title4[i]);//显示存储溢出
				}
				order=0;  //存储位置0
			}
		}
		//Display的两个函数，以及短路提示函数貌似可以合并成一个
		if(refresh==1) //是否刷新显示
		{
			if(star==1)  //显示页数
			{
				DisPlay2();
			}
			if(star==2)  //显示测量模式
			{
				DisPlay();
			}
		}	
	}
}


void Int1()	interrupt 2		//外部中断1的中断函数，实际为测量按键
{
	delay(1000);//延时消抖
	if(startkey==0)
	{	
		star=1; //测试标识符置1
		EX1=0;  //中断关闭
		EX0=0;
	}
}


void Int0()	interrupt 0		//外部中断0的中断函数，实际为存储按键
{
	delay(1000);	 //延时消抖
	if(remenkey==0)
	{
		star=2; //测试标识符置1
		EX0=0;  //中断关闭
		EX1=0;
	}
}

/*void Timer1() interrupt 1
{
	u16 i;
	LcdWriteCom(0x80);
	for(i=0;i<16;i++)
	{
		LcdWriteData(title6[i]);
	}
	

	EX1=1;
	EX0=1;
	TR0=0;
	star=0;
	
		for(i=0; i<10000; i++)
		{
			bee=~bee;
			delay(10); 
		}
}
*/