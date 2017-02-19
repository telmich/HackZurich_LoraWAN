#include <Arduino.h>

void setupBuzzer(int pin)
{
    pinMode(pin, OUTPUT);
}

void buzzerOn(int pin) {
    digitalWrite(pin, HIGH);
}

void buzzerOff(int pin) {
    digitalWrite(pin, LOW);
}

void buzz(int pin, int ms) {
     buzzerOn(pin);
     delay(ms);
     buzzerOff(pin);
}
