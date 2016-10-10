#include <Arduino.h>
#include <Sodaq_UBlox_GPS.h>

#include "Sodaq_RN2483.h"

#define debugSerial SerialUSB
#define loraSerial Serial1

/* The number of the device: 1,2,3,4 */
#define deviceNo 1

#define beePin ENABLE_PIN_IO

void sendPacket(String packet);

void BLUE() {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, LOW);
}

void RED() {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, HIGH);
}

void YELLOW() {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, HIGH);
}

void WHITE() {
    digitalWrite(LED_RED, LOW);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, LOW);
}


void GREEN() {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_BLUE, HIGH);
}

void CLEAR() {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, HIGH);
}

void blink(int length) {
    switch(deviceNo) {
    case 1:
        BLUE();
        break;
    case 2:
        WHITE();
        break;
    case 3:
        GREEN();
        break;
    case 4:
        RED();
        break;
    }
    delay(length);
    CLEAR();
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

void setupGPS()
    {
        sodaq_gps.init(GPS_ENABLE);
    }

/*!
 * Find a GPS fix, but first wait a while
 */
void send_gps()
{
    /* Delay and light up */
    blink(10000);

    uint32_t start = millis();
    uint32_t timeout = 900L * 1000;
    debugSerial.println(String("waiting for fix ..., timeout=") + timeout + String("ms"));

    if (sodaq_gps.scan(false, timeout)) {
        debugSerial.println(String(" time to find fix: ") + (millis() - start) + String("ms"));
        debugSerial.println(String(" datetime = ") + sodaq_gps.getDateTimeString());

        String lat= String("lat=") + String(sodaq_gps.getLat(), 7);
        debugSerial.println(lat);
        sendPacket(lat);

        String lon = String("lon=") + String(sodaq_gps.getLon(), 7);
        debugSerial.println(lon);
        sendPacket(lon);

        String sats = String("num sats = ") + String(sodaq_gps.getNumberOfSatellites());
        debugSerial.println(sats);
        sendPacket(sats);
    } else {
        debugSerial.println("No Fix");
        sendPacket("No Fix");
    }
}


// OTAA
// Random numbers chosen + device id
uint8_t DevEUI[8] = { 0x9c, 0xd9, 0x0b, 0xb5, 0x2b, 0x6a, 0x1d, deviceNo };
// uint8_t AppEUI[8] = { 0xd4, 0x16, 0xcd, 0x0b, 0x7b, 0xcf, 0x2d, 0x5c };
// uint8_t AppKey[16] = { 0xa9, 0xbc, 0x8b, 0x6a, 0x81, 0x75, 0xf6, 0x33, 0xe0, 0xd6, 0x64, 0xd9, 0x2b, 0xcb, 0x13, 0x78 };

// ttn
uint8_t AppEUI[8] = { 0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x00, 0x0E, 0x9B
};

// id=2
// uint8_t AppKey[16] = { 0xE4, 0xAC, 0x4D, 0xD4, 0xB2, 0xA7,
// 0x84,0xB8, 0x9D, 0xC9, 0x4C, 0x37, 0xD9, 0x0E, 0x22, 0x45 };

// id=1
uint8_t AppKey[16] = { 0x27, 0x29, 0xCC, 0x46, 0xAD, 0xFA, 0xFA, 0x95, 0xA1, 0x8A, 0xA1, 0x21, 0x57, 0x21, 0x23, 0x6E };


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


void setup() {
    //Power up the LoRaBEE - on loraone/sodaq one
    pinMode(ENABLE_PIN_IO, OUTPUT); // ONE
    digitalWrite(beePin, HIGH); // ONE
    delay(3000);

    while ((!SerialUSB) && (millis() < 10000)){
        // Wait 10 seconds for the Serial Monitor
    }

    //Set baud rate
    debugSerial.begin(57600);
    loraSerial.begin(LoRaBee.getDefaultBaudRate());

    setupGPS();
    blink(30);
    blink(30);

    setupLED();
    blink(60);

    //connect to the LoRa Network
    setupLoRa();
}

void setupLoRa()
{
    setupLoRaOTAA();
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

void loop() {
    /* Announce begin of code */
    blink(20); delay(50);
    blink(20); delay(50);
    blink(20); delay(50);

    String id = String("GPS node ")  + String(deviceNo);
    sendPacket(id);
    sendBatVoltage();

    /* Wait for 30s to send again */
    blink(1000); delay(29000);

    /* Wait one minute for finding fix */
    send_gps();
}
