#include <Arduino.h>
#include <Wire.h>

#include "nsarduino.h"

#define debugSerial SerialUSB

#define deviceNo 2

#define LOUDNESS_SENSOR 0
#define USE_LOUDNESS 1
#define BUZZER_PIN 2

void sendBatVoltage() {
  String battery = String("bat1=") + String(getBatteryVoltage());
  sendPacket(battery);
}

void signal_loop_start()
{
    blink(30); delay(50);
    blink(30); delay(50);
    blink(30); delay(50);
}

void signal_loop_end()
{
    blink(1000); delay(29000);
}

void setup() {
    while ((!SerialUSB) && (millis() < 10000)){
        // Wait 10 seconds for the Serial Monitor
    }

    /* Enable the pins 2/3, 6/7 and 8/9 */
    pinMode(11, OUTPUT);
    digitalWrite(11, HIGH);

    setupLED();
    gpsSetup();
    setupBuzzer();
    setupCompass();
    setupSunLight();
    loraSetup();
}


void sendIntAsString(String prefix, int value) {
    String tmp = prefix + String(value);
    debugSerial.println(tmp);
    loraSend(tmp);
}

String tmps;
int tmpi;

void loop() {
    signal_loop_start();

    loraSend(String("node=")  + String(deviceNo));
    sendIntAsString("battery=", getBatteryVoltage());
    sendIntAsString("loudness=", readLoudness());

    tmps = getSunLight();
    debugSerial.println(tmps);
    loraSend(tmps);

    readCompass();
    debugSerial.println(compassReport);

    // buzz(30);

    /* if((tmps = gpsGetPostion(120)) != "") { */
    /*     loraSend(tmps); */
    /* } */

    signal_loop_end();
}


/* Node = 2
48
48
48
52
65
51
48
66

hex: 3030303441333042
*/

/* temp / humidity code (not functional)

/* humid / temperature */
/*
#include <HDC1000.h>

HDC1000 hdc;

float temperature;
float humidity;


*/

//    temperature = hdc.getTemperature();
//    humidity =  hdc.getHumidity();
//    String msg_tmphumid = String("tmp=") + temperature + String(" humid=") + humidity;
//    debugSerial.println(msg_tmphumid);
