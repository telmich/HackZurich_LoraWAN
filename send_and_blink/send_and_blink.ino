#include "Sodaq_RN2483.h"
#include "Arduino.h"


#define debugSerial SerialUSB
#define loraSerial Serial1

/* The number of the device: 1,2,3,4 */
#define deviceNo 2


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
        RED();
        break;
    case 3:
        GREEN();
        break;
    case 4:
        YELLOW();
        break;
    }
    delay(length);
    CLEAR();
}


// OTAA
// Random numbers chosen + device id
uint8_t DevEUI[8] = { 0x9c, 0xd9, 0x0b, 0xb5, 0x2b, 0x6a, 0x1d, deviceNo };

uint8_t AppEUI[8] = { 0xd4, 0x16, 0xcd, 0x0b, 0x7b, 0xcf, 0x2d, 0x5c };

uint8_t AppKey[16] = { 0xa9, 0xbc, 0x8b, 0x6a, 0x81, 0x75, 0xf6, 0x33,
0xe0, 0xd6, 0x64, 0xd9, 0x2b, 0xcb, 0x13, 0x78 };

uint8_t counter;

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

void setupLED() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
}

void setup() {
  //Power up the LoRaBEE
  pinMode(ENABLE_PIN_IO, OUTPUT); // ONE
  digitalWrite(ENABLE_PIN_IO, HIGH); // ONE
  delay(3000);

  while ((!SerialUSB) && (millis() < 10000)){
    // Wait 10 seconds for the Serial Monitor
  }

  //Set baud rate
  debugSerial.begin(57600);
  loraSerial.begin(LoRaBee.getDefaultBaudRate());

  // Debug output from LoRaBee
  // LoRaBee.setDiag(debugSerial); // optional

  setupLED();
  blink(60);

  /* used for blinking */
  counter=0;

  //connect to the LoRa Network
  setupLoRa();

}

void setupLoRa(){
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

  // put your main code here, to run repeatedly:
  String packet = "SODAQ";

  /* Blink long after sending packet */
  if(counter >= 60) {
      sendPacket(packet);
      blink(500);
      counter = 0;
  } else {
      blink(30);
      counter++;
  }

  delay(1000);
}
