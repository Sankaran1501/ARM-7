#include<lpc21xx.h>
#include"lcd_fun.h"
#include<string.h>
#define LED1 1<<20
#define LED 1<<18
void uart0_tx(unsigned char d);
void uart0_config(void);
unsigned char uart0_rx(void);
int main()
{
char i=0,s=0;
unsigned char a[20];
unsigned char c;
PINSEL0|=1<<16|1<<18;
PINSEL0|=0x00000005;
IODIR0|=LED|LED1;
IOSET0|=LED|LED1;
lcd_init();
uart0_config();

cmd(0xc0);
string("V25CE2S2");
delay(4000);

cmd(0x80);
lcd_scroll("IOT-ENABLED SMART HOME AUTOMATION USING ARM7 TECHNOLOGY");

cmd(0x01);
cmd(0x80);
string("WAITING FOR MESSAGE");
while(1)
{
 if(s==0)
 {
for(i=0;i<20;i++)
{  c =uart0_rx();
uart0_tx(c);
if(c=='\r'||c=='\n')
break;
   
a[i]=c;
}
a[i]='\0';
s=1;
}
if(s==1)
{
if(strcmp((char*)a,"LIGHT1ON")==0)
{
IOCLR0=LED;
cmd(0x01);
cmd(0xc0);
string(a);
}
else if(strcmp((char*)a,"LIGHT1OFF")==0)
{
IOSET0=LED;
cmd(0xc0);
string(a);

}
else if(strcmp((char*)a,"LIGHT2ON")==0)
{
IOCLR0=LED1;
cmd(0x01);
cmd(0x94);
string(a);
}
else if(strcmp((char*)a,"LIGHT2OFF")==0)
{
IOSET0=LED1;
cmd(0x94);
string(a);

}
s=0;
 }
/*while(((IOPIN0>>sw)&1)==1);

cmd(0x80);
string(a);
*/
}
}
void uart0_config(void)
{
U0LCR=0x83;
U0DLL=97;
U0DLM=0;
U0LCR=0x03;
}
void uart0_tx(unsigned char d)
{
U0THR=d;
while(((U0LSR>>5)&1)==0);
}
unsigned char uart0_rx(void)
{
while((U0LSR&1)==0);
return U0RBR;
}
