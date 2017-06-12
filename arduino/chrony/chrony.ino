#define pinLED 9
#define pinA 2
#define pinB 3
#define Distance (1000000.0 * 0.26)

volatile unsigned long timestampA = 0;
volatile unsigned long timestampB = 0;
volatile unsigned long tmpA = 0;
volatile unsigned long tmpB = 0;
volatile byte impulseA = 0;
volatile byte impulseB = 0;

byte seven_seg_pins[] = {4, 5, 6, 7, 8, 10, 11, 12};
byte seven_seg_cathodes[] = {13, A0, A1, A2};
byte seven_seg_digits[10][7] = { 
  { 1,1,1,1,1,1,0 },  // = 0
  { 0,1,1,0,0,0,0 },  // = 1
  { 1,1,0,1,1,0,1 },  // = 2
  { 1,1,1,1,0,0,1 },  // = 3
  { 0,1,1,0,0,1,1 },  // = 4
  { 1,0,1,1,0,1,1 },  // = 5
  { 1,0,1,1,1,1,1 },  // = 6
  { 1,1,1,0,0,0,0 },  // = 7
  { 1,1,1,1,1,1,1 },  // = 8
  { 1,1,1,0,0,1,1 }   // = 9
};
byte seven_seg_values[4] = {0, };
byte shot_count = 0;
float velocity_values[5] = {0, };

void setup() {
  int i;
  
  for (i = 0; i < 8; i++)
    pinMode(seven_seg_pins[i], OUTPUT);
  for (i = 0; i < 4; i++)
    pinMode(seven_seg_cathodes[i], OUTPUT);
  displayWrite();
  
//  pinMode (pinLED, OUTPUT); 
//  TCCR1A = 0;
//  TCCR1B = 0;
//  TCNT1  = 0;
//  OCR1A = 26;   // fclk / (2 * foc) - 1
//  TCCR1A |= (1 << COM1A0);   // Toggle OC1A on Compare Match.
//  TCCR1B |= (1 << WGM12);    // CTC mode
//  TCCR1B |= (1 << CS10);     // clock on, no pre-scaler

  attachInterrupt(0, sensorA, RISING);
  attachInterrupt(1, sensorB, RISING);
  delay(500);
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

void to_display(float velocity, byte * chars)
{
    char str[10] = "";
    int i, j, len;
 
    dtostrf(velocity, 4, 3, str);
    len = strlen(str);
    j = 0;
    for (i = 0; i < 4; i++)
      chars[i] = 0;
    for (i = 0; i < len && j < 4; i++)
      if (str[i] == '.')
        chars[j - 1] |= 0x10;
      else
        chars[j++] |= str[i] - '0';
}

void loop() {
  unsigned long now = micros();

  //seven_seg_values[0] = impulseA;
  //seven_seg_values[1] = impulseB;

  if (impulseA && impulseB != 0)
  {
/**/    unsigned long duration;
    int i;
    float velocity;

    noInterrupts();
    if (timestampA > timestampB)    
      duration = timestampA - timestampB;
    else
      duration = timestampB - timestampA;
     
    velocity = Distance / duration;
    
    to_display(velocity, seven_seg_values);
    for (i = 4; i > 0; i--)
      velocity_values[i] = velocity_values[i - 1];
    velocity_values[0] = velocity;
    shot_count++;
    interrupts();
/**/
    delay(500);
    noInterrupts();
    timestampA = timestampB = 0;
    impulseA = impulseB = 0;
    interrupts();
  }
  else if ((timestampA != 0 && (now - timestampA > 500000)) || 
           (timestampB != 0 && (now - timestampB > 500000)))
  {
    noInterrupts();
    timestampA = timestampB = 0;
    impulseA = impulseB = 0;
    interrupts();
  }
  
  {
    byte n = 0;
    byte i;
    float velocity = 0;
    n = (!!digitalRead(A3));
    n |= (!!digitalRead(A4)) << 1;
    n |= (!!digitalRead(A5)) << 2;
    
    switch (n)
    {
      case 0:
        break;
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
        to_display(velocity_values[n - 1], seven_seg_values);
        break;
      case 6:
        for (i = 0; i < 5 && i < shot_count; i++)
          velocity += velocity_values[i];
        to_display(velocity / (float)i, seven_seg_values);
        break;
      case 7:
        to_display(shot_count, seven_seg_values);
        break;
    }
  }
  
  displayWrite();
}

void displayWrite()
{
  byte i, j;
  for (i = 0; i < 4; i++)
  {
    for (j = 0; j < 8; j++)
      digitalWrite(seven_seg_pins[j], LOW);
    digitalWrite(seven_seg_cathodes[i], HIGH);
    sevenSegWrite(seven_seg_values[i]);
    delayMicroseconds(64);
    digitalWrite(seven_seg_cathodes[i], LOW);
  }
}

void sevenSegWrite(byte digit) {
  byte d = digit & 0x0F;
  for (byte segCount = 0; segCount < 7; ++segCount) {
    digitalWrite(seven_seg_pins[segCount], seven_seg_digits[d][segCount]);
  }
  digitalWrite(seven_seg_pins[7], digit & 0xF0 ? HIGH : LOW);
}

