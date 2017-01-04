#include <Arduino.h>



int getB()
{
    return (float)analogRead(BAT_VOLT);
}
