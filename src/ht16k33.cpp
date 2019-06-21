#include <Arduino.h>
#include <Wire.h>
#include <assert.h>

#ifdef LOOK_BEHIND_THE_CURTAIN
extern "C" {
#include <utility/twi.h>
}
#endif

#include "ht16k33.h"

void HT16K33Communications::sendCommand(uint8_t command)
{
    Wire.beginTransmission(m_addr);
    Wire.write(command);
    Wire.endTransmission();
}

uint8_t HT16K33Communications::readMemory(uint8_t address, uint8_t * buffer, uint8_t len)
{
    Wire.beginTransmission(m_addr);
    Wire.write(address);
    Wire.endTransmission();
#ifdef LOOK_BEHIND_THE_CURTAIN
    return twi_readFrom(m_addr, buffer, len, true);
#else
    uint8_t ret = Wire.requestFrom(m_addr, len);
    ret = min(ret, len);
    // memcpy anyone?
    for (int i = 0; i < ret; ++i)
        *(buffer++) = Wire.read();
    return ret;
#endif
}

void HT16K33Communications::writeMemory(uint8_t address, uint8_t * buffer, uint8_t len)
{
    Wire.beginTransmission(m_addr);
    Wire.write(address);
    Wire.write(buffer, len);
    Wire.endTransmission();
}
