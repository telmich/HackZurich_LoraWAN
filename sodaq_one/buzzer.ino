#include <Arduino.h>

void setupBuzzer()
{
    pinMode(BUZZER_PIN, OUTPUT);
}

void buzzerOn() {
    digitalWrite(BUZZER_PIN, HIGH);
}

void buzzerOff() {
    digitalWrite(BUZZER_PIN, LOW);
}

void buzz(int ms) {
     buzzerOn();
     delay(ms);
     buzzerOff();
}
