#define BCM2708_PERI_BASE 0x3F000000
#define GPIO_BASE (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <string.h>
#include <pigpio.h>
#include <time.h>
#include <pthread.h>
#include "ST7066.h"
#include <time.h>

#define PAGE_SIZE (4 * 1024)
#define BLOCK_SIZE (4 * 1024)

static uint8_t dataPins[] = {D0, D1, D2, D3, D4, D5, D6, D7};
char SCREEN_BUFFER[80];

pthread_mutex_t  Mutex = PTHREAD_MUTEX_INITIALIZER;

void  modeSelect(uint8_t registers_sw) // tamam-test et
{
    switch (registers_sw)
    {
    case 0:
         ClrPin(RS);
         ClrPin(RW);
        break;

    case 1:
         ClrPin(RS);
         SetPin(RW);
        break;

    case 2:
         SetPin(RS);
         ClrPin(RW);
        break;

    case 3:
         SetPin(RS);
         SetPin(RW);
        break;

    default:
        break;
    }
}

void  Enable() // tamam-test et
{
     SetPin(RE);
}

void  Disable() // tamam-test et
{
     ClrPin(RE);
}

void  Initialize8bit() // tamam-test et
{

    gpioSetMode(RE, PI_OUTPUT);
    gpioSetMode(RW, PI_OUTPUT);
    gpioSetMode(RS, PI_OUTPUT);

     SetD0toD7output();

     ClrPin(RS);
     ClrPin(RW);

    usleep(16000);

     SendInstruction( INIT_SEQ);
    usleep(4500);
     PulseE();
    usleep(150);
     PulseE();
    usleep(50);
     SendInstruction(  _8BIT_MODE |   _2_ROWS |  FONT_5x8);
     SendInstruction( DISP_OFF);
     SendInstruction( DISP_CLEAR);
     SendInstruction( ENTRY_MODE);
}

void  Initialize4bit() // tamam-test et
{

    gpioSetMode(RE, PI_OUTPUT);
    gpioSetMode(RW, PI_OUTPUT);
    gpioSetMode(RS, PI_OUTPUT);

     SetD4toD7output();
    usleep(16000);

     ClrPin(RS);
     ClrPin(RW);
     ClrPin(RE);
     Send4bitsIn4bitMode(0b0011); // 00110000
    usleep(4300);
     PulseE();
    usleep(150);
     Send4bitsIn4bitMode(0b0011);

     Send4bitsIn4bitMode(0b0010);
     Send4bitsIn4bitMode(0b0010);
     Send4bitsIn4bitMode(0b0010);

     Send4bitsIn4bitMode(0b0000);
     Send4bitsIn4bitMode(0b1000);

     Send4bitsIn4bitMode(0b0000);
     Send4bitsIn4bitMode(0b0001);

     Send4bitsIn4bitMode(0b0000);
     Send4bitsIn4bitMode(0b0110);
}

void  Initialize4bitNew() // tamam-test et
{
    gpioSetMode(RE, PI_OUTPUT);
    gpioSetMode(RW, PI_OUTPUT);
    gpioSetMode(RS, PI_OUTPUT);

     SetD4toD7output();

     ClrPin(RS);
     ClrPin(RW);
     ClrPin(RE);
    usleep(16000);
     Send4bitsIn4bitMode( INIT_SEQ);
    usleep(4500);
     Send4bitsIn4bitMode( INIT_SEQ);
    usleep(150);
     Send4bitsIn4bitMode( INIT_SEQ);
    usleep(50);
     Send4bitsIn4bitMode(  _4BIT_MODE);

     SendInstruction( _4BIT_MODE | 0x02 |  FONT_5x8);
     SendInstruction( DISP_OFF);
     SendInstruction( DISP_CLEAR);
     SendInstruction( ENTRY_MODE);
}

void  SendInstruction(unsigned short int data) // modifiye
{
    // Clear RS
     ClrPin(RS);
    // send required data in required mode
    if ( MODE ==   _8BIT_MODE)
    {
         Send8bitsIn8bitMode(data);
    }
    else if ( MODE ==   _4BIT_MODE)
    {
         Send8bitsIn4bitMode(data);
        //  CheckBFin4bitMode();
    }
    usleep(500);
    // check busy flag
}

void  SendData(unsigned short int data) // modifiye
{
    // Set RS
     SetPin(RS);
    // send required data in required mode
    if ( MODE ==   _8BIT_MODE)
    {
         Send8bitsIn8bitMode(data);
    }
    else if ( MODE ==   _4BIT_MODE)
    {
         Send8bitsIn4bitMode(data);
        //  CheckBFin4bitMode();
    }
    usleep(50);
    // check busy flag
    //  CheckBFin8bitMode();
     ClrPin(RS);
}

void  Send8bitsIn8bitMode(unsigned short int buffer) // modifiye
{
    uint8_t binary[] = {0, 0, 0, 0, 0, 0, 0, 0};

    int i = 0;

    // Set E
     SetPin(RE);

    // Send data to LCD
    while (buffer > 0)
    {
        binary[i] = buffer % 2;
        buffer = buffer / 2;
        i++;
    }

    for (int g = 7; g >= 0; g--)
    {
        if (binary[g] == 1)
        {
             SetPin(dataPins[g]);
            // usleep(50);
        }
        else if (binary[g] == 0)
        {
             ClrPin(dataPins[g]);
            // usleep(50);
        }
    }
    usleep(50);
    // PWeh delay time > 450ns
    // Clear E
     ClrPin(RE);
    // TcycE > 1000ns -> delay depends on PWeh delay time
    // delay = TcycE - PWeh = 1000 - 500 = 500ns
    usleep(50);
}

void  ClrPin(uint8_t PIN) // tamam-test et
{
    gpioWrite(PIN, 0);
}

void  SetPin(uint8_t PIN) // tamam-test et
{
    gpioWrite(PIN, 1);
}

void  PulseE()
{
    // Set E
     SetPin(RE);
    // PWeh delay time > 450ns
    usleep(1);
    // Clear E
     ClrPin(RE);
    // TcycE > 1000ns -> delay depends on PWeh delay time
    // delay = TcycE - PWeh = 1000 - 500 = 500ns
    usleep(1);
}

void  SetD0toD7output()
{
    gpioSetMode(D0, PI_OUTPUT);
    gpioSetMode(D1, PI_OUTPUT);
    gpioSetMode(D2, PI_OUTPUT);
    gpioSetMode(D3, PI_OUTPUT);
    gpioSetMode(D4, PI_OUTPUT);
    gpioSetMode(D5, PI_OUTPUT);
    gpioSetMode(D6, PI_OUTPUT);
    gpioSetMode(D7, PI_OUTPUT);
}

void  DrawString(char *str)
{
    unsigned char i = 0;
    // loop through 5 bytes
    while (str[i] != '\0')
    {
        // read characters and increment index
         SendData(str[i++]);
    }
}

void  DrawChar(char character)
{
    // Diplay clear
     SendData(character);
}

void  DisplayOn(void)
{
    // send instruction - display on
     SendInstruction( DISP_ON);
}

void  DisplayClear(void)
{
    // Diplay clear
     SendInstruction( DISP_CLEAR);
}

void  CursorBlink(void)
{
    // send instruction - Cursor blink
     SendInstruction( CURSOR_BLINK);
}

void  CursorOff(void)
{
    // send instruction - cursor on
     SendInstruction( CURSOR_OFF);
}

void  CursorOn(void)
{
    // send instruction - cursor on
     SendInstruction( CURSOR_ON);
}

char  Shift(char item, char direction)
{
    // check if item is cursor or display or direction is left or right
    if ((item !=  DISPLAY) && (item !=  CURSOR))
    {
        // error
        return ERROR;
    }
    // check if direction is left or right
    if ((direction !=  RIGHT) && (direction !=  LEFT))
    {
        // error
        return ERROR;
    }

    // cursor shift
    if (item ==  CURSOR)
    {
        // right shift
        if (direction ==  RIGHT)
        {
            // shit cursor / display to right / left
             SendInstruction( SHIFT |  CURSOR |  RIGHT);
        }
        else
        {
            // shit cursor / display to right / left
             SendInstruction( SHIFT |  CURSOR |  LEFT);
        }
        // display shift
    }
    else
    {
        // right shift
        if (direction ==  RIGHT)
        {
            // shit cursor / display to right / left
             SendInstruction( SHIFT |  DISPLAY |  RIGHT);
        }
        else
        {
            // shit cursor / display to right / left
             SendInstruction( SHIFT |  DISPLAY |  LEFT);
        }
    }
    // success
    return 0;
}

void  ShiftStringRight(char *str)
{
     DrawString(str);

    while (1)
    {
        usleep(100000);

         Shift( DISPLAY,  RIGHT);
        //  SendInstruction( DISP_CLEAR);

        usleep(100000);
    }
}

char  PositionXY(uint8_t x, uint8_t y)
{
    if (x >  COLS || y >  ROWS)
    {
        return ERROR;
    }
    switch (y)
    {
    case 0:
         SendInstruction( POSITION +  ROW1_START + x);
        break;
    case 1:

         SendInstruction( POSITION +  ROW2_START + x);
        break;
    }
}

void  DisplayOff(void)
{
    // send instruction - display on
     SendInstruction( DISP_OFF);
}

void  Msleep(uint8_t time)
{
    for (int i = 0; i < 1000; i++)
    {
        usleep(time);
    }
}

void  RemoveAllPulls()
{
    printf("RE %d\n", gpioSetPullUpDown(RE, PI_PUD_OFF));
    printf("RW %d\n", gpioSetPullUpDown(RW, PI_PUD_OFF));
    printf("RS %d\n", gpioSetPullUpDown(RS, PI_PUD_OFF));
    // data pins
    printf("D0 %d\n", gpioSetPullUpDown(D0, PI_PUD_OFF));
    printf("D1 %d\n", gpioSetPullUpDown(D1, PI_PUD_OFF));
    printf("D2 %d\n", gpioSetPullUpDown(D2, PI_PUD_OFF));
    printf("D3 %d\n", gpioSetPullUpDown(D3, PI_PUD_OFF));
    printf("D4 %d\n", gpioSetPullUpDown(D4, PI_PUD_OFF));
    printf("D5 %d\n", gpioSetPullUpDown(D5, PI_PUD_OFF));
    printf("D6 %d\n", gpioSetPullUpDown(D6, PI_PUD_OFF));
    printf("D7 %d\n", gpioSetPullUpDown(D7, PI_PUD_OFF));
}

void  SetAllOutput()
{

    gpioSetMode(RE, PI_OUTPUT);
    gpioSetMode(RW, PI_OUTPUT);
    gpioSetMode(RS, PI_OUTPUT);
    // data pins
    gpioSetMode(D0, PI_OUTPUT);
    gpioSetMode(D1, PI_OUTPUT);
    gpioSetMode(D2, PI_OUTPUT);
    gpioSetMode(D3, PI_OUTPUT);
    gpioSetMode(D4, PI_OUTPUT);
    gpioSetMode(D5, PI_OUTPUT);
    gpioSetMode(D6, PI_OUTPUT);
    gpioSetMode(D7, PI_OUTPUT);
}

void  Startup()
{
    if ( MODE ==   _8BIT_MODE)
    {
         Initialize8bit();
    }
    else if ( MODE ==   _4BIT_MODE)
    {
         Initialize4bitNew();
    }

     DisplayOn();
}


uint8_t  PositionXYString(uint8_t x, uint8_t y, char *str)
{
    pthread_mutex_lock(&Mutex);

    switch (y)
    {
    case 0:
         SendInstruction( POSITION +  ROW1_START + x);
        break;
    case 1:

         SendInstruction( POSITION +  ROW2_START + x);
        break;
    }
    unsigned char i = 0;

    while (str[i] != '\0')
    {
        // read characters and increment index
         SendData(str[i++]);
    }
    pthread_mutex_unlock(&Mutex);
}

void  SetUppNibble(unsigned short int data)
{
    // clear bits DB7-DB4
     ClrPin(D7);
     ClrPin(D6);
     ClrPin(D5);
     ClrPin(D4);

    // set DB7-DB4 if corresponding bit is set

    if (data & 0x80)
    {
         SetPin(D7);
    }
    if (data & 0x40)
    {
         SetPin(D6);
    }
    if (data & 0x20)
    {
         SetPin(D5);
    }
    if (data & 0x10)
    {
         SetPin(D4);
    }
    usleep(50);
}

void  SetLowNibble(unsigned short int data)
{
    // clear bits DB7-DB4
     ClrPin(D3);
     ClrPin(D2);
     ClrPin(D1);
     ClrPin(D0);

    // set DB7-DB4 if corresponding bit is set

    if (data & 0x80)
    {
         SetPin(D3);
    }
    if (data & 0x40)
    {
         SetPin(D2);
    }
    if (data & 0x20)
    {
         SetPin(D1);
    }
    if (data & 0x10)
    {
         SetPin(D0);
    }
    usleep(50);
}

void  Send8bitsIn4bitMode(unsigned short int data)
{
    // Send upper nibble
    // ----------------------------------
    // Set E
     SetPin(RE);
    // send data to LCD
     SetUppNibble(data);
    // PWeh delay time > 450ns
    usleep(1);
    // Clear E
     ClrPin(RE);
    // TcycE > 1000ns -> delay depends on PWeh delay time
    // delay = TcycE - PWeh = 1000 - 500 = 500ns
    usleep(1);

    // Send lower nibble
    // ----------------------------------
    // Set E
     SetPin(RE);
    // send data to LCD
     SetUppNibble(data << 4);
    // PWeh delay time > 450ns
    usleep(1);
    // Clear E
     ClrPin(RE);
    // TcycE > 1000ns -> delay depends on PWeh delay time
    // delay = TcycE - PWeh = 1000 - 500 = 500ns
    usleep(1);
}

void  Send4bitsIn4bitMode(unsigned short int data)
{
    // Set E
     SetPin(RE);
    // send data to LCD
     SetUppNibble(data);
    // PWeh delay time > 450ns
    usleep(1);
    // Clear E
     ClrPin(RE);
    // TcycE > 1000ns -> delay depends on PWeh delay time
    // delay = TcycE - PWeh = 1000 - 500 = 500ns
    usleep(1);
}

void  CheckBFin4bitMode()
{
    unsigned char input = 0;
     SetD4toD7input();

     ClrPin(RS);
     SetPin(RW);

    while (1)
    {

        // Read upper nibble
        // --------------------------------
        // Set E
         SetPin(RE);
        // PWeh > 0.5us
        usleep(1);
        // read upper nibble (tDDR > 360ns)
        input = gpioRead(D7);
        // Clear E
         ClrPin(RE);
        // TcycE > 1000ns -> delay depends on PWeh delay time
        // delay = TcycE - PWeh = 1000 - 500 = 500ns
        usleep(1);

        // Read lower nibble
        // --------------------------------
        // Set E
         SetPin(RE);
        // PWeh > 0.5us
        usleep(1);
        // read lower nibble (tDDR > 360ns)
        input |= gpioRead(D7) >> 4;
        // Clear E
         ClrPin(RE);
        // TcycE > 1000ns -> delay depends on PWeh delay time
        // delay = TcycE - PWeh = 1000 - 500 = 500ns
        usleep(1);
        printf("flag busy\n");
        // check if DB7 is cleared
        if (!(input & (1 << gpioRead(D7))))
        {
            // if BF cleared -> end loop
            break;
        }
    }
    printf("flag released.\n");
    // clear RW
     ClrPin(RW);

    // set DB7-DB4 as output
     SetD4toD7output();
}

void  SetD4toD7input()
{
    gpioSetMode(D4, PI_INPUT);
    gpioSetMode(D5, PI_INPUT);
    gpioSetMode(D6, PI_INPUT);
    gpioSetMode(D7, PI_INPUT);
}

void  SetD4toD7output()
{
    gpioSetMode(D4, PI_OUTPUT);
    gpioSetMode(D5, PI_OUTPUT);
    gpioSetMode(D6, PI_OUTPUT);
    gpioSetMode(D7, PI_OUTPUT);
}

void  SetD4toD7pullup()
{
    gpioSetPullUpDown(D4, PI_PUD_DOWN);
    gpioSetPullUpDown(D5, PI_PUD_DOWN);
    gpioSetPullUpDown(D6, PI_PUD_DOWN);
    gpioSetPullUpDown(D7, PI_PUD_DOWN);
}
