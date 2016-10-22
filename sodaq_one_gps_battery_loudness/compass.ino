#include <Wire.h>
#include <LSM303.h>

LSM303 compass;

char compassReport[80];

void setupCompass()
{
  Wire.begin();
  compass.init();
  compass.enableDefault();
}

void readCompass()
{
  compass.read();

  snprintf(compassReport, sizeof(compassReport), "A: %6d %6d %6d    M: %6d %6d %6d",
           compass.a.x, compass.a.y, compass.a.z,
           compass.m.x, compass.m.y, compass.m.z);

}
