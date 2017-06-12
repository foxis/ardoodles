#ifndef Nokia5110_h
#define Nokia5110_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include <pins_arduino.h>
#endif 
#include <inttypes.h>

#define LCD_PIXEL_WIDTH     84
#define LCD_PIXEL_HEIGHT     48
#define LCD_WIDTH			84
#define LCD_HEIGHT			5

class Nokia5110
{
  private:
	byte * _ASCII;
	int _Reset;
	int _SCE;
	int _SCLK;
	int _DC;
	int _SDIN;
	int _x, _y;

  public:
    Nokia5110(int pinReset, int pinSCE, int pinSCLK, int pinDC, int pinSDIN);

	void init();

	void setCharTable(byte * ASCII);
    void clear(byte val);
	void setPos(byte x, byte y);
	void putChar(byte ch);
	void putSymbol(const byte * symbol, byte size);
	void putStr(const char * str);
	void putSprite(const byte * sprite, byte width, byte height);

	void putByte(byte);
	void putInt(int);
	void putLong(long);
	void putBool(byte);

	void printAtBuf(byte x, byte y, char * buf, const __FlashStringHelper * format, ...);
	void printAt(byte x, byte y, const __FlashStringHelper * format, ...);

  protected:
	void write(byte dc, byte data);
};

#endif

