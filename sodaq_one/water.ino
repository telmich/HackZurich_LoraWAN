#include <Arduino.h>

void setupWater(int pin) {
     pinMode(pin, INPUT);
}

boolean hasWater(int pin)
{
    if(digitalRead(pin) == LOW) {
        return true;
    } else {
        return false;
    }
}

String getWater(int pin)
{
    String tmp;

    if(hasWater(pin)) {
        tmp = "water=1";
    } else {
        tmp = "water=0";
    }
    return tmp;
}
