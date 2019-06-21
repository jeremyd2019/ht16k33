#include <Wire.h>

#include "ht16k33.h"

HT16K33Display ht16k33(0x70);

void setup()
{
    Serial.begin(9600);
    Wire.begin();
    delay(1);
    ht16k33.systemSetup(true);
    ht16k33.dimmingSet(5);
    ht16k33.writeDigitRaw(0, 0xFFFF);
    ht16k33.writeDigitRaw(1, 0xFFFF);
    ht16k33.writeDigitRaw(2, 0xFFFF);
    ht16k33.writeDigitRaw(3, 0xFFFF);
    ht16k33.displaySetup(true);
    ht16k33.writeDisplay();
}

void loop()
{
}
