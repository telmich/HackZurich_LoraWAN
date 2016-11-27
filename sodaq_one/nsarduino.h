
/* battery.ino */
uint16_t getBatteryVoltage();

void blink(int length);

void buzzerOn();
void buzzerOff();
void buzz(int ms);

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
