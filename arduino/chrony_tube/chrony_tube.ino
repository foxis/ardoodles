#include <EEPROM.h>
#include <SFE_BMP180.h>
#include <Switch.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 10
Adafruit_SSD1306 display(OLED_RESET);

#define pinA 2
#define pinB 3

#define buttonA 4
#define buttonB 5
#define buttonC 6
#define buttonD 7

#define SHOT_STRING_LENGTH 50
#define DEFAULT_DISTANCE 137
#define DISTANCE_ADDR  0
#define SHOT_COUNT_ADDR  1

typedef struct shot_info_struct
{
  float velocity;
  //unsigned long timestamp;
} shot_info_t;

enum DISPLAY_TYPE
{
  DT_UNKNOWN = -2,
  DT_ABOUT_INIT = -1,
  DT_LAST_SHOT,
  DT_SHOT_STRING,
  DT_SHOT_GRAPH,
//  DT_SHOT_PREDICT,
  DT_ABOUT,
//  DT_TRANSFER,
  DT_NULL,
};

volatile unsigned long timestampA = 0;
volatile unsigned long timestampB = 0;
volatile unsigned long tmpA = 0;
volatile unsigned long tmpB = 0;
volatile byte impulseA = 0;
volatile byte impulseB = 0;
volatile byte max_aval = 0; // maximum pressure
unsigned long timestamp = 0;
byte Distance = DEFAULT_DISTANCE;

DISPLAY_TYPE display_type = DT_ABOUT_INIT;
int display_pos = 0;
int display_pos1 = 0;
int total_shot_count = 0;
int shot_count = 0;
int last_shot_count = 0;
int ambient_pressure;
byte ambient_temperature;
shot_info_t velocity_values[SHOT_STRING_LENGTH] = {0, };
char buf[16] = "";
char buf1[16] = "";


Switch buttonUp = Switch(buttonA, INPUT_PULLUP, LOW, 3, 1000, 50);
Switch buttonDown = Switch(buttonB, INPUT_PULLUP, LOW, 3, 1000, 50);
Switch buttonEx1 = Switch(buttonC, INPUT_PULLUP, LOW, 3, 1000, 50);
Switch buttonEx2 = Switch(buttonD, INPUT_PULLUP, LOW, 3, 1000, 50);
SFE_BMP180 pressureSensor;

long readVcc() 
{
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
     ADMUX = _BV(MUX5) | _BV(MUX0) ;
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  
 
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring
 
  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both
 
  long result = (high<<8) | low;
 
  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}

void printAt(byte x, byte y, const __FlashStringHelper * format, ...)
{
  va_list args;
  va_start (args, format);
  vsnprintf_P(buf1, 16, (const char *)format, args);
  display.setCursor(x,y * 10);
  display.print(buf1);
} 
void init_display()
{
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.clearDisplay();
}

void printBatt(byte x, byte y, int vcc)
{
}

void clear()
{
  //long vcc = (unsigned int)readVcc() / 100;
  display.clearDisplay();
  //printAt(128-2*6 - 16,0, F("%i"), (int)vcc);
}

static const unsigned char PROGMEM battery_bmp0[] =
{
  B11111111,B11111000,
  B10000000,B00001000,
  B10000000,B00001110,
  B10000000,B00001110,
  B10000000,B00001110,
  B10000000,B00001000,
  B11111111,B11111000,
};
static const unsigned char PROGMEM battery_bmp1[] =
{
  B11111111,B11111000,
  B11100000,B00001000,
  B11100000,B00001110,
  B11100000,B00001110,
  B11100000,B00001110,
  B11100000,B00001000,
  B11111111,B11111000,
};
static const unsigned char PROGMEM battery_bmp2[] =
{
  B11111111,B11111000,
  B11101100,B00001000,
  B11101100,B00001110,
  B11101100,B00001110,
  B11101100,B00001110,
  B11101100,B00001000,
  B11111111,B11111000,
};
static const unsigned char PROGMEM battery_bmp3[] =
{
  B11111111,B11111000,
  B11101101,B10001000,
  B11101101,B10001110,
  B11101101,B10001110,
  B11101101,B10001110,
  B11101101,B10001000,
  B11111111,B11111000,
};
static const unsigned char PROGMEM battery_bmp4[] =
{
  B11111111,B11111000,
  B11101101,B10111000,
  B11101101,B10111110,
  B11101101,B10111110,
  B11101101,B10111110,
  B11101101,B10111000,
  B11111111,B11111000,
};
static const unsigned char PROGMEM battery_bmp5[] =
{
  B11111111,B11111000,
  B11111110,B01111000,
  B11111100,B11111110,
  B10000000,B11111110,
  B11111100,B11111110,
  B11111110,B01111000,
  B11111111,B11111000,
};

void show()
{
  long vcc = (unsigned int)readVcc();
  register byte i = 0;
  if(vcc > 2900) i++;
  if(vcc > 3200) i++;
  if(vcc > 3500) i++;
  if(vcc > 3800) i++;
  if(vcc > 4100) i++;

  switch (i)
  {
    case 0: display.drawBitmap(128-15, 0, battery_bmp0, 15, 7, 1); break;
    case 1: display.drawBitmap(128-15, 0, battery_bmp1, 15, 7, 1); break;
    case 2: display.drawBitmap(128-15, 0, battery_bmp2, 15, 7, 1); break;
    case 3: display.drawBitmap(128-15, 0, battery_bmp3, 15, 7, 1); break;
    case 4: display.drawBitmap(128-15, 0, battery_bmp4, 15, 7, 1); break;
    case 5: display.drawBitmap(128-15, 0, battery_bmp5, 15, 7, 1); break;
  }
  printAt(128-2*6 - 16,0, F("%i"), (int)(vcc / 100));

  display.display();
}

// read double word from EEPROM, give starting address
unsigned long EEPROM_readlong(int address) {
  unsigned long dword = EEPROM_readint(address);
  dword = dword << 16;
  dword = dword | EEPROM_readint(address+2);
  return dword;
}

//write word to EEPROM
void EEPROM_writeint(int address, int value) {
  EEPROM.write(address,highByte(value));
  EEPROM.write(address+1 ,lowByte(value));
}
  
//write long integer into EEPROM
void EEPROM_writelong(int address, unsigned long value) {
  EEPROM_writeint(address+2, word(value));
  value = value >> 16;
  EEPROM_writeint(address, word(value));
}

unsigned int EEPROM_readint(int address) {
  unsigned int word = word(EEPROM.read(address), EEPROM.read(address+1));
  return word;
}

void setup()
{
  byte dst = EEPROM.read(DISTANCE_ADDR);
  unsigned long tsc = EEPROM_readlong(SHOT_COUNT_ADDR);

  if (dst == 255)
  {
    EEPROM.write(DISTANCE_ADDR, Distance);
  }
  else
    Distance = dst;
  if (tsc == 0xFFFFFFFF)
  {
    EEPROM_writelong(SHOT_COUNT_ADDR, total_shot_count);
  }
  else
    total_shot_count = tsc;
    
  // Setting pins
  pinMode(pinA, INPUT_PULLUP);
  pinMode(pinB, INPUT_PULLUP);
  pinMode(buttonA, INPUT_PULLUP);
  pinMode(buttonB, INPUT_PULLUP);
  pinMode(buttonC, INPUT_PULLUP);
  pinMode(buttonD, INPUT_PULLUP);

  // setting up interrupts
  attachInterrupt(0, sensorA, RISING);
  attachInterrupt(1, sensorB, RISING);

  // setting sensors
  pressureSensor.begin();
  
  // setup display and display about info
  init_display();
  
  // setting up measuring stuff
  noInterrupts();
  impulseA = impulseB = 0;
  timestampA = timestampB = 0;
  interrupts();
}

void sensorA()
{
  tmpA = micros();
  if (impulseA++ == 0)
    timestampA = tmpA;
}

void sensorB()
{
  tmpB = micros();
  if (impulseB++ == 0)
    timestampB = tmpB;
}

double read_ambient_pressure(double T)
{
  double P;
  delay(pressureSensor.startPressure(2));
  pressureSensor.getPressure(P, T);
  return P;
}
double read_ambient_temperature()
{
  double T;
  delay(pressureSensor.startTemperature());
  pressureSensor.getTemperature(T);
  return T;
}

void register_shot()
{
    unsigned long duration;
    int i;
    float velocity;
    double T;

    delay(500);
    noInterrupts();
    if (timestampA > timestampB)    
      duration = timestampA - timestampB;
    else
      duration = timestampB - timestampA;
     
    velocity = (1000.0 * Distance) / duration;
    
    for (i = SHOT_STRING_LENGTH - 1; i > 0; i--)
      velocity_values[i] = velocity_values[i - 1];
    velocity_values[0].velocity = velocity;
    shot_count++;
    interrupts();
    
    T = read_ambient_temperature();
    //velocity_values[0].timestamp = millis();
    ambient_temperature = (int)T;
    ambient_pressure = (int)read_ambient_pressure(T);

    delay(500);

    noInterrupts();
    timestampA = timestampB = 0;
    impulseA = impulseB = 0;
    max_aval = 0;
    interrupts();
}

void display_about()
{
  static int last_display_pos = DT_UNKNOWN;
  clear();
  if (buttonEx1.longPress())
  {
    Distance = EEPROM.read(DISTANCE_ADDR);
    display_pos = 0;
  }
  if (buttonEx2.longPress())
  {
    Distance += display_pos;
    EEPROM.write(DISTANCE_ADDR, Distance);
    display_pos = 0;
  }
    
  double T = read_ambient_temperature();
  printAt(0, 0, F("TUBE Chrony"));
  printAt(0, 1, F("c 2017 v1.1"));
  printAt(0, 2, F("d = %d mm"), (int)(Distance + display_pos));
  printAt(0, 3, F("Ta = %i C"), (int)T);
  printAt(0, 4, F("Pa = %i mb"), (int)read_ambient_pressure(T));
  show();
  
  if (last_display_pos == display_pos && !buttonEx1.pushed())
    return;
  last_display_pos = display_pos;
}

void display_shot_string()
{
  char str[10]; 
  int i, j;

  if (shot_count == 0)
  {
    printAt(0, 0, F("Shot String"));
    printAt(7 * 3, 2, F("WAITING"));
    show();
  }

  if (shot_count == last_shot_count)
    return;
  clear();
  last_shot_count = shot_count;
  
  if (display_pos < 0)
    display_pos = 0;
  if (display_pos * 5 + 5 >= SHOT_STRING_LENGTH)
  {
    display_pos = SHOT_STRING_LENGTH / 5 - 1;
  }
  j = display_pos * 5;

  printAt(0, 0, F("(%3i) #: %3i"), j, shot_count);

  // display shot string
  for (i = 0; i < 5; i++)
  {
    dtostrf(velocity_values[i + j].velocity, 3, 1, str);
    printAt(0, 1 + i, F("%s %i %i"), str, 
      ambient_temperature,
      ambient_pressure);
  }
  show();
}

void display_last_shot()
{
  if (shot_count == 0)
  {
    printAt(0, 0, F("Last Shot"));
    printAt(7 * 3, 2, F("WAITING"));
    show();
  }

  if (shot_count == last_shot_count)
    return;
  clear();
  
  last_shot_count = shot_count;

  if (display_pos < 0)
    display_pos = 0;
  if (display_pos + 1 >= shot_count)
  {
    display_pos = shot_count - 1;
  }

  printAt(0, 0, F("(%3i) #: %3i"), display_pos, shot_count);

  dtostrf(velocity_values[display_pos].velocity, 3, 1, buf);buf[5] = '\0';
  printAt(0,1, F("V= %smps"), buf);
  printAt(0,2, F("Ta= %i C"), ambient_temperature);
  printAt(0,3, F("Pa= %i mb"), ambient_pressure);
  show();
}

void display_graph()
{
  float mi = 1000, ma = 0;
  int x, y, x1, y1, i;
  
  if (shot_count < 2)
  {
    printAt(0, 0, F("Shot Graph"));
    printAt(7 * 3, 2, F("WAITING"));
    show();
  }

  if (shot_count == last_shot_count)
    return;
  clear();
  last_shot_count = shot_count;
  if (display_pos < 0)
    display_pos = 0;  
  display_pos = display_pos % 5;  

  for (i = 0; i < shot_count; i++)
  {
    mi = min(mi, velocity_values[i].velocity); ma = max(ma, velocity_values[i].velocity);
  }

  printAt(0, 0, F("%i: %i-%i"), shot_count, (int)mi, (int)ma);

  x = 0;
  y = 0;
  for (i = 0; i < shot_count; i++)
  {
    x1 = (int)i * (128.0f / (float)shot_count);
    y1 = (int)54.0f*(1.0f - (velocity_values[i].velocity - mi) / (ma - mi));
    display.drawLine(128 - x, 10 + y, 128 - x1, 10 + y1, WHITE);
    x = x1;
    y = y1;
    //put_pixel(shot_count - i, mi, ma, velocity_values[i].velocity);
  }
  show();
}

/*void display_transfer()
{
  clear();
  printAt(0, 0, F("TRANSFER"));
  printAt(0, 1, F("press btn 3"));
  printAt(0, 2, F("to send"));
  show();
  if (buttonEx1.pushed())
  {
    SoftwareSerial sender(8, A0);
    sender.begin(9600);
    
    sender.println(F("# TUBE Crony (c) 2014 v1.0"));
    sender.println(F("# Shot count ")); 
    sender.println(F("# velocity; timestamp ")); 
    sender.println(shot_count);
    for (int i = 0; i < shot_count; i++)
    {
      // send
      dtostrf(velocity_values[i].velocity, 4, 2, buf);
      sender.print(buf); sender.print(F("; "));
      //sender.print(velocity_values[i].timestamp); 
      sender.println();
      printAt(0, 4, F("Out: %i/%i"), i, shot_count);
      show();
    }
  }
  show();
} */

void display_info()
{
  static DISPLAY_TYPE last_display_type = DT_UNKNOWN;
#ifdef DEBUG
  int a, b;
  a = digitalRead(pinA);
  b = digitalRead(pinB);

  printAt(42, 4, "%i, %i ", a, b);
  printAt(42, 5, "%i, %i  ", impulseA, impulseB);
#endif

  if (last_display_type != display_type)
  {
    last_display_type = display_type;
    display_pos = 0;
    last_shot_count = 0;
    clear();
  }

  switch (display_type)
  {
  case DT_ABOUT:
    display_about();
    return;
  case DT_ABOUT_INIT:
    display_about();
    delay(5000);
    display_type = DT_LAST_SHOT;
    return;
  case DT_SHOT_STRING:
    display_shot_string();
    return;
  case DT_LAST_SHOT:
    display_last_shot();
    return;
  case DT_SHOT_GRAPH:
    display_graph();
    return;
//  case DT_TRANSFER:
//    display_transfer();
//    return;
  }
}

void handle_buttons()
{
  buttonUp.poll();
  buttonDown.poll();
  buttonEx1.poll();
  buttonEx2.poll();

  if (buttonDown.pushed()) 
  {
    display_pos--;
    last_shot_count = 0;
  }
  if (buttonDown.longPress()) display_type = (DISPLAY_TYPE)(display_type == 0 ? 0 : (int)display_type - 1);

  if (buttonUp.pushed()) 
  {
    display_pos++;
    last_shot_count = 0;
  }
  if (buttonUp.longPress()) 
  {
    if ((DISPLAY_TYPE)((int)display_type + 1) != DT_NULL) display_type = (DISPLAY_TYPE)((int)display_type + 1);
  }

  if (buttonEx2.longPress())
  {
    // perform reset
    for (int i = 0; i < SHOT_STRING_LENGTH; i++)
    {
      velocity_values[i].velocity = 0;
      //velocity_values[i].timestamp = 0;
    }
    shot_count = 0;
    last_shot_count = 0;
    display_pos = 0;
    display_type = DT_ABOUT_INIT;
    clear();
  }
}

void loop()
{
  unsigned long now = micros();

  if (impulseA && impulseB)
  {
    register_shot();
    EEPROM_writelong(SHOT_COUNT_ADDR, total_shot_count++);
    timestamp = 0;    
  }
  else if ((impulseA | impulseB) && (impulseA ^ impulseB))
  {
    now = millis();
    if (timestamp == 0)
    {
      timestamp = now;
    }
    else if (now - timestamp > 500)
    {
      noInterrupts();
      timestampA = timestampB = 0;
      impulseA = impulseB = 0;
      interrupts();
      timestamp = 0;
    }
  }
  
  handle_buttons();  
  display_info();
}

