#ifndef _ST7066_H
#define _ST7066_H

#define RS 2 // RS pin
#define RW 3 // RW pin
#define RE 4 // RE pin
// data pins
#define D0 17
#define D1 27
#define D2 22
#define D3 23
// used in 4 bit mode
#define D4 24
#define D5 10
#define D6 9
#define D7 11

#define  BUSY_FLAG D7
#define  INIT_SEQ 0x30
#define  DISP_CLEAR 0x01
#define  DISP_OFF 0x08
#define  DISP_ON 0x0C
#define  CURSOR_ON 0x0E
#define  CURSOR_OFF 0x0C
#define  CURSOR_BLINK 0x0F
#define  RETURN_HOME 0x02
#define  ENTRY_MODE 0x06
#define  _4BIT_MODE 0x20
#define  _8BIT_MODE 0x30
#define  _2_ROWS 0x08
#define  FONT_5x8 0x00
#define  FONT_5x10 0x04
#define  POSITION 0x80

#define  SHIFT 0x10
#define  DISPLAY 0x08
#define  LEFT 0x00
#define  RIGHT 0x04
#define  CURSOR 0x00

#define  CLEARLINE "                   " //length 20

#define  ROWS 2  
#define  COLS 40 // 40x2 
#define  COLS2 80
#define  ROW1_START 0x00
#define  ROW1_END  COLS
#define  ROW2_START 0x40
#define  ROW2_END  COLS2

#define  MODE  4BIT_MODE

#define  CLRBIT(REG) \
    {                           \
        GPIO_CLR = 1 << REG;    \
    }
#define  SETBIT(REG) \
    {                           \
        GPIO_SET = 1 << REG;    \
    }

#define ERROR 1

void  SetD0toD7output();
void  ClrPin(uint8_t PIN);
void  SetPin(uint8_t PIN);
void  Send8bitsIn8bitMode(unsigned short int buffer);
void  SendInstruction(unsigned short int data);
void  Initialize8bit();
void  Disable();
void  Enable();
void  PulseE();
void  SendData(unsigned short int data);
void  DrawChar(char character);
void  DrawString(char *str);
void  DisplayClear();
void  DisplayOn(void);
void  CursorOn(void);
void  CursorOff(void);
void  CursorBlink(void);
char  Shift(char item, char direction);
void  ShiftStringRight(char *str);
void  DisplayOff(void);
char  PositionXY(uint8_t x, uint8_t y);
void  Msleep(uint8_t time);
void  Startup();
void  GetandPrintTime();
void  RemoveAllPulls();
void  SetAllOutput();
uint8_t  PositionXYString(uint8_t x, uint8_t y, char *str);
void  SetUppNibble(unsigned short int data);
void  SetLowNibble(unsigned short int data);
void  Send8bitsIn4bitMode(unsigned short int data);
void  Send4bitsIn4bitMode(unsigned short int data);
void  SetD4toD7output();
void  Send4bitsIn4bitMode(unsigned short int data);
void  Initialize4bitNew();
void  SetD4toD7pullup();
void  SetD4toD7input();
void  CheckBFin4bitMode();
#endif