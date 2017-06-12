// Simple example using the ADNS-2620 optical mouse camera, reading out the motion registers and displaying the running total to the serial port.
// Requires the Mousecam (ADNS) hardware library.


#include "Adns.h"

int pos_x=0;
int pos_y=0;

Mousecam cam(10, 11, 12, 8); // sclk, sdio, pwr, gnd


void setup()
{
  Serial.begin(115200);

  pinMode(6, OUTPUT);  // optional: run an LED
  pinMode(7, OUTPUT);
  digitalWrite(6, HIGH);  // LED anode
  digitalWrite(7, LOW);    // LED cathode


  
  if(cam.adns_init() == 0)
  {
    Serial.println("Cam not found\r\n");
  }
  
}

void loop()
{
    //Note: The mouse camera returns signed 8-bit values (char) for the DELTA_X and DELTA_Y; the rest return (byte)
    pos_x = pos_x + cam.adns_get_delta_x();
    pos_y = pos_y + cam.adns_get_delta_y();    

    Serial.print("X= ");
    Serial.print(pos_x, DEC);
    Serial.print("  Y= ");
    Serial.println(pos_y, DEC);

// Uncomment the below to send raw data to a PC program instead
//    Serial.print(cam.adns_get_delta_x(), BYTE);
//    Serial.print(cam.adns_get_delta_y(), BYTE);
//    Serial.print(cam.adns_get_max_pixel(), BYTE);
//    Serial.print(cam.adns_get_min_pixel(), BYTE);
//    Serial.print(cam.adns_get_surface_quality(), BYTE);

}

