#include <EEPROM.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <Wire.h>
#include <DallasTemperature.h>
#include <SFE_BMP180.h>
#include <nokia5110.h>
#include <Switch.h>

#define PIN_RESET 12  // LCD RST .... Pin 1
                     // LCD Gnd .... Pin 2
#define PIN_SCE   11  // LCD CS  .... Pin 3
#define PIN_SCLK 13  // LCD SPIClk . Pin 4
#define PIN_DC    10  // LCD Dat/Com. Pin 5
#define PIN_SDIN  9  // LCD SPIDat . Pin 6
                     // LCD Vlcd ... Pin 7
                     // LCD Vcc .... Pin 8

#define pinA 2
#define pinB 3

#define buttonA 4
#define buttonB 5
#define buttonC 6
#define buttonD 7

// Define various ADC prescaler
#define PS_16 = (1 << ADPS2);
#define PS_32 = (1 << ADPS2) | (1 << ADPS0);
#define PS_64 = (1 << ADPS2) | (1 << ADPS1);
#define PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

#define SHOT_STRING_LENGTH 60
#define DEFAULT_DISTANCE 137
#define DISTANCE_ADDR  0
#define SHOT_COUNT_ADDR  1

typedef struct shot_info_struct
{
  float velocity;
  int ambient_pressure;
  byte ambient_temperature;
  byte co2_temperature;
  int co2_pressure;
  unsigned long timestamp;
} shot_info_t;

enum DISPLAY_TYPE
{
  DT_SHOT_STRING = 0,
  DT_LAST_SHOT,
  DT_SHOT_GRAPH,
//  DT_SHOT_PREDICT,
  DT_ABOUT,
  DT_TRANSFER,
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

DISPLAY_TYPE display_type = DT_LAST_SHOT;
int display_pos = 0;
int display_pos1 = 0;
unsigned long total_shot_count = 0;
int shot_count = 0;
int last_shot_count = 0;
shot_info_t velocity_values[SHOT_STRING_LENGTH] = {0, };

Nokia5110 lcd(12,11,13,10,9);
Switch buttonUp = Switch(buttonA, INPUT_PULLUP, LOW, 3, 1000, 50);
Switch buttonDown = Switch(buttonB, INPUT_PULLUP, LOW, 3, 1000, 50);
Switch buttonEx1 = Switch(buttonC, INPUT_PULLUP, LOW, 3, 1000, 50);
Switch buttonEx2 = Switch(buttonD, INPUT_PULLUP, LOW, 3, 1000, 50);
OneWire tempSensorBus(A1);
DallasTemperature tempSensor(&tempSensorBus);
SFE_BMP180 pressureSensor;

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

  // ADC for co2 pressure in the moderator
  ADMUX = (1 << REFS0) | (1 << ADLAR);
  ADCSRA = 0xAC;           // AD-converter on, interrupt enabled, prescaler = 16
  ADCSRB = 0x40;           // AD channels MUX on, free running mode
  bitWrite(ADCSRA, 6, 1);  // Start the conversion by setting bit 6 (=ADSC) in ADCSRA

  // setting sensors
  pressureSensor.begin();
  tempSensor.begin();
  
  // setup display and display about info
  lcd.init();
  display_about();

  delay(700);
  
  // setting up measuring stuff
  noInterrupts();
  impulseA = impulseB = 0;
  timestampA = timestampB = 0;
  interrupts();

  lcd.clear(0);
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

/*** Interrupt routine ADC ready ***/
ISR(ADC_vect) {
  byte aval = ADCH;
  if (aval > max_aval)
    max_aval = aval;
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
int read_co2_temperature()
{
  tempSensor.requestTemperatures();
  return tempSensor.getTempCByIndex(0);
}
int read_co2_pressure()
{
  int ma = max_aval;
  max_aval = 0;
  return ma;
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
    velocity_values[0].ambient_temperature = (int)T;
    velocity_values[0].ambient_pressure = (int)read_ambient_pressure(T);
    velocity_values[0].co2_temperature = read_co2_temperature();
    velocity_values[0].co2_pressure = read_co2_pressure();
    velocity_values[0].timestamp = millis();

    delay(500);

    noInterrupts();
    timestampA = timestampB = 0;
    impulseA = impulseB = 0;
    max_aval = 0;
    interrupts();
}

void display_about()
{
  static int last_display_pos = -1;
  
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
  lcd.printAt(0, 0, F("TUBE Chrony"));
  lcd.printAt(0, 1, F("c 2014 v1.0"));
  lcd.printAt(0, 2, F("d = %d mm"), (int)(Distance + display_pos));
  lcd.printAt(0, 3, F("Ta = %i C"), (int)T);
  lcd.printAt(0, 4, F("Pa = %i mb"), (int)read_ambient_pressure(T));

  if (last_display_pos == display_pos && !buttonEx1.pushed())
    return;
  last_display_pos = display_pos;

  lcd.printAt(0, 5, F("Tc = %i C  "), read_co2_temperature());
}

void display_shot_string()
{
  char str[10]; 
  int i, j;

  if (shot_count == 0)
  {
    lcd.printAt(0, 0, F("Shot String"));
    lcd.printAt(7 * 3, 2, F("WAITING"));
  }

  if (shot_count == last_shot_count)
    return;
  lcd.clear(0);
  last_shot_count = shot_count;
  
  if (display_pos < 0)
    display_pos = 0;
  if (display_pos * 5 + 5 >= SHOT_STRING_LENGTH)
  {
    display_pos = SHOT_STRING_LENGTH / 5 - 1;
  }
  j = display_pos * 5;

  lcd.printAt(0, 0, F("(%3i) #: %3i"), j, shot_count);

  // display shot string
  for (i = 0; i < 5; i++)
  {
    dtostrf(velocity_values[i + j].velocity, 3, 1, str);
    lcd.printAt(0, 1 + i, F("%s %i %i"), str, 
      velocity_values[i + j].co2_temperature, 
      velocity_values[i + j].ambient_pressure);
  }
}

void display_last_shot()
{
  char str[10]; 

  if (shot_count == 0)
  {
    lcd.printAt(0, 0, F("Last Shot"));
    lcd.printAt(7 * 3, 2, F("WAITING"));
  }

  if (shot_count == last_shot_count)
    return;
  
  lcd.clear(0);
  last_shot_count = shot_count;

  if (display_pos < 0)
    display_pos = 0;
  if (display_pos + 1 >= shot_count)
  {
    display_pos = shot_count - 1;
  }

  lcd.printAt(0, 0, F("(%3i) #: %3i"), display_pos, shot_count);

  dtostrf(velocity_values[display_pos].velocity, 3, 1, str);str[5] = '\0';
  lcd.printAt(0,1, F("V= %smps"), str);
  lcd.printAt(0,2, F("Ta= %i C"), velocity_values[display_pos].ambient_temperature);
  lcd.printAt(0,3, F("Pa= %i mb"), velocity_values[display_pos].ambient_pressure);
  lcd.printAt(0,4, F("Tc= %i C"), velocity_values[display_pos].co2_temperature);
  lcd.printAt(0,5, F("Pc= %i mb"), velocity_values[display_pos].co2_pressure);
}

void put_pixel(int i, float mi, float ma, float v)
{
  int y = 4 * 8 - (int)((4 * 8) * ((v - mi) / (ma - mi)));
  int p = y / 8;
  int b = y % 8;
  byte data = 1<<b;
  lcd.setPos(i + 5, p + 1);
  lcd.putSymbol(&data, 1);
}

void display_graph()
{
  float mi = 1000, ma = 0;
  int i;
  char* s[] = {"V", "Pa", "Ta", "Pc", "Tc"};
  //char smi[6]; sma[6];
  
  if (shot_count == 0)
  {
    lcd.printAt(0, 0, F("Shot Graph"));
    lcd.printAt(7 * 3, 2, F("WAITING"));
  }

  if (shot_count == last_shot_count)
    return;
  lcd.clear(0);
  last_shot_count = shot_count;
  if (display_pos < 0)
    display_pos = 0;  
  display_pos = display_pos % 5;  

  for (i = 0; i < shot_count; i++)
  {
    switch (display_pos)
    {
      case 0: mi = min(mi, velocity_values[i].velocity); ma = max(ma, velocity_values[i].velocity); break;
      case 1: mi = min(mi, velocity_values[i].ambient_pressure); ma = max(ma, velocity_values[i].ambient_pressure); break;
      case 2: mi = min(mi, velocity_values[i].ambient_temperature); ma = max(ma, velocity_values[i].ambient_temperature); break;
      case 3: mi = min(mi, velocity_values[i].co2_pressure); ma = max(ma, velocity_values[i].co2_pressure); break;
      case 4: mi = min(mi, velocity_values[i].co2_temperature); ma = max(ma, velocity_values[i].co2_temperature); break;
    }
  }

  lcd.printAt(0, 0, F("%s %i %i-%i"), s[display_pos], shot_count, (int)mi, (int)ma);

  for (i = 0; i < shot_count; i++)
  {
    switch (display_pos)
    {
      case 0: put_pixel(shot_count - i, mi, ma, velocity_values[i].velocity); break;
      case 1: put_pixel(shot_count - i, mi, ma, velocity_values[i].ambient_pressure); break;
      case 2: put_pixel(shot_count - i, mi, ma, velocity_values[i].ambient_temperature); break;
      case 3: put_pixel(shot_count - i, mi, ma, velocity_values[i].co2_pressure); break;
      case 4: put_pixel(shot_count - i, mi, ma, velocity_values[i].co2_temperature); break;
    }
  }
}

void display_transfer()
{
  char str[10];
  lcd.printAt(0, 0, F("TRANSFER"));
  lcd.printAt(0, 1, F("press btn 3"));
  lcd.printAt(0, 2, F("to send"));

  if (buttonEx1.pushed())
  {
    SoftwareSerial sender(8, A0);
    sender.begin(9600);
    
    sender.println(F("# TUBE Crony (c) 2014 v1.0"));
    sender.println(F("# Shot count ")); 
    sender.println(F("# velocity; Pamb; Tamb; Pco2; Tco2; timestamp ")); 
    sender.println(shot_count);
    for (int i = 0; i < shot_count; i++)
    {
      // send
      dtostrf(velocity_values[i].velocity, 4, 2, str);
      sender.print(str); sender.print(F("; "));
      sender.print(velocity_values[i].ambient_pressure); sender.print(F("; "));
      sender.print(velocity_values[i].ambient_temperature); sender.print(F("; "));
      sender.print(velocity_values[i].co2_pressure); sender.print(F("; "));
      sender.print(velocity_values[i].co2_temperature); sender.print(F("; "));
      sender.print(velocity_values[i].timestamp); sender.println();
      lcd.printAt(0, 4, F("Out: %i/%i"), i, shot_count);
    }
  }
}

void display_info()
{
  static DISPLAY_TYPE last_display_type = DT_ABOUT;
#ifdef DEBUG
  char str[10];  
  int a, b;
  a = digitalRead(pinA);
  b = digitalRead(pinB);

  lcd.setPos(42, 4);
  sprintf(str, "%i, %i ", a, b);
  lcd.putStr(str);
  lcd.setPos(42, 5);
  sprintf(str, "%i, %i  ", impulseA, impulseB);
  lcd.putStr(str);
#endif

  if (last_display_type != display_type)
  {
    last_display_type = display_type;
    display_pos = 0;
    last_shot_count = 0;
    lcd.clear(0);
  }

  switch (display_type)
  {
  case DT_ABOUT:
    display_about();
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
  case DT_TRANSFER:
    display_transfer();
    return;
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
      velocity_values[i].timestamp = 0;
      velocity_values[i].ambient_pressure = 0;
      velocity_values[i].co2_temperature = 0;
      velocity_values[i].co2_pressure = 0;
      velocity_values[i].co2_temperature = 0;
    }
    shot_count = 0;
    last_shot_count = 0;
    display_about();
    delay(700);
    display_type = DT_LAST_SHOT;
    display_pos = 0;
    lcd.clear(0);
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

