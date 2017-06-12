#include <nokia5110.h>

#define PIN_RESET 12  // LCD RST .... Pin 1
                     // LCD Gnd .... Pin 2
#define PIN_SCE   11  // LCD CS  .... Pin 3
#define PIN_SCLK 13  // LCD SPIClk . Pin 4
#define PIN_DC    10  // LCD Dat/Com. Pin 5
#define PIN_SDIN  9  // LCD SPIDat . Pin 6
                     // LCD Vlcd ... Pin 7
                     // LCD Vcc .... Pin 8
                     
Nokia5110 lcd(PIN_RESET, PIN_SCE, PIN_SCLK, PIN_DC, PIN_SDIN);              

void setup() {
  // put your setup code here, to run once:
  lcd.init();
  lcd.setPos(1, 3);
  lcd.putStr("Hello World !");
}

void loop() {
  static int i = 0;
  // put your main code here, to run repeatedly: 
  
  i++;
  lcd.setPos(0,0);
  lcd.putSymbol((byte*)&i, 2);
  delay(100);
}
