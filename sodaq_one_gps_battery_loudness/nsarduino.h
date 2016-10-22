
/* battery.ino */
uint16_t getBatteryVoltage();

void blink(int length);

void buzzerOn();
void buzzerOff();
void buzz(int ms);

/* compass.ino */
extern char compassReport[80];

/* lora.ino */
void setupLoRa();
void sendPacket(String packet);

/* sunlight.ino */
String getSunLight();
void setupSunLight();
