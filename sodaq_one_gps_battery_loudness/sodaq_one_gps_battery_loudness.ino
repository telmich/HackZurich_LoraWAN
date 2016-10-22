#include <Arduino.h>
#include <Wire.h>
#include <Sodaq_UBlox_GPS.h>

/* sunlight sensor */
#include "SI114X.h"

#include "ledcolours.h"

#include "Sodaq_RN2483.h"

#define debugSerial SerialUSB
#define loraSerial Serial1

/* The number of the device: 1,2,3,4 */
#define deviceNo 2

#define beePin ENABLE_PIN_IO

#define LOUDNESS_SENSOR 0

#define USE_LOUDNESS 1

void sendPacket(String packet);

String msg_gps;
int loudness;

int readLoudness()
{
	return analogRead(LOUDNESS_SENSOR);
}



#define ADC_AREF 3.3f
#define BATVOLT_R1 2.0f
#define BATVOLT_R2 2.0f
#define BATVOLT_PIN BAT_VOLT

uint16_t getBatteryVoltage()
{
    uint16_t voltage = (uint16_t)((ADC_AREF / 1.023) * (BATVOLT_R1 + BATVOLT_R2) / BATVOLT_R2 * (float)analogRead(BATVOLT_PIN));

    return voltage;
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

uint8_t DevEUI[8] = { 0x9c, 0xd9, 0x0b, 0xb5, 0x2b, 0x6a, 0x1d, deviceNo };


// OTAA
// Random numbers chosen + device id
// 9cd90bb52b6a1d

// Swisscom
// uint8_t AppEUI[8] = { 0xd4, 0x16, 0xcd, 0x0b, 0x7b, 0xcf, 0x2d, 0x5c };
// uint8_t AppKey[16] = { 0xa9, 0xbc, 0x8b, 0x6a, 0x81, 0x75, 0xf6, 0x33, 0xe0, 0xd6, 0x64, 0xd9, 0x2b, 0xcb, 0x13, 0x78 };

// ttn
uint8_t AppEUI[8] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x00, 0x0E, 0x9B };

// ttn id=1
// uint8_t AppKey[16] = { 0x27, 0x29, 0xCC, 0x46, 0xAD, 0xFA, 0xFA, 0x95, 0xA1, 0x8A, 0xA1, 0x21, 0x57, 0x21, 0x23, 0x6E };

// ttn id=2
uint8_t AppKey[16] = { 0xE4, 0xAC, 0x4D, 0xD4, 0xB2, 0xA7, 0x84,0xB8, 0x9D, 0xC9, 0x4C, 0x37, 0xD9, 0x0E, 0x22, 0x45 };


// loriot
uint8_t devAddr[4] = { 0x00, 0xC5, 0x76, 0x8A };
uint8_t appSKey[16] = { 0x70, 0x79, 0x59, 0x92, 0x37, 0xAE, 0x59, 0x41, 0x4C, 0xD5, 0x2C, 0x60, 0xF1, 0x4B, 0x8D, 0x61 };
// 0x1B, 0xFC, 0xE9, 0x18, 0x6C, 0x89, 0xB8, 0x60, 0xD3, 0xF6, 0x73, 0x0D, 0xE3, 0x2F, 0x1C, 0xA3 };
uint8_t nwkSKey[16] = { 0xA0, 0x6B, 0xEB, 0xE1, 0x30, 0xC4, 0xDA, 0x3F, 0x50, 0xD7, 0xFA, 0xB4, 0x73, 0x03, 0xDA, 0x3E };

// loriot2
/* uint8_t devAddr[4] = { 0x00, 0x88, 0x21, 0x30 }; */
/* uint8_t appSKey[16] = { 0x96, 0xEC, 0xAB, 0xBE, 0x1F, 0x2B, 0x99, 0x3C, 0x54, 0x4B, 0x9E, 0xE9, 0xA9, 0x4A, 0x23, 0x2D }; */
/* uint8_t nwkSKey[16] = { 0x1D, 0xAE, 0xC3, 0x27, 0x98, 0x56, 0x2A, 0xB1, 0xBF, 0x42, 0x94, 0xD3, 0xCA, 0x04, 0x2C, 0x56 }; */


void setupLoRaABP(){
  if (LoRaBee.initABP(loraSerial, devAddr, appSKey, nwkSKey, true))
  {
    debugSerial.println("Communication to LoRaBEE successful.");
  }
  else
  {
    debugSerial.println("Communication to LoRaBEE failed!");
  }
}

void setupLoRaOTAA(){
    if (LoRaBee.initOTA(loraSerial, DevEUI, AppEUI, AppKey, true))
    {
        debugSerial.println("Communication to LoRaBEE successful.");
    }
    else
    {
        debugSerial.println("OTAA Setup failed!");
    }
}

SI114X SI1145 = SI114X();

void setupSunlight() {

  while (!SI1145.Begin()) {
    debugSerial.println("Si1145 is not ready!");
    delay(1000);
  }
  debugSerial.println("Si1145 is ready!");
}


void setupLoRa()
{
//    setupLoRaOTAA();
    setupLoRaABP();
}

void sendPacket(String packet){
    switch (LoRaBee.sendReqAck(1, (uint8_t*)packet.c_str(), packet.length(), 8))
    {
    case NoError:
        debugSerial.println("Successful transmission.");
        break;
    case NoResponse:
        debugSerial.println("There was no response from the device.");
        setupLoRa();
        break;
    case Timeout:
        debugSerial.println("Connection timed-out. Check your serial connection to the device! Sleeping for 20sec.");
        delay(20000);
        break;
    case PayloadSizeError:
        debugSerial.println("The size of the payload is greater than allowed. Transmission failed!");
        break;
    case InternalError:
        debugSerial.println("Oh No! This shouldn't happen. Something is really wrong! Try restarting the device!\r\nThe network connection will reset.");
        setupLoRa();
        break;
    case Busy:
        debugSerial.println("The device is busy. Sleeping for 10 extra seconds.");
        delay(10000);
        break;
    case NetworkFatalError:
        debugSerial.println("There is a non-recoverable error with the network connection. You should re-connect.\r\nThe network connection will reset.");
        setupLoRa();
        break;
    case NotConnected:
        debugSerial.println("The device is not connected to the network. Please connect to the network before attempting to send data.\r\nThe network connection will reset.");
        setupLoRa();
        break;
    case NoAcknowledgment:
        debugSerial.println("There was no acknowledgment sent back!");
        // When you this message you are probaly out of range of the network.
        break;
    default:
        break;
    }
}



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

void sendLight()
{
    String msg_light;
    msg_light = String("vis=") + SI1145.ReadVisible() + String(" ir=") + SI1145.ReadIR() + String(" uv=") + ((float)SI1145.ReadUV()/100);

    sendPacket(msg_light);
    debugSerial.println(msg_light);
}

void setup() {
    pinMode(ENABLE_PIN_IO, OUTPUT); // ONE
    digitalWrite(beePin, HIGH); // Lorawan
    delay(3000);

    while ((!SerialUSB) && (millis() < 10000)){
        // Wait 10 seconds for the Serial Monitor
    }

    loraSerial.begin(LoRaBee.getDefaultBaudRate());

    setup_gps();

    setupLED();
    blink(60);

    setupSunlight();
    //connect to the LoRa Network
    setupLoRa();
}


int mybyte;

int i;

void loop() {
    signal_loop_start();

    String msg_id = String("node=")  + String(deviceNo);
    sendPacket(msg_id);

    sendBatVoltage();
    sendLight();

    loudness = readLoudness();
    String msg_loudness = "loudness=" + String(loudness);
    debugSerial.println(msg_loudness);

    sendPacket(msg_loudness);

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
