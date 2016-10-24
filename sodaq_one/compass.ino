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

String getCompass()
{
    String tmps;
    compass.read();

    tmps = String("compass.a.x=" ) + compass.a.x +
        String(" compass.a.y=" ) + compass.a.y +
        String(" compass.a.z=" ) + compass.a.z +
        String(" compass.m.x=" ) + compass.m.x +
        String(" compass.m.y" ) + compass.m.y +
        String(" compass.m.z=" ) + compass.m.z;

    debugSerial.println(tmps);

    /* snprintf(compassReport, sizeof(compassReport), "A: %6d %6d %6d    M: %6d %6d %6d", */
    /*          compass.a.x, compass.a.y, compass.a.z, */
    /*          compass.m.x, compass.m.y, compass.m.z); */

}
