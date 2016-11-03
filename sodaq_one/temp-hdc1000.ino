#include <Arduino.h>
#include <Wire.h>
#include <HDC1000.h>

HDC1000 hdc;

void setupTempHumidHDC1000() {
    hdc.begin(); delay(500);
}

String getTempHumidHDC1000() {
    float temperature;
    float humidity;

    humidity =  hdc.getHumidity();
    temperature = hdc.getTemperature();
    return String("tmp=") + temperature + String(" humid=") + humidity;
}
