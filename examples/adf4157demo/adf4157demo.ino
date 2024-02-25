#include "ADF4157.h"
const byte deviceUpdate = 32;          // The Ardunio pin where the device update is controlled, if used
ADF4157 Device(deviceUpdate);

const double mark = 2592196250.0; // for ADF 4157 with x4 10GHZ VCO
const double space = 2592196123.4; // for ADF 4157 with x4 10GHZ VCO

void setup() {
  Device.Initialize(mark);
  delay(3000);
}

void loop() {
  delay(100);
  Device.SetFrequency(space);
  delay(100);
  Device.SetFrequency(mark);
}
