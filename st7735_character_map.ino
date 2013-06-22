//  st7735_character_map.ino - 6/22/2013 - Chris K, ckelley@ca-cycleworks.com
//
// Code snippets are based on various Adafruit examples
// Adafruit 1.8TFT w/joystick 160x128 in landscape (128x160)
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SD.h>
#include <SPI.h>
// pinout for Mega: http://forums.adafruit.com/viewtopic.php?uid=115205&f=31&t=36738&start=0
#define sclk 13
#define mosi 11
#define cs  10
#define dc   8
#define rst -1  // you can also connect this to the Arduino reset
//
#define ADA_BUTTON 3  // analog pin 3 for button input
#define SD_CS 4  // SD card chip select digital pin 4

// Works with our screen rotation (3), yaay
#define Neutral 0
#define Press 1
#define Up 2
#define Down 3
#define Right 4
#define Left 5

// TFT Globals
Adafruit_ST7735 tft = Adafruit_ST7735(cs, dc, mosi, sclk, rst);
volatile uint8_t button = 0;

void setup() {         
  // If your TFT's plastic wrap has a Red Tab, use the following:
  tft.initR(INITR_REDTAB);   // initialize a ST7735R chip, red tab
  // The rotation parameter can be 
  // 0, sets the display to a portrait (tall) mode, with the USB jack at the top right 
  // 1, also a portrait mode, with the USB jack at the bottom left
  // 2, landscape (wide) mode, with the USB jack at the bottom right
  // 3, also landscape, but with the USB jack at the top left
  tft.setRotation(3); 
//  tft.fillScreen(BLACK);
  tft.setTextSize(1); //  1 = 5x8, 2 = 10x16; chars leave blank pixel on bottom 
}

// text size 1 "0123456789abcdef0123456789" » 16 lines of 0x20 (decimal 26) characters
// text size 2 "0123456789abc" » 8 lines of 13 characters

int page=0;
int lastpage=1; // make different for 1st time thru loop()
// used initializer to make the string long enough!
char lineText[0x22]="0x--- 0123 4567 89ab cdef\n";  

void loop() {
  char text[512];
  char fBuffer[10];
  float position;
  int _char=page*15*0x10;
  // 0x12 = up+down arrow in once character
  // 0x18 = up arrow  0x1e = up arrow solid
  // 0x19 = down arr  0x1f = down arr solid
  // diameter = 0xec
  // identity, equality = 0xef
  // left arrow quote = 0xae, r arrow quote » = 0xaf
  // 0x413 is double exclamation!
  // tft.setTextWrap(false);
  if( page != lastpage ){
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(0,0);
    // print header
    tft.setTextColor(ST7735_BLUE);
    sprintf( fBuffer, "pg %-4d", page+1); // page 0 doesn't make sense...
    sprintf( lineText, "%6.6s 0123 4567 89ab cdef", fBuffer );
    tft.print(lineText);
    for( int line=0 ; line <= 15; line++ ) {
      if(_char%0x100 == 0 ) { // skip first rows of each char set
          tft.setTextColor(ST7735_RED);
          sprintf( fBuffer, "%#x", _char);
          sprintf( lineText, "%6.6s %c skip row with \\n ", fBuffer, 0x19 );
          tft.print(lineText);
          _char += 0x10;
        continue;
      }
      tft.setTextColor(ST7735_BLUE);
      sprintf( fBuffer, "%#x", _char);
      // sprintf( lineText, "%6.6s 0123 4567 89ab cdef\n", fBuffer );
      sprintf( lineText, "%6.6s", fBuffer );
      tft.print(lineText);
      tft.setTextColor(ST7735_WHITE);
      for( int block=0; block<4; block++){
        sprintf( lineText, " %c%c%c%c"
          , _char 
          , _char+1 
          , _char+2 
          , _char+3 
          );
        _char+=4;
        tft.print(lineText);
      }
      
      
    }
    lastpage=page;  
  }

  button=checkJoystick();
  while( button != Neutral ){
    if( button == Down )
      page++;
    if( button == Up && page > 0 )
      page--;
    button = Neutral;
    delay(250);
    button=checkJoystick();
  }
}

// Check the joystick position
int checkJoystick()
{
  int joystickState = analogRead(ADA_BUTTON);
  if (joystickState < 50) return Left;
  if (joystickState < 150) return Down;
  if (joystickState < 250) return Press;
  if (joystickState < 500) return Right;
  if (joystickState < 650) return Up;
  return Neutral;
}
