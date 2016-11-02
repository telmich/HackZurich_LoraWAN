#include <Arduino.h>
#include "Sodaq_RN2483.h"

#define loraSerial Serial1
#define beePin ENABLE_PIN_IO

#ifdef LORA_ABP
void setupLoRaABP(){
  if (LoRaBee.initABP(loraSerial, devAddr, appSKey, nwkSKey, true))
  {
      debugSerial.println("Lora: ABP setup ok");
  }
  else
  {
      debugSerial.println("Lora: ABP setup failed");
  }
}
#endif

#ifdef LORA_OTAA
void setupLoRaOTAA(){
    if (LoRaBee.initOTA(loraSerial, DevEUI, AppEUI, AppKey, true))
    {
        debugSerial.println("Lora: OTAA setup ok");
    }
    else
    {
        debugSerial.println("Lora: OTAA Setup failed!");
    }
}
#endif


void loraSetup()
{
    pinMode(beePin, OUTPUT); // ONE
    digitalWrite(beePin, HIGH); // Lorawan
    loraSerial.begin(LoRaBee.getDefaultBaudRate());

#ifdef LORA_OTAA
    setupLoRaOTAA();
#endif

#ifdef LORA_ABP
    setupLoRaABP();
#endif
}

void loraSend(String packet){
    debugSerial.println("Trying to send: " + packet);
    switch (LoRaBee.sendReqAck(1, (uint8_t*)packet.c_str(), packet.length(), 8))
    {
    case NoError:
        debugSerial.println("Successful transmission.");
        break;
    case NoResponse:
        debugSerial.println("There was no response from the device.");
        loraSetup();
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
        loraSetup();
        break;
    case Busy:
        debugSerial.println("The device is busy. Sleeping for 10 extra seconds.");
        delay(10000);
        break;
    case NetworkFatalError:
        debugSerial.println("There is a non-recoverable error with the network connection. You should re-connect.\r\nThe network connection will reset.");
        loraSetup();
        break;
    case NotConnected:
        debugSerial.println("The device is not connected to the network. Please connect to the network before attempting to send data.\r\nThe network connection will reset.");
        loraSetup();
        break;
    case NoAcknowledgment:
        debugSerial.println("There was no acknowledgment sent back!");
        // When you this message you are probaly out of range of the network.
        break;
    default:
        break;
    }

    /* Delay some time to give avoid keeping the device busy */
    delay(2000);
}
