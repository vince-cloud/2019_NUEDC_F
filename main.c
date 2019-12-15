/***************************************
2019�������ƴ���F��������
by sustech ���� wuyl017@163.com
****************************************/

#include "reg52.h"                    //����c52�ļĴ���
#include "lcd.h"                      //����LCD1602�Ľӿںͺ���

typedef unsigned int u16;	  //���������ͽ�����������
typedef unsigned char u8;

sbit startkey = P3^3;       //�����ð���
sbit remenkey =P3^2;        //�洢�ð���
sbit trigger =P3^6;         //�͵�ƽ���������
sbit resever =P3^7;         //�ߵ�ƽ�ź�����˿�
sbit bee=P1^3;              //�������˿�

u8 order;                   //���ݴ������
u8 page;                    //ҳ��
u16 time;										//����ʱ��
u16 timerefren[10];         //ҳ����Ӧ������ʱ�䣬���ο�������ʱ��
u8 refresh;                 //�Ƿ�Ҫˢ����ʾ
u8 star;                    //�Ƿ�ʼ����

//lcd1602�ֶδ���
u8 title1[]=" beginning...   "; //δ��ʼʱ��Ĭ����ʾ
u8 title2[]="the number is:  "; //���������ʾ
//u8 title3[]="memory spilled  ";//�ο�����ʱ���ڴ����ʱ��ʾ
u8 title4[]="remember now..  "; //��¼�ο�ʱ��ʱ��ʾ
u8 title6[]="shourt circuit! "; //��·ʱ��ʾ

/***********���ݴ�����**************
����ȡ����ʱ���봢��Ĳο�ʱ�����Ƚ�
�Ӷ���֪��ǰҳ��
*************************************/
void DataDeal()
{
	u8 j;                //���ڱȽϼ���
	page=0;              //ҳ������
	for(j=0;j<10;j++)    //j��������������ֵ
	{
		if(time>timerefren[j]) //����ҳ��Խ�࣬����ԽС�����ҳ�ʵ�ʶ������������ο�ֵ֮��
		{
			if(j==0)  page=1;    //ҳ��Ϊ1����������г�
			else                 //ʵ�ʶ������ĸ��ο�ֵ�Ĳ�ֵСҳ��������Ӧֵ
			{
				if(time-timerefren[j]>timerefren[j-1]-time) page=j;
				else page=j+1;
			}
		break;
		}
	}
	time=0;               //ʵ��ʱ���������
}


void TimerInit()        //��ʱ��0�ĳ�ʼ��
{
	TMOD|=0X01;          //ѡ��Ϊ��ʱ��0ģʽ��������ʽ1������TR1��������
	TH0=0X00;	           //����ʱ������
	TL0=0X00;	
	ET0=1;               //�򿪶�ʱ��1�ж�����
	EA=1;                //�����ж�
	TR0=0;               //�رն�ʱ������ʱ��Ӧ���ڵ͵�ƽ���巢��������			
}


void Int0Init()       //�ж���0�ĳ�ʼ��
{
	IT0=1;              //�����س�����ʽ���½��أ�
	EX0=1;              //��INT0���ж�����	
	EA=1;               //�����ж�	
}

void Int1Init()       //�ж���1�ĳ�ʼ��
{
	IT1=1;              //�����س�����ʽ���½��أ�
	EX1=1;              //��INT1���ж�����	
	EA=1;               //�����ж�	
}



void delay(u16 i)     //��ʱ������i=1ʱ��Լ��ʱ10us
{
	while(i--);
}

/******************���ݶ�ȡ����*********************
���Ƶ͵�ƽ��������������ͬʱ������ʱ��
ֱ���ߵ�ƽ�����������ת�����ߵ�ƽ����ʱ�䳬��Ԥ��ֵ
���ж�Ϊ��·
���������ظ�50����ƽ��ֵ�Լ������
***************************************************/
void Read()
{
	u8 i;    //�ظ���������
	u16 j;   //���ڷ�����ʹ��
	u16 timetemple; //����ʱ����ʱ������
	unsigned long tatoltime; //ʱ���ܺͣ����ڼ���ƽ��ֵ
  u8 stop;                 //���ڶ�·���
	tatoltime=0;             //ʱ���ܺ�����
	for(i=0;i<50;i++)        //i������Ϊ�ظ����� 
	{
		trigger=1;             //��ƽ�������ø�λ����ʱ1ms���õ�λ
		delay(100);
		trigger=0;             //�õ�λʱ�򿪼�ʱ����֮�󱣳�5ms�͵�ƽʱ�䣬��ʱ����Գ��ڸߵ�ƽά��ʱ��
		EX1=0;                 //emmmm��������
		TR0=1;                 //�򿪶�ʱ��
		delay(500);
		trigger=1;             //��ƽ��������ظ���λ
		timetemple=0;          //��ʱʱ�䴢��������
		while(resever)         //�����ƽ���ָ�λʱһֱѭ��
		{	
			if(TH0>0Xdf)         //��ʱ������Ԥ��ʱ��
			{
				TR0=0;             //��ʱ���ر�
				EX1=1;             //�ж�������
				EX0=1;  
				stop=1;            //��·��ʶ��1
				LcdWriteCom(0x80); //�˴����Ż�
				for(j=0;j<16;j++)
				{			
					LcdWriteData(title6[j]);
				}
				for(j=0; j<10000; j++)
				{
					bee=~bee;
					delay(10); 
				}
				break;              //����ѭ��
			}
			
		}	
		if(TH0!=0x00||TL0!=0x00)//��ʱ����Ϊ�㼴��Ϊ����Ч���ݣ����ж��������ࣩ
		{
			TR0=0;
			timetemple=TH0<<8;
			timetemple|=TL0;
			TH0=0X00;
			TL0=0X00;
		}
		tatoltime+=timetemple;  //������ʱ��
		if(stop==1)             //�ж��Ƿ��·
		{
			stop=0;               //��·��ʶ��0
			star=0;               //������ʶ��0
			break;                //����ѭ��
		}
	}
	time=tatoltime/50;        //����ƽ��ʱ��
	refresh=1;                //ˢ�±�ʶ��1
}

/**********************************************
��ʾ����1
��¼�ο�ʱ��ʱʹ��
��ʾ��ǰ�ο�ҳ������Ӧ����
**********************************************/
void DisPlay()
{	
	u8 i;             //lcd1602����
	u16 t;            //��¼ʱ���Լ���������ʱ

	LcdWriteCom(0x80);
	for(i=0;i<16;i++)
	{
		LcdWriteData(title4[i]);//��ʾ����¼�ο�ʱ�䡱
	}
	LcdWriteCom(0x8e);        //��ʾ��ǰҳ��
	LcdWriteData('0'+order/10);
	LcdWriteData('0'+order%10);
		
	t=time;                    //��ʱ��ת����ʱʱ��λ��ʹ��ʱ���漰ȫ�ֱ����ı仯              
	if(t>9999)                 //�˴�bug�����۸�λ�Ƿ�������0��������ʾ�����ڴ˴����Ǳ�project�ص㣬�������޸�
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
	
	EX1=1;       //�ж���1
	EX0=1;
	refresh=0;   //ˢ�±�ʶ��0
	time=0;      //ʱ������
	star=0;      //���Ա�ʶ����
	
	for(t=0; t<5000; t++)//������
	{
		bee=~bee;
		delay(10); 
	}
}


/**********************************************
��ʾ����2
����ҳ��ʱʹ��
��ʾ��ǰҳ��
**********************************************/
void DisPlay2()
{	
	u16 j;             //��������ʱʹ��
	u8 p;              //lcd1602 ����ʹ���Լ���ʱҳ��ʹ��
	LcdWriteCom(0x80);
	for(p=0;p<16;p++)
	{
		LcdWriteData(title2[p]); //��ʾ��the page is����
	}
	p=page;            //��ҳ��pageת����ʱҳ��p��ʹ��ʱ���漰ȫ�ֱ����ı仯
	page=0;            //ҳ�����㣨�˴��������ࣩ
	if(p>9)            //ҳ����ʾ
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
	LcdWriteCom(0xc3);//�˴�bug�����û�����пո���ʾ��������������
	LcdWriteData(' ');
	LcdWriteCom(0xc4);
	LcdWriteData(' ');
	LcdWriteCom(0xc5);
	LcdWriteData(' ');
	EX1=1;            //�ж���1
	EX0=1;
	refresh=0;        //ˢ�±�ʶ��0
	star=0;           //���Ա�ʶ��0
	
	//������
	for(j=0; j<5000; j++)
	{
		bee=~bee;
		delay(10); 
	}
	
}

/********************************************
������
���ֳ�ʼ��
���ݲ��Ա�ʶ��ˢ�±�ʶ���к�������
********************************************/
void main()
{
	u8 i;       //������
	Int1Init(); //�ж���1��ʼ��
	Int0Init(); //�ж���0��ʼ��
	TimerInit();//��ʱ����ʼ��
	LcdInit();  //lcd1602��ʼ��
	trigger=1;  //��������ʼ����1
	order=0;    //ҳ���洢����ʼ����0
	
	LcdWriteCom(0x80); //��ʾ��beganning��������
	for(i=0;i<16;i++)  
	{
		LcdWriteData(title1[i]);
	}
	
	while(1)
	{
		if(star==1) //���Ա�ʶ��Ϊ1������ģʽ
		{
			Read();   //���ݶ�ȡ
			DataDeal();//���ݴ���
		}
		
		if(star==2)//���Ա�ʶΪ2����¼ģʽ����ʾʱ�������ͬʱ���ݴ���ο�������
		{
			Read(); //���ݶ�ȡ
			timerefren[order]=time;//���ݴ洢
			order++;     //�洢λ�ü�1
			if(order==15)//�ж��Ƿ�洢���
			{
				LcdWriteCom(0x80);
				for(i=0;i<16;i++)
				{
					LcdWriteData(title4[i]);//��ʾ�洢���
				}
				order=0;  //�洢λ��0
			}
		}
		//Display�������������Լ���·��ʾ����ò�ƿ��Ժϲ���һ��
		if(refresh==1) //�Ƿ�ˢ����ʾ
		{
			if(star==1)  //��ʾҳ��
			{
				DisPlay2();
			}
			if(star==2)  //��ʾ����ģʽ
			{
				DisPlay();
			}
		}	
	}
}


void Int1()	interrupt 2		//�ⲿ�ж�1���жϺ�����ʵ��Ϊ��������
{
	delay(1000);//��ʱ����
	if(startkey==0)
	{	
		star=1; //���Ա�ʶ����1
		EX1=0;  //�жϹر�
		EX0=0;
	}
}


void Int0()	interrupt 0		//�ⲿ�ж�0���жϺ�����ʵ��Ϊ�洢����
{
	delay(1000);	 //��ʱ����
	if(remenkey==0)
	{
		star=2; //���Ա�ʶ����1
		EX0=0;  //�жϹر�
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