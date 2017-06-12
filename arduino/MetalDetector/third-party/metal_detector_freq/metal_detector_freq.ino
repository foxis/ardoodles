#include <Event.h>
#include <Timer.h>

#define LCD

#include <nokia5110.h>

// Frequency Counter Lib example

/*
  Martin Nawrath KHM LAB3
  Kunsthochschule f¸r Medien Kˆln
  Academy of Media Arts
  http://www.khm.de
  http://interface.khm.de/index.php/labor/experimente/	
 */
#include <FreqCounter.h>

Nokia5110 lcd(12,11,13,10,9);
Timer t;
int8_t osc_id = 0;
unsigned long frq;
unsigned long frq_0 = 0;
unsigned long frq_avg = 0;
unsigned long frq_delta_avg = 0;
int cnt;
int pinLed=13;
bool measurement = false;

void setup() {
//  pinMode(pinLed, OUTPUT);
  pinMode(6, OUTPUT);
  digitalWrite(7, HIGH);
  lcd.init();

  lcd.clear(0xFF);
  delay(2000);
  t.every(100, update_frequency);
  t.every(100, display_lcd);
  osc_id = t.oscillate(6, 100000, 0);
}

void update_frequency()
{
  if (measurement)
    return;
  // wait if any serial is going on
  FreqCounter::f_comp=10;   // Cal Value / Calibrate with professional Freq Counter
  FreqCounter::start(100);  // 100 ms Gate Time
  measurement = true;
}


#ifdef LCD
void put_pixel(int i, float mi, float ma, float v)
{
  int y = 3 * 8 - (int)((3 * 8) * ((v - mi) / (ma - mi)));
  int p = y / 8;
  int b = y % 8;
  byte data = 1<<b;
  byte data0 = 0;
  for (int j = 0; j < 3; j++)
  {
    lcd.setPos(i, j + 2);
    if (j == p)
      lcd.putSymbol(&data, 1);
    else
      lcd.putSymbol(&data0, 1);
  }
}

long pixels[84];

void display_lcd()
{
  long mi = 0xFFFFFF, ma = -0xFFFFFF;
  int i;

  for (i = 0; i < 84; i++)
  {
    mi = min(mi, pixels[i]);
    ma = max(ma, pixels[i]);
  }
  if (mi == ma)
  {
    ma++;
    mi--;
  }
  
  lcd.setPos(0,0);
  lcd.setPos(0,0);
  lcd.putLong(frq);
  lcd.setPos(0,1);
  lcd.putLong(frq_delta_avg);
  lcd.putStr("   ");
  lcd.setPos(48,1);
  lcd.putLong(frq - (frq_avg>>3));
  lcd.putStr("   ");

  for (i = 0; i < 84; i++)
  {
    put_pixel(i, mi, ma, pixels[i]);
  }
}
#endif

void loop() 
{
  if (digitalRead(7) == LOW || abs(frq_delta_avg) > 10000)
  {
    frq_delta_avg = 0;
    frq_0 = frq;
  }

  t.update();

  if (measurement && FreqCounter::f_ready)
  {  
    frq = FreqCounter::f_freq;
  
    frq_avg = frq_avg - (frq_avg>>3) + frq;
  //  frq_delta_avg = frq_delta_avg * 0.2 + ((float)frq_0 - (float)frq) * 0.8;
    frq_delta_avg = frq - frq_0;
  
  #ifdef LCD
    for (int i = 0; i < 83; i++)
    {
      pixels[i] = pixels[i + 1];
    }
    pixels[83] = frq - (frq_avg>>3);
  #endif
  
    t.stop(osc_id);
    osc_id = t.oscillate(6, 2000 / min(2000, max(1, abs(frq_delta_avg))), 0);
    measurement = false;
  }
}  

