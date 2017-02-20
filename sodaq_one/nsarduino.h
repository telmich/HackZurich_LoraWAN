#include <Arduino.h>

/* battery.ino */
// uint16_t getBatteryVoltage();
int getBattery();

void blink(int length);
void led_on();
void led_off();
void signal_loop_start();


void setupBuzzer(int pin);

void buzzerOn();
void buzzerOff();
void buzz(int pin, int ms);

/* compass.ino */
extern char compassReport[80];

/* ledcolours.ino */
void setupLED();

/* loudness.ino */
int readLoudness();


/* lora.ino */
void setupLoRa();
void sendPacket(String packet);

/* sunlight.ino */
String getSunLight();
void setupSunLight();

/* gps.ino */
void gpsSetup();
String gpsGetPostion(long waittime);
String gpsGetDateTime(long waittime);
