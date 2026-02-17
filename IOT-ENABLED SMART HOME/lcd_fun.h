#include<lpc21xx.h>
#define data_pins (0xf)<<14
#define rs 1<<10
#define e 1<<11
void delay(int ms)
{
	T0PR=15000-1;
	T0TCR=0X00;
	T0TCR=0X01;
	while(T0TC<ms);
	T0TCR=0X03;
	T0TCR=0X01;
}
void lcd_init(void);
void cmd(unsigned char);
void data(unsigned char);
void floate(float);
void inti(int );
void lcd_string(char*);
void lcd_init(void)
{
	IODIR0 |= data_pins |rs |e;
	cmd(0x01);
	cmd(0x02);
	cmd(0x0c);
	cmd(0x28);
	//cmd(0x80);
}
void cmd(unsigned char cmdd)
{
	IOCLR0 |=data_pins;
	IOSET0 |=((cmdd>>4)& 0X0F)<<14;
	IOCLR0 |=rs;
	IOSET0 |=e;
	delay(2);
	IOCLR0|= e;

	IOCLR0 |=data_pins;
	IOSET0 =(cmdd&(0x0f))<<14;
	IOCLR0 =rs;
	IOSET0 =e;
	delay(2);
	IOCLR0= e;
}
void data(unsigned char datas)
{
	IOCLR0 =data_pins;
	IOSET0 =((datas>>4 )& 0X0F)<<14;
	IOSET0 =rs;
	IOSET0 =e;
	delay(2);
	IOCLR0= e;

	IOCLR0 =data_pins;
	IOSET0 =(datas&(0x0f))<<14;
	IOSET0 =rs;
	IOSET0 =e;
	delay(2);
	IOCLR0= e;
}
void floate(float f)
{
	int x=f;

	data((x%10)+48);
	x=f*100;
	data('.');
	data(((x/10)%10)+48);
	data((x%10)+48);
}
void string(unsigned char *s)
{
	while(*s)
	{
		data(*s++);
	}
}

void inti(int ch1)
{
	data(((ch1/100)%10)+48);
	
	data(((ch1%100)/10)+48);
	data((ch1%10)+48);
	
}
void lcd_string(char*s)
{
	while(*s)
	{
		data(*s++);
	}
}

void lcd_scroll(char *s) 



{

	int len = 0, i, j;

	while(s[len] != '\0') 

	len++;

	for (i= 0; i <= len; i++) 

	{

		cmd(0x80);

		for (j= 0; j < 16; j++) 

		{

			if ((i + j) < len)

			data(s[i + j]);

			else

			data(' ');

		}

		delay(300);

	}

}
