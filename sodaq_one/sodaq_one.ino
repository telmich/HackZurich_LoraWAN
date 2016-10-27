#include <Arduino.h>
#include <Wire.h>
#include <math.h>

#include "nsarduino.h"

#define debugSerial SerialUSB

/* Which network to use */
#define LORADEV 1
// #define SWISSCOM 1
// #define LORIOT 1
#define TTN 1

#define BUZZER_PIN 2

void signal_loop_start()
{
    blink(30); delay(50);
    blink(30); delay(50);
    blink(30); delay(50);
}


int sleepcnt;

void setup() {
    while ((!SerialUSB) && (millis() < 10000)){
        // Wait 10 seconds for the Serial Monitor
    }

    /* Enable the pins 2/3, 6/7 and 8/9 */
    pinMode(11, OUTPUT);
    digitalWrite(11, HIGH);

    setupLED();
    gpsSetup();

    // setupBuzzer();

    setupCompass();
    setupSunLight();
    loraSetup();


    /* sleep little in the beginning, longer the longer we run */
    sleepcnt = 0;

}


void sendIntAsString(String prefix, int value) {
    String tmp = prefix + String(value);
    debugSerial.println(tmp);
    loraSend(tmp);
}

void sendFloatAsString(String prefix, float value) {
    String tmp = prefix + String(value);
    debugSerial.println(tmp);
    loraSend(tmp);
}


String tmps;

#define TEMP_PIN 2
#define LOUDNESS_PIN 0

#define SLEEPTIME 5*60*1000

void loop() {
    signal_loop_start();

    sendFloatAsString("temperature=", getTemperature(TEMP_PIN));

    /* loraSend(String("node=")  + String(deviceNo)); */
    sendIntAsString("battery=", getBatteryVoltage());
    sendIntAsString("loudness=", readLoudness(LOUDNESS_PIN));
    loraSend(getSunLight());
    loraSend(getCompass());

    if((tmps = gpsGetPostion(120)) != "") {
        loraSend(tmps);
    }

    if(sleepcnt < 10) {
        sleepcnt++;
        delay(10000);
    } else {
        delay(SLEEPTIME);
    }
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


*/

/* #include <Wire.h> */
/* #include <HDC1000.h> */

/* HDC1000 hdc; */

/* float temperature; */
/* float humidity; */

    /* hdc.begin(); delay(500); */
    /* temperature = hdc.getTemperature(); */
    /* delay(500); */
    /* hdc.begin(); delay(500); */
    /* humidity =  hdc.getHumidity(); */
    /* String msg_tmphumid = String("tmp=") + temperature + String(" humid=") + humidity; */
    /* debugSerial.println(msg_tmphumid); */
