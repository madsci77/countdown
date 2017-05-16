// LCD header

#define LCD_ZIGZAG 10
#define LCD_BLANK 11

extern void SetDigit(char digit, char value);
extern void ClearDisplay(void);
extern void BlinkOn(void);
extern void BlinkOff(void);
extern void LCDInit(void);