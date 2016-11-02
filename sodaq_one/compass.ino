#include <Wire.h>
#include <LSM303.h>

LSM303 compass;

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

    tmps = String("compass a.x=" ) + compass.a.x +
        String(" a.y=" ) + compass.a.y +
        String(" a.z=" ) + compass.a.z +
        String(" m.x=" ) + compass.m.x +
        String(" m.y=" ) + compass.m.y +
        String(" m.z=" ) + compass.m.z;

    debugSerial.println(tmps);

    return tmps;
}
