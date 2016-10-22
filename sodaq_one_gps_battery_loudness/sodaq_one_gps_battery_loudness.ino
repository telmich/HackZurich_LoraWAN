#include <Arduino.h>
#include <Wire.h>
#include <Sodaq_UBlox_GPS.h>


#include "nsarduino.h"

#define debugSerial SerialUSB

/* The number of the device: 1,2,3,4 */
#define deviceNo 2

#define LOUDNESS_SENSOR 0
#define USE_LOUDNESS 1
#define BUZZER_PIN 2

String msg_gps;
int loudness;

int readLoudness()
{
	return analogRead(LOUDNESS_SENSOR);
}

void sendBatVoltage() {
  String battery = String("bat1=") + String(getBatteryVoltage());
  sendPacket(battery);
}

void setupLED() {
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
}

void setup_gps() { sodaq_gps.init(GPS_ENABLE); }

/*!
 * Find a GPS fix, but first wait a while
 */
boolean get_gps(long waittime)
{
    uint32_t timeout = waittime * 1000;

    debugSerial.println(String("waiting for fix ..., timeout=") + timeout + String("ms"));
    blink(5000);

    uint32_t start = millis();

    if (sodaq_gps.scan(false, timeout)) {
        debugSerial.println(String(" time to find fix: ") + (millis() - start) + String("ms"));
        debugSerial.println(String(" datetime = ") + sodaq_gps.getDateTimeString());


        msg_gps = String("lat=") + String(sodaq_gps.getLat(), 7) + " " + String("lon=") + String(sodaq_gps.getLon(), 7) + " " + String("sats=") + String(sodaq_gps.getNumberOfSatellites());

        /* String lat= String("lat=") + String(sodaq_gps.getLat(), 7); */
        /* debugSerial.println(lat); */
        /* msg_gps = lat; */

        /* String lon = String("lon=") + String(sodaq_gps.getLon(), 7); */
        /* debugSerial.println(lon); */
        /* msg_gps += " " + lon; */

        /* String sats = String("sats=") + String(sodaq_gps.getNumberOfSatellites()); */

        /* msg_gps += " " + sats; */

        debugSerial.println(msg_gps);
    } else {
        debugSerial.println("GPS: No Fix");
        return false;
    }
    return true;
}

/*
*  1 Sodaq
*  2 sodaq
*  3 sodaq
*  4 rpi
*/
// 9cd90bb52b6a1d

void signal_loop_start()
{
    blink(30); delay(50);
    blink(30); delay(50);
    blink(30); delay(50);
}

void signal_loop_end()
{
    /* Wait for 30s to send again */
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

    setup_gps();

    setupBuzzer();
    setupCompass();


    blink(60);

    setupSunLight();

    loraSetup();
}

int mybyte;
int i;

String tmp;

void loop() {
    signal_loop_start();

    /* String msg_id = String("node=")  + String(deviceNo); */
    /* sendPacket(msg_id); */

    /* sendBatVoltage(); */
    // sendLight();

    /* loudness = readLoudness(); */
    /* String msg_loudness = "loudness=" + String(loudness); */
    /* debugSerial.println(msg_loudness); */

    tmp = getSunLight();
    debugSerial.println(tmp);
    loraSend(tmp);

    readCompass();
    debugSerial.println(compassReport);

    // buzz(30);

//    sendPacket(msg_loudness);

    /* if(get_gps(60)) { */
    /*     sendPacket(msg_gps); */
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
