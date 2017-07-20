#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Switch.h>
#include <Fonts/FreeSans12pt7b.h>

#define OLED_RESET 10
Adafruit_SSD1306 display(OLED_RESET);
Switch mode = Switch(7, INPUT_PULLUP, LOW, 1);

void setup(void) {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.clearDisplay();
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  display.setTextColor(WHITE);
  display.setFont(&FreeSans12pt7b);
}

#define READINGS_NUM 64
#define RR_NUM 64
#define MAX_RR_VAL 1400.0
#define MIN_RR_VAL 100.0
#define RR_VAL_SCALE (64.0 / (MAX_RR_VAL - MIN_RR_VAL))

short vals[READINGS_NUM] = {};
short rr[RR_NUM] = {};
short rr_count = 0, rr_index = 0;
long last_time = 0, now = 0;
bool last_min = false;

enum show_enum {
  VARIABILITY = 0,
  PULSE,
  PULSE_VARIABILITY,
  HR,
};
show_enum show = PULSE;

void loop(void) {  
  float acc = 0;
  short mi = 2048, ma = 0;

  mode.poll();
  if (mode.pushed())
  {
    show = (show_enum)(show == HR ? VARIABILITY : (int)show + 1);
  }
  
  for (int i = 0; i < READINGS_NUM - 1; i++)
    vals[i] = vals[i + 1];
  vals[READINGS_NUM -1] = analogRead(A0);
  now = micros() / 1000;

  for (int i = 0; i < READINGS_NUM; i++)
  {
    if (vals[i] > ma)
      ma = vals[i];
    if (vals[i] < mi)
      mi = vals[i];
  }
  short mid = mi + (ma - mi) / 2;
  bool valid = abs(mid - 512) < 10 && (ma - mi) < 20 && (ma - mi) > 5;
  //Serial.print(vals[READINGS_NUM -1]);
  //Serial.print(' ');
  //Serial.print(mi);
  //Serial.print(' ');
  //Serial.print(ma);
  //Serial.print(' ');
  //Serial.println(mid);
  if (vals[READINGS_NUM - 3] >= vals[READINGS_NUM - 2] && vals[READINGS_NUM - 1] >= vals[READINGS_NUM - 2] && vals[READINGS_NUM - 2] < mid && valid)
  {
    last_min = true;
    digitalWrite(13, LOW);
  }
  
  if (vals[READINGS_NUM - 3] <= vals[READINGS_NUM - 2] && vals[READINGS_NUM - 1] <= vals[READINGS_NUM - 2] && vals[READINGS_NUM - 2] > mid && last_min && valid)
  {
    short rr_now = now - last_time;
    //Serial.println(ma);
    if (rr_now <= MAX_RR_VAL && rr_now >= MIN_RR_VAL)
    {
      rr[rr_index] = rr_now;
      rr_index = (rr_index + 1)%RR_NUM;
      if (rr_index > rr_count)
        rr_count = rr_index;
      //Serial.println(64 + 64 * rr_now / 1400.0);
      digitalWrite(13, HIGH);
    }
    last_min = false;
    last_time = now;
  }
  else
  {
    //Serial.println(mi);
  }

  display.clearDisplay();         // clear the internal memory

  switch (show)
  {
    case HR:
      for (int i = 0; i < rr_count; i++)
        acc += rr[i];
      acc /= (float)rr_count;
      display.setCursor(0, 40);
      display.print((int)(60000.0 / acc));
      break;
    case PULSE:
      for (int i = 0; i < READINGS_NUM - 1; i++)
        display.drawLine(i, 64 -(int)(64 * (vals[i] - mi) / (ma - mi)), 
                      i+1, 64 - (int)(64 * (vals[i+1] - mi) / (ma - mi)), WHITE);
      break;
    case VARIABILITY:
      for (int i = 0; i < min(rr_count, READINGS_NUM); i++)
      {
        int index1 = (rr_index - i)%RR_NUM;
        int index2 = (rr_index - i-1)%RR_NUM;
        if (index1 < 0)
          index1 += rr_count;
        if (index2 < 0)
          index2 += rr_count;
        display.drawLine(64 - i, 64 - (rr[index1] - MIN_RR_VAL) * RR_VAL_SCALE, 
                      64 - i-1, 64 - (rr[index2] - MIN_RR_VAL) * RR_VAL_SCALE, WHITE);
      }
      break;
    case PULSE_VARIABILITY:
      for (int i = 0; i < READINGS_NUM - 1; i++)
        display.drawLine(i, 64 - (int)(64 * (vals[i] - mi) / (ma - mi)), 
                      i+1, 64 - (int)(64 * (vals[i+1] - mi) / (ma - mi)), WHITE);
      for (int i = 0; i < min(rr_count, READINGS_NUM); i++)
      {
        int index1 = (rr_index - i)%RR_NUM;
        int index2 = (rr_index - i-1)%RR_NUM;
        if (index1 < 0)
          index1 += rr_count;
        if (index2 < 0)
          index2 += rr_count;
        display.drawLine(128 - i, 64 - (rr[index1] - MIN_RR_VAL) * RR_VAL_SCALE, 
                      128 - i-1, 64 - (rr[index2] - MIN_RR_VAL) * RR_VAL_SCALE, WHITE);
      }
      break;
  }

  if (show != PULSE_VARIABILITY)
  {
    display.drawFastHLine(64, 63, 64, WHITE);
    display.drawFastVLine(64, 0, 64, WHITE);
    for (int i = 0; i < rr_count - 1; i++)
    {
       display.drawPixel(64 + (rr[i] - MIN_RR_VAL) * RR_VAL_SCALE, 64 - (rr[i+1] - MIN_RR_VAL) * RR_VAL_SCALE, WHITE);
    }
  }

  display.display();          // transfer internal memory to the display
}

