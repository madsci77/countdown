#include <hidef.h>
#include "derivative.h"
#include <string.h>
#include "lcd.h"

/*

Yearly countdown timer
MC9S08LL16

To set:

Make sure interval starts with number of days between the upcoming burn and the next year's burn.
Set START_DAYS to number of full days before the burn, and START_MINS to the time from programming
until midnight.

*/

const unsigned short interval[] = {	364,	// 2016-2017
												364,	// 2017-2018
												364,	// 2018-2019
												371,	// 2019-2020
												364, 364, 364, 364, 364, 371, 364, 364, 364, 364, 364, 371, 364, 364, 364, 364, 371, 363, 364, 364, 364, 364, 371};

#define AsmStop  __asm STOP;  
#define AsmNop   __asm NOP;

volatile unsigned char state = 1;

// Starting conditions - to be replaced with external configuration input signal

#define START_DAYS 11
#define START_MINS 900

// Not zeroed out, so retained across resets (RFI, SEUs)
volatile unsigned short days @ 0x870; 				// Days in count (0 = target date)
volatile unsigned short mins @ 0x872;				// Minutes left in this day
volatile unsigned short year @ 0x874;				// Year index, starting with 2013
volatile unsigned short safety_sig @ 0x860;		// Signature indicating RAM previously initialized
volatile unsigned short safety_days @ 0x862;		// 1s complement of counters for data validity checking
volatile unsigned short safety_year @ 0x864;
volatile unsigned short safety_mins @ 0x866;



// ********* System Initialization **********

void SystemInit(void)
{

  	// Set up internal clock source (32.768 kHz crystal)
  
	ICSC2 = 0x0f;			// Low power, oscillator requested, ICSERCLK enabled, enabled in stop
	ICSC1 = 0x80;			// External reference, FLL bypassed external

	SOPT1 = 0x60;			// COP disabled, STOP enabled
//	SOPT1 = 0x63;			// Enable BDM and RST pins for testing
	SPMSC1 = 0x00;			// LVD disabled

	IRQSC_IRQPE = 0;		// IRQ pin disabled

	// Set up time-of-day clock
	
	TODC_TODCLKS = 0;		// Clock source
	TODC_TODPS = 1;		// Prescaler
	TODSC_MTCHIE = 1;		// Match interrupt enabled
	TODSC_MTCHEN = 1;		// Match enabled
	TODSC_SECIE = 0;		// No second interrupt
	TODSC_QSECIE = 0;		// No quarter-second interrupt
	TODM = 60 << 2;	// Match count (every 60 seconds)
//	TODM = 4 << 2;			// Fast clock for testing
	TODC_TODEN =1;			// Enable module
 
	// All I/O pins to output
	PTADD = PTBDD = PTCDD = PTDDD = PTEDD = 0xFF;

	// Set all I/O pins low
	PTAD = PTBD = PTCD = PTDD = PTED = 0x00;

	LCDInit();

	SCGC1 = 0b10001000; 	// TPM2, TPM1, ADC, I2C, and SCI clocks disabled
	SCGC2 = 0b01000110;	// IRQ, KBI, ACMP, and SPI clocks disabled

	SPMSC2 = 0x80;			// LPrun enabled

	EnableInterrupts;
     
}

// ********* Main Loop **********

void main(void) 
{
	int i;
	// If signature and complments match, data is still valid across CPU reset - otherwise load defaults
	if ((safety_sig != 0xabcd) || (days != ~safety_days) || (mins != ~safety_mins) || (year != ~safety_year))
	{
		days = START_DAYS;
		mins = START_MINS;
		year = 0;
	}
	
	SystemInit();

	ClearDisplay();

	for (i = 2550; i; i--) {
		AsmNop;
		AsmNop;
		AsmNop;
	}

	for(;;)	
	{   

		if (state)
		{
			// Only update display on change to avoid extra time doing math
			if (days)
			{
				BlinkOff();
				SetDigit(0, days % 10); 
				if (days >= 10) SetDigit(1, (days % 100) / 10);
				else SetDigit(1, LCD_BLANK);
				if (days >= 100) SetDigit(2, (days / 100) & 0xff);
				else SetDigit(2, LCD_BLANK);
			}
			else
			{
				// Target date - display animation
				SetDigit(0, LCD_ZIGZAG);
				SetDigit(1, LCD_ZIGZAG);
				SetDigit(2, LCD_ZIGZAG);
				BlinkOn();
			}
			state = 0;
		}
		AsmStop;    
	}
}


// ********* Time-of-Day Interrupt **********

interrupt VectorNumber_Vtod void tod_ISR(void)
{
	safety_sig = 0xabcd;
	if (mins)
	{
		mins--;
	}
	else
	{
		mins = 1439;
		if (days)
		{
			days--;
		}
		else
		{
			// Done with this year and last-day animation, load next interval
			days = interval[year]-1;
			year++;		
			safety_year = ~year;
		}		
		state = 1;
		safety_days = ~days;
	}
	safety_mins = ~mins;
	// Acknowledge interrupt sources
	TODSC_SECF = 1; 
	TODSC_MTCHF = 1;
	TODSC_QSECF = 1;
}
