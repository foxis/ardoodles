#include <Arduino.h>
#include <U8x8lib.h>
#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>
#include <Switch.h>
#include <MenuSystem.h>

///////////////////////////////////////////
//
///////////////////////////////////////////
#define COOLER_A_PIN 7
#define COOLER_B_PIN 8
#define MOTOR_A_PIN 6
#define MOTOR_B_PIN 9
typedef enum SENDOR_enum {
  HOT_SIDE_A = 0,
  HOT_SIDE_B,
  COLD_SIDE_A,
  COLD_SIDE_B,
  SPECIMEN_A,
  SPECIMEN_B,
  SENSORS_NUM
} SENSOR_t;

///////////////////////////////////////////
//
///////////////////////////////////////////
void nav_menu();
void perform_controls();
void start();
void stop();

void on_manual(MenuItem * );
void on_restart(MenuItem * );
void on_save(MenuItem * );
void on_load(MenuItem * );
void on_temp_cold(MenuItem * );
void on_temp_specimen(MenuItem * );
void on_temp_P(MenuItem * );
void on_temp_I(MenuItem * );
void on_temp_D(MenuItem * );
void on_temp_ctrl_cold(MenuItem * );
void on_temp_ctrl_specimen(MenuItem * );
void on_pump_in(MenuItem * );
void on_pump_out(MenuItem * );

void handler_dashboard();

///////////////////////////////////////////
//
///////////////////////////////////////////
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);

// Menu variables
// renderer

class MyRenderer : public MenuComponentRenderer
{
public:
    virtual void render(Menu const& menu) const
    {
      u8x8.setCursor(0,0);
      u8x8.print("== ");
      u8x8.print(menu.get_name());
      u8x8.print(" ==");
      for (int i = 0; i < menu.get_num_components(); ++i)
      {
        MenuComponent const* cp_m_comp = menu.get_menu_component(i);
        u8x8.setCursor(0,1 + i);
        if (cp_m_comp->is_current())
          u8x8.inverse();
        cp_m_comp->render(*this);
        if (cp_m_comp->is_current())
          u8x8.noInverse();
      }  
    }

    virtual void render_menu_item(MenuItem const& menu_item) const
    {
      u8x8.print(' ');
      u8x8.print(menu_item.get_name());
    }

    virtual void render_back_menu_item(BackMenuItem const& menu_item) const
    {
      u8x8.print(' ');
        u8x8.print(menu_item.get_name());
    }

    virtual void render_numeric_menu_item(NumericMenuItem const& menu_item) const
    {
      u8x8.print(' ');
      u8x8.print(menu_item.get_name());
      u8x8.print(' ');
      u8x8.print(menu_item.get_value());
    }

    virtual void render_menu(Menu const& menu) const
    {
      u8x8.print('+');
      u8x8.print(menu.get_name());
    }
};
MyRenderer my_renderer; 

MenuSystem ms(my_renderer);
Menu m_ctrl("Control");
MenuItem m_ctrl_manual("Manual", &on_manual);
MenuItem m_ctrl_reset("Reset", &on_restart);
MenuItem m_ctrl_save("Save", &on_save);
MenuItem m_ctrl_load("Load", &on_load);
Menu m_temp("Temperature");
NumericMenuItem m_temp_cold("coolant", &on_temp_cold, -15, -30, 25);
NumericMenuItem m_temp_specimen("specimen", &on_temp_specimen, -5, -30, 25);
NumericMenuItem m_temp_p("P", &on_temp_P, 15, 0, 100);
NumericMenuItem m_temp_i("I", &on_temp_I, -15, 0, 100);
NumericMenuItem m_temp_d("D", &on_temp_D, -15, 0, 100);
Menu m_temp_ctrl("Control for");
MenuItem m_temp_ctrl_cold("", &on_temp_ctrl_cold);
MenuItem m_temp_ctrl_specimen("", &on_temp_ctrl_specimen);
Menu m_pump("Pump");
NumericMenuItem m_pump_input("Input", &on_pump_in, 50, 0, 100);
NumericMenuItem m_pump_output("Output", &on_pump_out, 50, 0, 100);

typedef void (*handler_t)();

unsigned long menu_start = 0;
handler_t default_handler = handler_dashboard;
handler_t last_handler = handler_dashboard;

int knob_a_pos = 0;
int knob_b_pos = 0;
int last_knob_a_pos = 0;
int last_knob_b_pos = 0;

Encoder knobA(2, 4);
Encoder knobB(3, 5);
Switch ESC(1);
Switch ENT(0);

///////////////////////////////////////////
//
///////////////////////////////////////////
enum {
  MANUAL,
  IDLE,
  COOLING,
  PUMPING,
} current_state = IDLE;

float target_cold = -15;
float target_specimen = -10;
float cold_PID[3] = {10, 0, 0};
float specimen_PID[3] = {10, 0, 0};
SENSOR_t target = COLD_SIDE_A;
float temp[SENSORS_NUM] = {};
byte pump_input = 50;
byte pump_output = 100;
bool cooler_a = false, cooler_b = false;


///////////////////////////////////////////
//
///////////////////////////////////////////
void update_menus()
{
  m_temp_cold.set_value(target_cold);
  m_temp_specimen.set_value(target_specimen);
  if (target == COLD_SIDE_A)
  {
    m_temp_p.set_value(cold_PID[0]);
    m_temp_i.set_value(cold_PID[1]);
    m_temp_d.set_value(cold_PID[2]);
    m_temp_ctrl_cold.set_name("Coolant  X");
    m_temp_ctrl_specimen.set_name("Specimen  ");
  }
  else
  {
    m_temp_p.set_value(specimen_PID[0]);
    m_temp_i.set_value(specimen_PID[1]);
    m_temp_d.set_value(specimen_PID[2]);
    m_temp_ctrl_cold.set_name("Coolant   ");
    m_temp_ctrl_specimen.set_name("Specimen X");
  }
  m_pump_input.set_value(pump_input);
  m_pump_output.set_value(pump_output);
}

void read_defaults()
{
  // TODO
  update_menus();
}

void write_defaults()
{
  // TODO
}

void setup(void)
{
  pinMode(MOTOR_A_PIN, OUTPUT);
  pinMode(MOTOR_B_PIN, OUTPUT);
  pinMode(COOLER_A_PIN, OUTPUT);
  pinMode(COOLER_B_PIN, OUTPUT);
  
  digitalWrite(COOLER_A_PIN, LOW);
  digitalWrite(COOLER_B_PIN, LOW);

  analogWrite(MOTOR_A_PIN, 0);
  analogWrite(MOTOR_B_PIN, 0);
  
  u8x8.begin();
  u8x8.setPowerSave(0);
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  ms.get_root_menu().add_menu(&m_ctrl);
  m_ctrl.add_item(&m_ctrl_manual);
  m_ctrl.add_item(&m_ctrl_reset);
  m_ctrl.add_item(&m_ctrl_save);
  m_ctrl.add_item(&m_ctrl_load);
  ms.get_root_menu().add_menu(&m_temp);
  m_temp.add_item(&m_temp_cold);
  m_temp.add_item(&m_temp_specimen);
  m_temp.add_item(&m_temp_p);
  m_temp.add_item(&m_temp_i);
  m_temp.add_item(&m_temp_d);
  m_temp.add_menu(&m_temp_ctrl);
  m_temp_ctrl.add_item(&m_temp_ctrl_cold);
  m_temp_ctrl.add_item(&m_temp_ctrl_specimen);
  ms.get_root_menu().add_menu(&m_pump);
  m_pump.add_item(&m_pump_input);
  m_pump.add_item(&m_pump_output);
  
  // TODO: read defaults from eeprom
  read_defaults();
}

float read_temperature(byte pin)
{
  static byte pins[6] = {A0, A1, A2, A3, A6, A7};
  float average = 0;
  byte _analogPin = pins[pin];

#define NOMINAL_RESISTANCE  10000.0
#define SERIES_RESISTANCE   10000.0
#define BETA_COEFFICIENT    3950.0
#define NOMINAL_TEMPERATURE 25.0
#define ADC_SIZE_8_BIT              8
#define ADC_SIZE_10_BIT             10
#define ADC_SIZE_12_BIT             12
#define ADC_SIZE_16_BIT             16
#define ADC_SIZE                    ADC_SIZE_10_BIT
  for (uint8_t i = 0; i < 3; i++) {
      average += analogRead(_analogPin);
      delay(5);
  }

  average /= 3.0f;

  // convert the value to resistance
  average = SERIES_RESISTANCE * ((ADC_SIZE * ADC_SIZE - 1) / average - 1.0);

  // Steinhart–Hart equation, based on https://learn.adafruit.com/thermistor/using-a-thermistor
  float steinhart = (log(average / NOMINAL_RESISTANCE)) / BETA_COEFFICIENT;
  steinhart += 1.0 / (NOMINAL_TEMPERATURE + 273.15);
  steinhart = 1.0 / steinhart; // invert
  steinhart -= 273.15; // convert to celsius

  return steinhart;
}

void loop(void)
{
  static byte temperature_read_pos = 0;
  knob_b_pos = knobB.read() >> 2;
  knob_a_pos = knobA.read() >> 2;

  ESC.poll();
  ENT.poll();
  
  temp[temperature_read_pos++] = read_temperature(temperature_read_pos);
  if (temperature_read_pos >= SENSORS_NUM)
    temperature_read_pos = 0;
  //temp[HOT_SIDE_B] = read_temperature(A1);
  //temp[COLD_SIDE_A] = read_temperature(A2);
  //temp[COLD_SIDE_A] = read_temperature(A3);
  //temp[SPECIMEN_A] = read_temperature(A6);
  //temp[SPECIMEN_B] = read_temperature(A7);

  perform_controls();

  if (default_handler != last_handler)
  {
    if (!ENT.on() && !ESC.on())
    {
      last_handler = default_handler;
      knob_a_pos = knob_b_pos = 0;
      knobA.write(0);
      knobB.write(0);
      u8x8.clear();

      default_handler();
    }
  }
  else
    default_handler();
    
  last_knob_a_pos = knob_a_pos;
  last_knob_b_pos = knob_b_pos;
}

///////////////////////////////////////////
//
///////////////////////////////////////////
void handler_common()
{
  if (ESC.longPress() && ENT.longPress())
  {
    default_handler = handler_menu;
    menu_start = millis();
    ms.reset();
  }
  else if (ESC.longPress())
  {
    u8x8.clear();
    current_state = IDLE;
  } else if (ENT.longPress())
  {
    u8x8.clear();
    current_state = COOLING;
  }

}

void draw_state(int what)
{
  byte i = 0;
  if (what == 0) u8x8.inverse();
  switch (current_state)
  {
    case IDLE:
      u8x8.drawString(0,i++,"---- IDLE -----");
      break;
    case COOLING:
      u8x8.drawString(0,i++,"--- COOLING ---");
      break;
    case PUMPING:
      u8x8.drawString(0,i++,"--- PUMPING ---");
      break;
    case MANUAL:
      u8x8.drawString(0,i++,"---- MANUAL ---");
      break;
  }
  if (what == 0) u8x8.noInverse();

  u8x8.setCursor(0, i++);
  u8x8.print("HT ");
  u8x8.print(temp[HOT_SIDE_A], 1);
  u8x8.print(' ');
  u8x8.print(temp[HOT_SIDE_A] - temp[HOT_SIDE_B]);
  u8x8.print("  ");

  u8x8.setCursor(0, i++);
  if (what == 1) u8x8.inverse();
  u8x8.print("CD ");
  u8x8.print(temp[COLD_SIDE_A], 1);
  u8x8.print(' ');
  u8x8.print(temp[COLD_SIDE_A] - temp[COLD_SIDE_B]);
  u8x8.print("  ");
  if (what == 1) 
  {
    u8x8.noInverse();
    u8x8.setCursor(5, i++);
    u8x8.print(target_cold, 1);
    u8x8.print("  ");
  }

  u8x8.setCursor(0, i++);
  if (what == 2) u8x8.inverse();
  u8x8.print("SP ");
  u8x8.print(temp[SPECIMEN_A], 1);
  u8x8.print(' ');
  u8x8.print(temp[SPECIMEN_A] - temp[SPECIMEN_B]);
  u8x8.print("  ");
  if (what == 2) 
  { 
    u8x8.noInverse();
    u8x8.setCursor(5, i++);
    u8x8.print(target_specimen, 1);
    u8x8.print("  ");
  }

  u8x8.setCursor(0, i++);
  if (what == 3) u8x8.inverse();
  u8x8.print("PLT ");
  if (what == 3) u8x8.noInverse();
  u8x8.print(cooler_a ? "on  " : "off ");
  u8x8.print(cooler_b ? "on  ": "off ");

  u8x8.setCursor(0, i++);
  if (what == 4) u8x8.inverse();
  u8x8.print("PMP ");
  u8x8.print(current_state == PUMPING || current_state == MANUAL ? "on  " : "off ");
  u8x8.print(current_state == PUMPING || current_state == MANUAL ? "on  ": "off ");
  if (what == 4) 
  {
    u8x8.noInverse();
    u8x8.setCursor(5, i++);
    u8x8.print(pump_input);
    u8x8.print(' ');
    u8x8.print(pump_output);
    u8x8.print(' ');
  }
}

void handler_dashboard()
{
  draw_state(-1);

  handler_common();
}

void handler_manual()
{
  static byte current_edit = 0;
  int dp1 = knob_a_pos - last_knob_a_pos;
  int dp2 = knob_b_pos - last_knob_b_pos;

  if (ENT.pushed())
    current_edit = min(4, current_edit + 1);
  else if (ESC.pushed())
    current_edit = max(0, current_edit - 1);

  if (ENT.pushed() || ESC.pushed())
    u8x8.clear();

  // allow edits
  switch (current_edit)
  {
    case 0: // state
      if (dp2 > 0)
        current_state = min(PUMPING, current_state + 1);
      else if (dp2 < 0)
        current_state = max(MANUAL, current_state - 1);
      break;
    case 1: // cold side target
      target_cold = max(-30, min(25, target_cold + dp2));
      break;
    case 2: // specimen side target
      target_specimen = max(-30, min(25, target_specimen + dp2));
      break;
    case 3: // peltier element switcher
      if (dp1 > 0)
        cooler_a = true;
      else if (dp1 < 0)
        cooler_a = false;
      if (dp2 > 0)
        cooler_b = true;
      else if (dp2 < 0)
        cooler_b = false;
      break;
    case 4:
      pump_input = max(0, min(100, pump_input + dp1));
      pump_output = max(0, min(100, pump_output + dp2));
      break;
  }
  update_menus();
  
  draw_state(current_edit);
  handler_common();
}

void handler_menu()
{
  if (ESC.pushed())
  {
    ms.back();
    u8x8.clear();
    menu_start = millis();
  } else if (ENT.pushed())
  {
    ms.select();
    u8x8.clear();
    menu_start = millis();
  }
  else if (knob_b_pos > last_knob_b_pos)
  {
    ms.next();
    menu_start = millis();
  }
  else if (knob_b_pos < last_knob_b_pos)  
  {
    ms.prev();
    menu_start = millis();
  }

  ms.display();

  if (millis() - menu_start > 1000l * 3)
  {
    default_handler = handler_dashboard;
  }
}

///////////////////////////////////////////
//
///////////////////////////////////////////
void perform_controls()
{  
  bool ca, cb;
  byte pa, pb;

  pa = pb = 0;
  ca = cb = false;
  
  switch (current_state)
  {
    case IDLE:
      cooler_a = cooler_b = false;
      break;
    case COOLING:
      ca = cooler_a = true;
      cb = cooler_b = true;

      if (temp[SPECIMEN_A] < target_specimen)
      {
        current_state = PUMPING;
      }
      break;
    case PUMPING:
      pa = pump_input * 2 + 55;
      pb = pump_output * 2 + 55;

      if (temp[SPECIMEN_A] > target_specimen)
      {
        pa = pb = 0;
        current_state = COOLING;
      }
      break;
    case MANUAL:
      ca = cooler_a;
      cb = cooler_b;
      pa = pump_input * 2 + 55;
      pb = pump_output * 2 + 55;
      break;
  }

  digitalWrite(COOLER_A_PIN, ca);
  digitalWrite(COOLER_B_PIN, cb);
  analogWrite(MOTOR_A_PIN, pa);
  analogWrite(MOTOR_B_PIN, pb);
}

///////////////////////////////////////////
//
///////////////////////////////////////////
void on_manual(MenuItem * )
{
  default_handler = handler_manual;
  u8x8.clear();
}
void on_restart(MenuItem * )
{
  
}
void on_save(MenuItem * )
{
  write_defaults();
}
void on_load(MenuItem * )
{
  read_defaults();
}
void on_temp_cold(MenuItem * )
{
  target_cold = m_temp_cold.get_value();
}
void on_temp_specimen(MenuItem * )
{
  target_specimen = m_temp_specimen.get_value();
}
void on_temp_P(MenuItem * )
{
  switch (target)
  {
    case COLD_SIDE_A:
      cold_PID[0] = m_temp_p.get_value();
      break;
    case SPECIMEN_A:
      specimen_PID[0] = m_temp_p.get_value();
      break;
  }
}
void on_temp_I(MenuItem * )
{
  switch (target)
  {
    case COLD_SIDE_A:
      cold_PID[1] = m_temp_i.get_value();
      break;
    case SPECIMEN_A:
      specimen_PID[1] = m_temp_i.get_value();
      break;
  }
}
void on_temp_D(MenuItem * )
{
  switch (target)
  {
    case COLD_SIDE_A:
      cold_PID[2] = m_temp_d.get_value();
      break;
    case SPECIMEN_A:
      specimen_PID[2] = m_temp_d.get_value();
      break;
  }
}
void on_temp_ctrl_cold(MenuItem * )
{
  target = COLD_SIDE_A;
  update_menus();
}
void on_temp_ctrl_specimen(MenuItem * )
{
  target = SPECIMEN_A;
  update_menus();
}
void on_pump_in(MenuItem * )
{
  pump_input = (byte)m_pump_input.get_value();
}
void on_pump_out(MenuItem * )
{
  pump_output = (byte)m_pump_input.get_value();
}

