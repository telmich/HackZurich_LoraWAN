#include <Arduino.h>
#include "SI114X.h"

SI114X SI1145 = SI114X();

void setupSunLight() {
  while (!SI1145.Begin()) {
    debugSerial.println("Si1145 is not ready!");
    delay(1000);
  }
  debugSerial.println("Si1145 is ready!");
}

String getSunLight()
{
    return String("vis=") + SI1145.ReadVisible() + String(" ir=") + SI1145.ReadIR() + String(" uv=") + ((float)SI1145.ReadUV()/100);
}
