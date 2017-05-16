#include "derivative.h"
#include "lcd.h"

// Segment layout
//
//     AAA 
//     F B
//     FGB
//     E C
//  DP DDD

// Lumex p/n LCD-S401C39TR, static drive, 5v, pin assignments:
//  0   1  2  3  4  5  6  7   8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29
// COM 4F 4D 4C 3E 3D 3C 2DP 2E 2D 2C 1E 1D 1C 1B 1A 1F 1G 2B 2A 2F 2G  L 3B 3A 3F 3G 4B 4A 4G

// Differences for Varitronix VI-321:  3B = 22, 3A = 23, 3F = 24, 3G = 25, COM = 26

// Initialize LCD module

void LCDInit(void)
{
	//Enable LCD Pins 

#ifdef VI321
   LCDPEN0 = 0b11111111;
   LCDPEN1 = 0b00001111;
   LCDPEN2 = 0b11111100;
   LCDPEN3 = 0b00111111;
#else
   LCDPEN0 = 0xFF;
   LCDPEN1 = 0xFF;
   LCDPEN2 = 0xFF;
   LCDPEN3 = 0xFF;
#endif
       
	//Enable Back Plane Pin
#ifdef VI321
	LCDBPEN0 = 1;
	LCDWF0 = 0xff;
#else
	LCDBPEN0 = 0x01; 
	LCDWF0 = 0xff;				// Active across all phases
#endif

	///LCD Control registers
	LCDRVC = 0x0b;				// Voltage trim (not used)
	LCDSUPPLY = 0x05;
	LCDSUPPLY_LADJ = 0;

	BlinkOff();
	LCDC0 =    0xA0;  		// 1:1 duty
}

// Maps segments of each digit to the corresponding LCD pin


#ifdef VI321

const char seg_map[4][7] = {
	{11, 18, 19, 20, 21, 10, 9},
	{7, 8, 23, 24, 25, 6, 5},
	{3, 4, 27, 28, 29, 2, 1},
	{3, 4, 27, 28, 29, 2, 1}
	};

#else

const char seg_map[4][7] = {
	{15, 14, 13, 12, 11, 16, 17},
	{19, 18, 10,  9,  8, 20, 21},
	{24, 23,  6,  5,  4, 25, 26},
	{28, 27,  3,  2,  1,  1, 29}
	};

#endif

// Defines digits (and special symbols)

const char seven_seg[12][7] = {
	{1, 1, 1, 1, 1, 1, 0},	// 0
	{0, 1, 1, 0, 0, 0, 0},	// 1
	{1, 1, 0, 1, 1, 0, 1},	// 2
	{1, 1, 1, 1, 0, 0, 1},	// 3
	{0, 1, 1, 0, 0, 1, 1},	// 4
	{1, 0, 1, 1, 0, 1, 1},	// 5
	{1, 0, 1, 1, 1, 1, 1},	// 6
	{1, 1, 1, 0, 0, 0, 0},	// 7
	{1, 1, 1, 1, 1, 1, 1},	// 8
	{1, 1, 1, 1, 0, 1, 1},	// 9
	{0x00, 0x01, 0x10, 0x00, 0x01, 0x10, 0x11}, // Zigzag animation
	{0, 0, 0, 0, 0, 0, 0}
};	

// Enables blink mode

void BlinkOn(void)
{
	LCDBCTL_ALT = 0;			// Alternate between A and E entries
	LCDBCTL_BMODE = 1;		// Set alternate mode
	LCDBCTL_BRATE = 3;		// 1 hz
	LCDBCTL_BLINK = 1;		// Enable blink mode
}

// Disables blink mode

void BlinkOff(void)
{
	LCDBCTL_BLINK = 0;		// Disable blink mode
}

// Displays a single digit

void SetDigit(char digit, char value)
{
	*(&LCDWF0 + seg_map[digit][0]) = seven_seg[value][0];
	*(&LCDWF0 + seg_map[digit][1]) = seven_seg[value][1];
	*(&LCDWF0 + seg_map[digit][2]) = seven_seg[value][2];
	*(&LCDWF0 + seg_map[digit][3]) = seven_seg[value][3];
	*(&LCDWF0 + seg_map[digit][4]) = seven_seg[value][4];
	*(&LCDWF0 + seg_map[digit][5]) = seven_seg[value][5];
	*(&LCDWF0 + seg_map[digit][6]) = seven_seg[value][6];	
}
 
// Clears all segments

void ClearDisplay(void)
{
	LCDWF1 = 0x00;
	LCDWF2 = 0x00;
	LCDWF3 = 0x00;
	LCDWF4 = 0x00;
	LCDWF5 = 0x00;
	LCDWF6 = 0x00;
	LCDWF7 = 0x00;
	LCDWF8 = 0x00;
	LCDWF9 = 0x00;
	LCDWF10 = 0x00;
	LCDWF11 = 0x00;
	LCDWF12 = 0x00;
	LCDWF13 = 0x00;
	LCDWF14 = 0x00;
	LCDWF15 = 0x00;
	LCDWF16 = 0x00;  
	LCDWF17 = 0x00;
	LCDWF18 = 0x00;
	LCDWF19 = 0x00;
	LCDWF20 = 0x00;
	LCDWF21 = 0x00;
	LCDWF22 = 0x00;
	LCDWF23 = 0x00;
	LCDWF24 = 0x00;
	LCDWF25 = 0x00;
	LCDWF27 = 0x00;
	LCDWF28 = 0x00;
	LCDWF29 = 0x00;
	LCDWF30 = 0x00;
	LCDWF31 = 0x00;
	LCDWF26 = 0x00;
	LCDWF27 = 0x00;
	LCDWF28 = 0x00;
	LCDWF29 = 0x00;
}

 