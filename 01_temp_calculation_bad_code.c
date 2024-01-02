#include "sfr62p.h"

void main(void)
{
long int x;
unsigned int Time;
    long long t;

	p7   = 0xff;
    pd7= 0xff;

	 adcon0=0x98;
    adcon1=0x22;
   adcon2=0x01;
    adst   =0x01;
 ta3mr= 0x00;
    ir_ta3ic= 0;
    trgsr =0x80;
   ta4mr =0x01;
   udf= 0x10;
   ir_ta4ic= 0;
    ta3 = 0xEA60;
    ta4= 0;
u1mr = 0x05;
  	u1c0 = 0x10;
u1brg = 0x26;
  	te_u1c1 = 1;
  	_asm("fset I");

while (1)
{
	while (adst == 0);
	x =ad0;
    ta3s=1;
    ta4s= 1;
	p7_0 = 0;
	x = (((x * 5000)/255)-500) / 10;
	p7_0 = 1;
	ta3s= 0;
	ta4s= 0;
	Time =60000- ta3;
    t =(ta4 *60000);
    t=t+Time;
    t=t*166;


	while(ti_u1c1 ==0);
    u1tb = 't';
	while(ti_u1c1 ==0);
	u1tb = 'e';
	while(ti_u1c1 ==0);
	u1tb = 'm';
	while(ti_u1c1 ==0);
	    u1tb = 'p';
	while(ti_u1c1 ==0);
		    u1tb = ':';
	while(ti_u1c1 ==0);
	    u1tb = ' ';
	if (x < 0)
	{
		x = -x;
	while(ti_u1c1 ==0);
		u1tb = '-';
	while(ti_u1c1 ==0);
	u1tb = ((x/100)%10)+0x30;
	while(ti_u1c1 ==0);
	u1tb = ((x/10)%10)+0x30;
	while(ti_u1c1 ==0);
	u1tb = (x%10)+0x30;
	}
if (x >= 0)
	{
	while(ti_u1c1 ==0);
	u1tb = ((x/100)%10)+0x30;
	while(ti_u1c1 ==0);
	u1tb = ((x/10)%10)+0x30;
	while(ti_u1c1 ==0);
	u1tb = (x%10)+0x30;
	}
	while(ti_u1c1 ==0);
				    u1tb = '\t';


	while(ti_u1c1 ==0);
    u1tb = 't';
	while(ti_u1c1 ==0);
	u1tb = 'i';
	while(ti_u1c1 ==0);
	u1tb = 'm';
	while(ti_u1c1 ==0);
	    u1tb = 'e';
	while(ti_u1c1 ==0);
		    u1tb = ':';
	while(ti_u1c1 ==0);
		    u1tb = ' ';
	while(ti_u1c1 ==0);
	u1tb = ((t/10000000)%10)+0x30;
	while(ti_u1c1 ==0);
	u1tb = ((t/1000000)%10)+0x30;
	while(ti_u1c1 ==0);
	u1tb = ((t/100000)%10)+0x30;
	while(ti_u1c1 ==0);
	u1tb = ((t/10000)%10)+0x30;
	while(ti_u1c1 ==0);
	u1tb = ((t/1000)%10)+0x30;
	while(ti_u1c1 ==0);
	u1tb = ((t/100)%10)+0x30;
	while(ti_u1c1 ==0);
	u1tb = ((t/10)%10)+0x30;
	while(ti_u1c1 ==0);
	u1tb = (t%10)+0x30;
	while(ti_u1c1 ==0);
	u1tb = ' ';
	while(ti_u1c1 ==0);
	u1tb = ' ';
	while(ti_u1c1 ==0);
	u1tb = 'n';
	while(ti_u1c1 ==0);
	u1tb = 's';
	while(ti_u1c1 ==0);
	u1tb = '\n';

	}
}
