/**
 * @file       main.c
 * @brief      [MID001]-[01_temp_calculation]
 * @details    main program file.
 * @details    CPU GROUP = 62P
 * @copyright  -
 * @author     -
 * @version    00.01
 * @date       2019-01-22
 */

/**
 * Include file
 */
#include "sfr62p.h"

/**
 * Data type definition refer to MISRA
 */
typedef unsigned char        u1;
typedef unsigned short       u2;
typedef unsigned long        u4;
typedef unsigned long long   u8;
typedef signed char          s1;
typedef signed short         s2;
typedef signed long          s4;
typedef signed long long     s8;
typedef float                f4;
typedef double               f8;
typedef unsigned char        BOOL;

/**
 * Data definition
 */
/* char 1 byte */
#define S1_MIN              (-128)
#define S1_MAX              (127)
#define U1_MIN              (0)
#define U1_MAX              (255)
/* short 1 byte */
#define S2_MIN              (-32768)
#define S2_MAX              (32767)
#define U2_MIN              (0)
#define U2_MAX              (65535)
/* Analog input channel */
#define ADC_CH0             (0)
/* LED0 */
#define LED0_ON             (p7_0 = 0)
#define LED0_OFF            (p7_0 = 1)
/* ftoa data definition */
#define MAX_PRECISION       (10)

/**
 * Global Variable Definition
 */
static const double rounders[MAX_PRECISION + 1] = {
	0.5,          /* 0 */
	0.05,         /* 1 */
	0.005,        /* 2 */
	0.0005,       /* 3 */
	0.00005,      /* 4 */
	0.000005,     /* 5 */
	0.0000005,    /* 6 */
	0.00000005,   /* 7 */
	0.000000005,  /* 8 */
	0.0000000005, /* 9 */
	0.00000000005 /* 10 */
};

/**
 * fucntion prototype declaration
 */
static void init_hw(void);
static void init_adc(void);
static u2 adc_read(u1 ch);
static void init_timers(void);
static void time_tick(void);
static void time_tock(void);
static u8 read_time(void);
static void init_uart(void);
static void uart_putc(const char s1_c);
static void uart_puts(const char* s1_s);
static f8 read_temp(u4 u4_val);
char*  ftoa(f8 f8_f, char* buf, s2 s2_precision);

/**
 * Main function
 */
void main(void)
{
	/*
	 * Local Variable Definition
	 */
	const char program_text[] = "01 Temperature Calculation ver 00.01\n";
	char  temp_buf[10]        = { 0 };
	char  time_buf[10]        = { 0 };
	u4    u4_adc_val          = 0;
	f8    f8_temp_val         = 0.0;
	f8    f8_pro_time         = 0.0;

	init_hw();      /* Initialize hardware peripheral */
	init_adc();     /* Initialize ADC mode.           */
	init_timers();  /* Initialize Timer mode.         */
	init_uart();    /* Initialize UART mode.          */
	_asm("fset I"); /* Enable global interrupt        */

	/*
	 * Printing program information
	 * to serial port
	 */
	uart_puts(program_text);

	while (1)
	{
		/*
		 * Reading analog value
		 */
		u4_adc_val = adc_read(ADC_CH0);

		/*
		 * Start checking processing time
		 */
		LED0_ON;
		time_tick();

		/*
		 * Reading temperature data
		 */
		f8_temp_val = read_temp(u4_adc_val);

		/*
		 * Stop checking process time
		 */
		time_tock();
		LED0_OFF;

		/*
		 * Reading process time
		 * convert time from nanasecond to millisecond
		 */
		f8_pro_time = (double)read_time();
		f8_pro_time = f8_pro_time / 1000000;

		/*
		 * Convert double to string.
		 * precision = 2 (0.12)
		 */
		ftoa(f8_temp_val, temp_buf, 2);
		ftoa(f8_pro_time, time_buf, 2);

		/* Printing data to serial port */
		uart_puts("Teperature : ");
		uart_puts(temp_buf);
		uart_putc('\t');
		uart_puts("Time stamp : ");
		uart_puts(time_buf);;
		uart_puts("ms");
		uart_putc('\n');
	}
}

/**
 * @fn              static void init_hw(void)
 * @fid             [FID001]-[init_hw]
 * @fnbrf           Initialize Hardware
 * @param[in]       -
 * @param[in,out]   -
 * @retval          -
 * @warning         -
 * @remark          -
 */
static void init_hw(void)
{
	pd7 = 0xFF; /* Set port 7 direction as output */
	p7  = 0xFF; /* Set port 7 outpt off           */
}

/**
 * @fn              static void init_adc(void)
 * @fid             [FID002]-[init_adc]
 * @fnbrf           Initialize A/D
 * @param[in]       -
 * @param[in,out]   -
 * @retval          -
 * @warning         -
 * @remark          -
 */
static void init_adc(void)
{
	/*
	 * Objective: Repeat Sweep Mode 0
	 */

	adcon0 = 0x98;
	/* setting ADC control register adcon0 mode register                           */
	/* 10011000                                                                    */
	/* |||||+++---- ADC channel                                                    */
	/* |||++------- ADC Mode "11" is Repeat sweep mode 0 and mode 1                */
	/* ||+--------- Trigger ('0' is software trigger / '1' is extenal pin trigger) */
	/* |+---------- ADC start bit       */
	/* +----------- Freq divider selection ('1' is freq/2 / '0' is freq/4)         */
	/* Be careful !! , to use Freq divider, the bit ck of adcon1 have to be set    */

	adcon1 = 0x22;
	/* setting ADC control register adcon1 mode register                           */
	/* 00100010                                      		                       */
	/* ||||||++---- Sweep mode selection AN0 to AN5   		                       */
	/* |||||+------ '0' for repeat sweep mode 0 only 		                       */
	/* ||||+------- Resolution ('0' : 8 bit / '1' : 10 bit)                        */
	/* |||+-------- Vref connected '1'                                             */
	/* ||+--------  Freq divider select ('0' not use freq div / '1' use freq div)  */
	/* ++----------                                                                */

	adcon2 = 0x01;
	/* setting ADC control register adcon2 mode register                           */
	/* 00000001                                                                    */
	/* ||||||++---- timer  mode                                                    */
	/* |||||+------ pulse is not output                                            */
	/* |||++------- gate function not availble                                     */
	/* ||+--------- timer mode                                                     */
	/* ++----------  count source is f32                                           */

	/*
	 * May not need this interupt control
	 * because of repeat mode
	 */
	/* adic = 0x01; */

	/*
	 * ADC conversion start
	 */
	adst = 0x01;
}

/**
 * @fn              static u2 adc_read(u1 ch)
 * @fid             [FID003]-[adc_read]
 * @fnbrf           Initialize A/D
 * @param[in]       ch ; u1 ; adc channel
 * @param[in,out]   -
 * @retval          u2_vla ; u2 ; adc data
 * @warning         -
 * @remark          -
 */
static u2 adc_read(u1 ch)
{
	u2 u2_val = 0;
	switch (ch)
	{
		case 0:
			while (adst == 0); /* waiting conversion complete */
			u2_val = ad0;
			break;
		default:
			break;
	}
	return u2_val;
}

/**
 * @fn              static void init_timers(void)
 * @fid             [FID004]-[init_timers]
 * @fnbrf           Initialize timer for tick/tock
 * @param[in]       -
 * @param[in,out]   -
 * @retval          -
 * @warning         -
 * @remark          -
 */
static void init_timers(void)
{
	ta3mr    = 0x00;   /* ta3 clock source not divided where f = 6 mhz , set as timer mode */
	ir_ta3ic = 0;      /* Clear interupt for ta3 (set to 0)                                */
	trgsr    = 0x80;   /* trigger register select to ta3                                   */
	ta4mr    = 0x01;   /* set ta4 as event mode to monitor overflow of ta3                 */
	udf      = 0x10;   /* set ta4 to count up                                              */
	ir_ta4ic = 0;      /* Clear interupt for ta4 (set to 0)                                */
	ta3      = 0xEA60; /* Hex value equivalent to 60000, ta3 start at 60000                */
	ta4      = 0;      /* set ta4 to start at 0                                            */
}

/**
 * @fn              static void time_tick(void)
 * @fid             [FID005]-[time_tick]
 * @fnbrf           Start timer counter
 * @param[in]       -
 * @param[in,out]   -
 * @retval          -
 * @warning         -
 * @remark          -
 */
static void time_tick(void)
{
	ta3s = 1; /* Timer a3 start count */
	ta4s = 1; /* Timer a4 start count */
}

/**
 * @fn              static void time_tock(void)
 * @fid             [FID006]-[time_tock]
 * @fnbrf           Stop timer counter
 * @param[in]       -
 * @param[in,out]   -
 * @retval          -
 * @warning         -
 * @remark          -
 */
static void time_tock(void)
{
	ta3s = 0; /* Timer a3 stop count */
	ta4s = 0; /* Timer a4 stop count */
}

/**
 * @fn              static u8 read_time(void)
 * @fid             [FID007]-[read_time]
 * @fnbrf           Read timer count
 * @param[in]       -
 * @param[in,out]   -
 * @retval          -
 * @warning         -
 * @remark          -
 */
static u8 read_time(void)
{
	u4 u4_time_old = 0;
	u8 u8_time_new = 0;
	u8 u8_time     = 0;

	/*
	 * Revert time value so that with increasing time,
	 * time can be displayed in ascending order
	 */
	u4_time_old = 60000 - ta3;
	/* u8_time_new = (ta4 * 60000); */
	u8_time_new = u8_time_new + u4_time_old;

	/*
	 * Calcuate time in nanosecond.
	 * 0.166us = 166 ns , 0.000166 ms =  0.166 us
	 */
	u8_time = u8_time_new * 166;

	/* clear data */
	u4_time_old = 0;
	u8_time_new = 0;

	return u8_time;
}

/**
 * @fn              static void init_uart(void)
 * @fid             [FID008]-[init_uart]
 * @fnbrf           Initialize timer for tick/tock
 * @param[in]       -
 * @param[in,out]   -
 * @retval          -
 * @warning         -
 * @remark          -
 */
static void init_uart(void)
{
	/*
	 * Configure Uart0 for buad : 9600
	 * Buad rate : 9600
	 * Data bits : 8
	 * Stop bits : 1
	 * Parity    : NONE
	 */
	u1mr    = 0x05; /* Set mode register      */
	u1c0    = 0x10; /* Set control register   */
	u1brg   = 0x26; /* Set bit rate generator */
	te_u1c1 = 0x01; /* Enable transmission    */
}

/**
 * @fn              static void uart_putc(const char s1_c)
 * @fid             [FID009]-[uart_putc]
 * @fnbrf           UART character transmit.
 * @param[in]       s1_c ; const char ; character
 * @param[in,out]   -
 * @retval          -
 * @warning         -
 * @remark          -
 */
static void uart_putc(const char s1_c)
{
	while (ti_u1c1 == 0);
	u1tb = s1_c;
}

/**
 * @fn              static void uart_puts(const char* s1_s)
 * @fid             [FID010]-[uart_puts]
 * @fnbrf           UART string transmit.
 * @param[in]       s1_s ; const char ; string
 * @param[in,out]   -
 * @retval          -
 * @warning         Side effect with s1 (signed char) 
 * @remark          -
 */
static void uart_puts(const char* s1_s)
{
	while (*s1_s != '\0')
	{
		while (ti_u1c1 == 0);
		u1tb = *s1_s;
		s1_s++;
	}
}

/**
 * @fn              static f8 read_temp(u4 u4_val)
 * @fid             [FID011]-[read_temp]
 * @fnbrf           Read temperature sensor.
 * @param[in]       u4_val ; u4 ; adc data (mV)
 * @param[in,out]   -
 * @retval          f8_temp ; f8 ; temperature data
 * @warning         -
 * @remark          -
 */
static f8 read_temp(u4 u4_val)
{
	f8 f8_temp = 0.0;

	/*
	 * This formula converts the number 0-255
	 * from the ADC into 0-5000mV (= 5V)
	 */
	f8_temp = ((u4_val * 5000) / 255);

	/*
	 * This formula convertsmillivolts into temperature
	 */
	f8_temp = (f8_temp - 500) / 10;
}

/**
 * @fn              char* ftoa(f8 f8_f, char* buf, s2 s2_precision)
 * @fid             [FID012]-[ftoa]
 * @fnbrf           Convert floating point to string.
 * @param[in]       f8_f ; f8 ; floating point input 
 * @param[in,out]   *buf ; char ; buffer output
 * @retval			*ftoa ; char ; pointer
 * @warning         May side effect with s1 (signed char) 
 * @remark          -
 */
char* ftoa(f8 f8_f, char* buf, s2 s2_precision)
{
	char* ptr = buf;
	char* p = ptr;
	char* ptr1;
	char c;
	s4 intPart;

	if (s2_precision > MAX_PRECISION)
	{
		s2_precision = MAX_PRECISION;
	}

	if (f8_f < 0)
	{
		f8_f = -f8_f;
		*ptr++ = '-';
	}

	if (s2_precision < 0)
	{
		if (f8_f < 1.0)
		{
			s2_precision = 6;
		}
		else if (f8_f < 10.0)
		{
			s2_precision = 5;
		}
		else if (f8_f < 100.0)
		{
			s2_precision = 4;
		}
		else if (f8_f < 1000.0)
		{
			s2_precision = 3;
		}
		else if (f8_f < 10000.0)
		{
			s2_precision = 2;
		}
		else if (f8_f < 100000.0)
		{
			s2_precision = 1;
		}
		else
		{
			s2_precision = 0;
		}
	}

	if (s2_precision)
	{
		f8_f += rounders[s2_precision];
	}

	intPart = f8_f;
	f8_f -= intPart;

	if (!intPart)
	{
		*ptr++ = '0';
	}
	else
	{
		p = ptr;

		while (intPart)
		{
			*p++ = '0' + intPart % 10;
			intPart /= 10;
		}

		ptr1 = p;

		while (p > ptr)
		{
			c = *--p;
			*p = *ptr;
			*ptr++ = c;
		}

		ptr = ptr1;
	}

	if (s2_precision)
	{
		*ptr++ = '.';

		while (s2_precision--)
		{
			f8_f *= 10.0;
			c = f8_f;
			*ptr++ = '0' + c;
			f8_f -= c;
		}
	}

	*ptr = 0;

	return buf;
}
