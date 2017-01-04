#include <Arduino.h>

#define ADC_AREF 3.3f
#define BATVOLT_R1 2.0f
#define BATVOLT_R2 2.0f
#define BATVOLT_PIN BAT_VOLT

int getB()
{
    return ((ADC_AREF / 1.023) * (BATVOLT_R1 + BATVOLT_R2) / BATVOLT_R2 * (float)analogRead(BATVOLT_PIN));
}
