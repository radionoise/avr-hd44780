#include <hd44780.h>
#include <util/delay.h>

#define sbi(port,bit) (port) |= (1 << (bit))
#define cbi(port,bit) (port) &= ~(1 << (bit))

Hd44780Port *_portRs;
Hd44780Port *_portRw;
Hd44780Port *_portE;

Hd44780DataPort *_portD0;
Hd44780DataPort *_portD1;
Hd44780DataPort *_portD2;
Hd44780DataPort *_portD3;
Hd44780DataPort *_portD4;
Hd44780DataPort *_portD5;
Hd44780DataPort *_portD6;
Hd44780DataPort *_portD7;

Hd44780Bits bits;

void hd44780SetBit(Hd44780Port *lcdPort) {
    sbi(*lcdPort->avrPort, lcdPort->avrPin);
}

void hd44780ClearBit(Hd44780Port *lcdPort) {
    cbi(*lcdPort->avrPort, lcdPort->avrPin);
}

void hd44780SetDataBit(Hd44780DataPort *lcdPort) {
    sbi(*lcdPort->avrPort, lcdPort->avrPin);
}

void hd44780ClearDataBit(Hd44780DataPort *lcdPort) {
    cbi(*lcdPort->avrPort, lcdPort->avrPin);
}

void hd44780SetDdrBit(Hd44780DataPort *lcdPort) {
    sbi(*lcdPort->avrDdrPort, lcdPort->avrPin);
}

void hd44780ClearDdrBit(Hd44780DataPort *lcdPort) {
    cbi(*lcdPort->avrDdrPort, lcdPort->avrPin);
}

void hd44780WriteBits(uint8_t data, uint8_t startBit, uint8_t endBit) {
    for (int bit = startBit; bit <= endBit; bit++) {
        Hd44780DataPort *port;

        switch (bit) {
            case 0: port = _portD0;
                break;
            case 1: port = _portD1;
                break;
            case 2: port = _portD2;
                break;
            case 3: port = _portD3;
                break;
            case 4: port = _portD4;
                break;
            case 5: port = _portD5;
                break;
            case 6: port = _portD6;
                break;
            case 7: port = _portD7;
                break;
        }

        if (((data >> bit) & 1) == 1) {
            hd44780SetDataBit(port);
        } else {
            hd44780ClearDataBit(port);
        }
    }
}

uint8_t hd44780ReadBits(uint8_t startBit, uint8_t endBit) {
    uint8_t data = 0;

    for (int bit = startBit; bit <= endBit; bit++) {
        Hd44780DataPort *port;

        switch (bit) {
            case 0: port = _portD0;
                break;
            case 1: port = _portD1;
                break;
            case 2: port = _portD2;
                break;
            case 3: port = _portD3;
                break;
            case 4: port = _portD4;
                break;
            case 5: port = _portD5;
                break;
            case 6: port = _portD6;
                break;
            case 7: port = _portD7;
                break;
        }

        int mask = 1 << port->avrPin;
        int result = *port->avrPinPort & mask;

        if (result != 0) {
            sbi(data, bit);
        } else {
            cbi(data, bit);
        }
    }

    return data;
}

void hd44780Trigger() {
    hd44780SetBit(_portE);
    _delay_ms(1);
    hd44780ClearBit(_portE);
}

uint8_t hd44780TriggerRead(uint8_t startBit, uint8_t endBit) {
    hd44780SetBit(_portE);
    _delay_ms(1);
    uint8_t result = hd44780ReadBits(startBit, endBit);
    hd44780ClearBit(_portE);

    return result;
}

void hd44780Send(uint8_t data) {
    if (bits == HD44780_EIGHT) {
        hd44780WriteBits(data, 0, 7);
        hd44780Trigger();
    } else if (bits == HD44780_FOUR) {
        hd44780WriteBits(data, 4, 7);
        hd44780Trigger();
        data <<= 4;
        hd44780WriteBits(data, 4, 7);
        hd44780Trigger();
    }
}

uint8_t hd44780ReadBitData() {
    if (bits == HD44780_EIGHT) {
        return hd44780TriggerRead(0, 7);
    } else if (bits == HD44780_FOUR) {
        uint8_t highOrder = hd44780TriggerRead(4, 7);
        uint8_t lowOrder = hd44780TriggerRead(4, 7) >> 4;

        return highOrder | lowOrder;
    }
}

void hd44780SetWriteMode() {
    if (bits == HD44780_EIGHT) {
        hd44780SetDdrBit(_portD0);
        hd44780SetDdrBit(_portD1);
        hd44780SetDdrBit(_portD2);
        hd44780SetDdrBit(_portD3);

        hd44780ClearDataBit(_portD0);
        hd44780ClearDataBit(_portD1);
        hd44780ClearDataBit(_portD2);
        hd44780ClearDataBit(_portD3);
    }

    hd44780SetDdrBit(_portD4);
    hd44780SetDdrBit(_portD5);
    hd44780SetDdrBit(_portD6);
    hd44780SetDdrBit(_portD7);

    hd44780ClearDataBit(_portD4);
    hd44780ClearDataBit(_portD5);
    hd44780ClearDataBit(_portD6);
    hd44780ClearDataBit(_portD7);

    hd44780ClearBit(_portRw);
}

void hd44780SetReadMode() {
    if (bits == HD44780_EIGHT) {
        hd44780ClearDdrBit(_portD0);
        hd44780ClearDdrBit(_portD1);
        hd44780ClearDdrBit(_portD2);
        hd44780ClearDdrBit(_portD3);

        hd44780SetDataBit(_portD0);
        hd44780SetDataBit(_portD1);
        hd44780SetDataBit(_portD2);
        hd44780SetDataBit(_portD3);
    }

    hd44780ClearDdrBit(_portD4);
    hd44780ClearDdrBit(_portD5);
    hd44780ClearDdrBit(_portD6);
    hd44780ClearDdrBit(_portD7);

    hd44780SetDataBit(_portD4);
    hd44780SetDataBit(_portD5);
    hd44780SetDataBit(_portD6);
    hd44780SetDataBit(_portD7);

    hd44780SetBit(_portRw);
}

void hd44780SetCommandMode() {
    hd44780ClearBit(_portRs);
}

void hd44780SetDataMode() {
    hd44780SetBit(_portRs);
}

uint8_t hd44780ReadBusyFlagAndAddress() {
    hd44780SetCommandMode();
    hd44780SetReadMode();

    return hd44780ReadBitData();
}

uint8_t hd44780ReadData() {
    hd44780SetDataMode();
    hd44780SetReadMode();

    return hd44780ReadBitData();
}

void hd44780Init8Bit(
        Hd44780Port *portRs,
        Hd44780Port *portRw,
        Hd44780Port *portE,
        Hd44780DataPort *portD0,
        Hd44780DataPort *portD1,
        Hd44780DataPort *portD2,
        Hd44780DataPort *portD3,
        Hd44780DataPort *portD4,
        Hd44780DataPort *portD5,
        Hd44780DataPort *portD6,
        Hd44780DataPort *portD7
) {
    _portRs = portRs;
    _portRw = portRw;
    _portE = portE;
    _portD0 = portD0;
    _portD1 = portD1;
    _portD2 = portD2;
    _portD3 = portD3;
    _portD4 = portD4;
    _portD5 = portD5;
    _portD6 = portD6;
    _portD7 = portD7;
    bits = HD44780_EIGHT;

    _delay_ms(40);
    hd44780SetWriteMode();
    hd44780SetCommandMode();

    hd44780WriteBits(0b00110000, 0, 7);

    hd44780Trigger();
    _delay_ms(5);
    hd44780Trigger();
    _delay_ms(5);
    hd44780Trigger();
}

void hd44780Init4Bit(
        Hd44780Port *portRs,
        Hd44780Port *portRw,
        Hd44780Port *portE,
        Hd44780DataPort *portD4,
        Hd44780DataPort *portD5,
        Hd44780DataPort *portD6,
        Hd44780DataPort *portD7
) {
    _portRs = portRs;
    _portRw = portRw;
    _portE = portE;
    _portD4 = portD4;
    _portD5 = portD5;
    _portD6 = portD6;
    _portD7 = portD7;
    bits = HD44780_FOUR;

    _delay_ms(40);
    hd44780SetWriteMode();
    hd44780SetCommandMode();

    hd44780WriteBits(0b00110000, 4, 7);

    hd44780Trigger();
    _delay_ms(5);
    hd44780Trigger();
    _delay_ms(5);
    hd44780Trigger();
    _delay_ms(5);
    hd44780WriteBits(0b00100000, 4, 7);
    hd44780Trigger();
}

void hd44780CheckBusyFlag() {
    hd44780SetCommandMode();
    hd44780SetReadMode();

    while (true) {
        uint8_t data = hd44780ReadBitData();
        int mask = 1 << 7;
        int result = data & mask;

        if (result == 0) {
            break;
        }
    }
}

void hd44780FunctionSet(Hd44780FontSize fontSize, Hd44780LineCount lineCount) {
    hd44780CheckBusyFlag();
    hd44780SetCommandMode();
    hd44780SetWriteMode();

    uint8_t data = 0;

    cbi(data, 0);
    cbi(data, 1);

    if (fontSize == HD44780_SMALL) {
        cbi(data, 2);
    } else if (fontSize == HD44780_LARGE) {
        sbi(data, 2);
    }

    if (lineCount == HD44780_ONE) {
        cbi(data, 3);
    } else if (lineCount == HD44780_TWO) {
        sbi(data, 3);

    }

    if (bits == HD44780_FOUR) {
        cbi(data, 4);
    } else if (bits == HD44780_EIGHT) {
        sbi(data, 4);
    }

    sbi(data, 5);
    cbi(data, 6);
    cbi(data, 7);

    hd44780Send(data);
}

void hd44780ClearScreen() {
    hd44780CheckBusyFlag();
    hd44780SetCommandMode();
    hd44780SetWriteMode();

    hd44780Send(0b00000001);
    hd44780SetDdramAddress(0);
}

void hd44780ReturnHome() {
    hd44780CheckBusyFlag();
    hd44780SetCommandMode();
    hd44780SetWriteMode();

    hd44780Send(0b00000010);
}

void hd44780EntryModeSet(bool shiftScreen, Hd44780AddressShiftDirection addressShiftDirection) {
    hd44780CheckBusyFlag();
    hd44780SetCommandMode();
    hd44780SetWriteMode();

    uint8_t data = 0;

    if (shiftScreen == false) {
        cbi(data, 0);
    } else if (shiftScreen == true) {
        sbi(data, 0);
    }

    if (addressShiftDirection == HD44780_DECREMENT) {
        cbi(data, 1);
    } else if (addressShiftDirection == HD44780_INCREMENT) {
        sbi(data, 1);
    }

    sbi(data, 2);
    cbi(data, 3);
    cbi(data, 4);
    cbi(data, 5);
    cbi(data, 6);
    cbi(data, 7);

    hd44780Send(data);
}

void hd44780DisplayOnOffControl(bool cursorBlinking, bool showCursor, bool displayOn) {
    hd44780CheckBusyFlag();
    hd44780SetCommandMode();
    hd44780SetWriteMode();

    uint8_t data = 0;

    if (cursorBlinking == true) {
        sbi(data, 0);
    } else if (cursorBlinking == false) {
        cbi(data, 0);
    }

    if (showCursor == true) {
        sbi(data, 1);
    } else if (showCursor == false) {
        cbi(data, 1);
    }

    if (displayOn == true) {
        sbi(data, 2);
    } else if (displayOn == false) {
        cbi(data, 2);
    }

    sbi(data, 3);
    cbi(data, 4);
    cbi(data, 5);
    cbi(data, 6);
    cbi(data, 7);

    hd44780Send(data);
}

void hd44780CursorOrDisplayShift(Hd44780ShiftDirection shiftDirection, Hd44780ShiftMode shiftMode) {
    hd44780CheckBusyFlag();
    hd44780SetCommandMode();
    hd44780SetWriteMode();

    uint8_t data = 0;

    cbi(data, 0);
    cbi(data, 1);

    if (shiftDirection == HD44780_LEFT) {
        cbi(data, 2);
    } else if (shiftDirection == HD44780_RIGHT) {
        sbi(data, 2);
    }

    if (shiftMode == HD44780_CURSOR) {
        hd44780ClearDataBit(_portD3);
        cbi(data, 3);
    } else if (shiftMode == HD44780_DISPLAY) {
        sbi(data, 3);
    }

    sbi(data, 4);
    cbi(data, 5);
    cbi(data, 6);
    cbi(data, 7);

    hd44780Send(data);
}

void hd44780SetDdramAddress(uint8_t address) {
    hd44780CheckBusyFlag();
    hd44780SetCommandMode();
    hd44780SetWriteMode();

    sbi(address, 7);
    hd44780Send(address);
}

void hd44780SetCgramAddress(uint8_t address) {
    hd44780CheckBusyFlag();
    hd44780SetCommandMode();
    hd44780SetWriteMode();

    sbi(address, 6);
    cbi(address, 7);

    hd44780Send(address);
}

void hd44780SendData(uint8_t data) {
    hd44780SetDataMode();
    hd44780SetWriteMode();

    hd44780Send(data);
}

void hd44780SendString(char *string) {
    while (*string != '\0') {
        hd44780SendData((uint8_t) *string);
        string++;
    }
}