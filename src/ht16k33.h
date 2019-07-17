#ifndef HT16K33_H_3546452__
#define HT16K33_H_3546452__

#include <assert.h>

class HT16K33Communications
{
public:
    inline HT16K33Communications(uint8_t addr)
        : m_addr (addr)
    { }

    void sendCommand(uint8_t command);
    uint8_t readMemory(uint8_t address, uint8_t * buffer, uint8_t len = 1);
    void writeMemory(uint8_t address, uint8_t * buffer, uint8_t len = 1);

private:
    uint8_t m_addr;
};

class HT16K33Commands : protected HT16K33Communications
{
public:
    inline HT16K33Commands(uint8_t addr)
        : HT16K33Communications (addr)
    { }

    inline void systemSetup(bool powerOn)
    {
        sendCommand(SYSTEM_SETUP | (powerOn ? 1 : 0));
    }

    inline bool intFlag()
    {
        uint8_t retval = 0;
        readMemory(INT_FLAG_ADDRESS_POINTER, &retval, sizeof(retval));
        return retval != 0;
    }

    enum BlinkFreq
    {
        BLINK_OFF = 0,
        BLINK_2HZ,
        BLINK_1HZ,
        BLINK_0_5HZ
    };

    inline void displaySetup(bool displayOn, BlinkFreq blinkFreq = BLINK_OFF)
    {
        assert(blinkFreq >= 0 && blinkFreq <= 3);
        sendCommand(DISPLAY_SETUP | (blinkFreq << 1) | (displayOn ? 1 : 0));
    }

    inline void rowIntSet(bool intOutput, bool activeHigh)
    {
        sendCommand(ROW_INT_SET | ((activeHigh ? 1 : 0) << 1) | (intOutput ? 1 : 0));
    }

    inline void dimmingSet(uint8_t sixteenths)
    {
        assert (sixteenths > 0 && sixteenths <= 16);
        sendCommand(DIMMING_SET | (sixteenths - 1));
    }

    inline uint8_t readKeys(uint16_t (&keyBuffer)[3])
    {
        return readMemory(KEY_DATA_ADDRESS_POINTER, reinterpret_cast<uint8_t*>(keyBuffer), sizeof(keyBuffer));
    }

    inline void writeDisplay(uint16_t (&displayBuffer)[8])
    {
        writeMemory(DISPLAY_DATA_ADDRESS_POINTER, reinterpret_cast<uint8_t*>(displayBuffer), sizeof(displayBuffer));
    }

protected:
    enum Commands
    {
        DISPLAY_DATA_ADDRESS_POINTER = 0x00,
        SYSTEM_SETUP = 0x20,
        KEY_DATA_ADDRESS_POINTER = 0x40,
        INT_FLAG_ADDRESS_POINTER = 0x60,
        DISPLAY_SETUP = 0x80,
        ROW_INT_SET = 0xA0,
        DIMMING_SET = 0xE0,
        TEST_MODE = 0xD9
    };
};

class HT16K33Display : protected HT16K33Commands
{
    typedef HT16K33Commands supper;
public:
    inline HT16K33Display(uint8_t addr)
        : HT16K33Commands (addr)
        , m_displayBuffer {0}
        , m_keyBuffer {0}
    { }

    using supper::systemSetup;
    using supper::BlinkFreq;
    using supper::displaySetup;
    using supper::rowIntSet;
    using supper::dimmingSet;

    inline void clear()
    {
        memset(m_displayBuffer, 0, sizeof(m_displayBuffer));
    }

    inline void writeDigitRaw(uint8_t n, uint16_t glyph)
    {
        m_displayBuffer[n] = glyph;
    }

    inline void writeDisplay()
    {
        supper::writeDisplay(m_displayBuffer);
    }

    inline void readKeys()
    {
        supper::readKeys(m_keyBuffer);
    }

    inline bool isKeySet(uint8_t col, uint8_t row) const
    {
        return !!(m_keyBuffer[col] & (1 << row));
    }

    struct KeyPosition
    {
        uint8_t col:4;
        uint8_t row:4;
    };

    inline KeyPosition firstKeySet() const
    {
        for (uint8_t col = 0; col < 3; ++col)
            if (m_keyBuffer[col])
                return {col, static_cast<uint8_t>(__builtin_ctz(m_keyBuffer[col]))};
        return {0xF, 0xF};
    }

    struct KeyPosition2
    {
        KeyPosition key[2];
    };

    inline KeyPosition2 twoKeysSet() const
    {
        KeyPosition2 retval = {{{0xF, 0xF}, {0xF, 0xF}}};
        uint8_t i = 0;
        for (uint8_t col = 0; col < 3 && i < 2; ++col)
        {
            if (m_keyBuffer[col])
            {
                uint16_t tmp = m_keyBuffer[col];
                retval.key[i] = {col, static_cast<uint8_t>(__builtin_ctz(tmp))};
                tmp &= ~(1 << retval.key[i++].row);
                if (i < 2 && tmp)
                    retval.key[i++] = {col, static_cast<uint8_t>(__builtin_ctz(tmp))};
            }
        }

        return retval;
    }

private:
    uint16_t m_displayBuffer[8];
    uint16_t m_keyBuffer[3];
};

#endif // HT16K33_H_3546452__