#include <hd44780.h>
#include <util/delay.h>

LcdPort *_portRs;
LcdPort *_portRw;
LcdPort *_portE;

LcdDataPort *_portD0;
LcdDataPort *_portD1;
LcdDataPort *_portD2;
LcdDataPort *_portD3;
LcdDataPort *_portD4;
LcdDataPort *_portD5;
LcdDataPort *_portD6;
LcdDataPort *_portD7;

Bits bits;

void sbi(uint8_t *data, uint8_t bit) {
    *data |= (1 << bit);
}

void cbi(uint8_t *data, uint8_t bit) {
    *data &= ~(1 << bit);
}

void setBit(LcdPort *lcdPort) {
    *lcdPort->avrPort |= (1 << lcdPort->avrPin);
}

void clearBit(LcdPort *lcdPort) {
    *lcdPort->avrPort &= ~(1 << lcdPort->avrPin);
}

void setDataBit(LcdDataPort *lcdPort) {
    *lcdPort->avrPort |= (1 << lcdPort->avrPin);
}

void clearDataBit(LcdDataPort *lcdPort) {
    *lcdPort->avrPort &= ~(1 << lcdPort->avrPin);
}

void setDdrBit(LcdDataPort *lcdPort) {
    *lcdPort->avrDdrPort |= (1 << lcdPort->avrPin);
}

void clearDdrBit(LcdDataPort *lcdPort) {
    *lcdPort->avrDdrPort &= ~(1 << lcdPort->avrPin);
}

void writeBits(uint8_t data, uint8_t startBit, uint8_t endBit) {
    for (int bit = startBit; bit <= endBit; bit++) {
        LcdDataPort *port;

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
            setDataBit(port);
        } else {
            clearDataBit(port);
        }
    }
}

uint8_t readBits(uint8_t startBit, uint8_t endBit) {
    uint8_t data = 0;

    for (int bit = startBit; bit <= endBit; bit++) {
        LcdDataPort *port;

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
            sbi(&data, bit);
        } else {
            cbi(&data, bit);
        }
    }

    return data;
}

void trigger() {
    setBit(_portE);
    _delay_ms(1);
    clearBit(_portE);
}

uint8_t triggerRead(uint8_t startBit, uint8_t endBit) {
    setBit(_portE);
    _delay_ms(1);
    uint8_t result = readBits(startBit, endBit);
    clearBit(_portE);

    return result;
}

void send(uint8_t data) {
    if (bits == EIGHT) {
        writeBits(data, 0, 7);
        trigger();
    } else if (bits == FOUR) {
        writeBits(data, 4, 7);
        trigger();
        data <<= 4;
        writeBits(data, 4, 7);
        trigger();
    }
}

uint8_t readData() {
    if (bits == EIGHT) {
        return triggerRead(0, 7);
    } else if (bits == FOUR) {
        uint8_t highOrder = triggerRead(4, 7);
        uint8_t lowOrder = triggerRead(4, 7) >> 4;

        return highOrder | lowOrder;
    }
}

void setWriteMode() {
    if (bits == EIGHT) {
        setDdrBit(_portD0);
        setDdrBit(_portD1);
        setDdrBit(_portD2);
        setDdrBit(_portD3);

        clearDataBit(_portD0);
        clearDataBit(_portD1);
        clearDataBit(_portD2);
        clearDataBit(_portD3);
    }

    setDdrBit(_portD4);
    setDdrBit(_portD5);
    setDdrBit(_portD6);
    setDdrBit(_portD7);

    clearDataBit(_portD4);
    clearDataBit(_portD5);
    clearDataBit(_portD6);
    clearDataBit(_portD7);

    clearBit(_portRw);
}

void setReadMode() {
    if (bits == EIGHT) {
        clearDdrBit(_portD0);
        clearDdrBit(_portD1);
        clearDdrBit(_portD2);
        clearDdrBit(_portD3);

        setDataBit(_portD0);
        setDataBit(_portD1);
        setDataBit(_portD2);
        setDataBit(_portD3);
    }

    clearDdrBit(_portD4);
    clearDdrBit(_portD5);
    clearDdrBit(_portD6);
    clearDdrBit(_portD7);

    setDataBit(_portD4);
    setDataBit(_portD5);
    setDataBit(_portD6);
    setDataBit(_portD7);

    setBit(_portRw);
}

void setCommandMode() {
    clearBit(_portRs);
}

void setDataMode() {
    setBit(_portRs);
}

uint8_t lcdReadBusyFlagAndAddress() {
    setCommandMode();
    setReadMode();

    return readData();
}

uint8_t lcdReadData() {
    setDataMode();
    setReadMode();

    return readData();
}

void lcdInit8Bit(
        LcdPort *portRs,
        LcdPort *portRw,
        LcdPort *portE,
        LcdDataPort *portD0,
        LcdDataPort *portD1,
        LcdDataPort *portD2,
        LcdDataPort *portD3,
        LcdDataPort *portD4,
        LcdDataPort *portD5,
        LcdDataPort *portD6,
        LcdDataPort *portD7
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
    bits = EIGHT;

    _delay_ms(40);
    setWriteMode();
    setCommandMode();

    writeBits(0b00110000, 0, 7);

    trigger();
    _delay_ms(5);
    trigger();
    _delay_ms(5);
    trigger();
}

void lcdInit4Bit(
        LcdPort *portRs,
        LcdPort *portRw,
        LcdPort *portE,
        LcdDataPort *portD4,
        LcdDataPort *portD5,
        LcdDataPort *portD6,
        LcdDataPort *portD7
) {
    _portRs = portRs;
    _portRw = portRw;
    _portE = portE;
    _portD4 = portD4;
    _portD5 = portD5;
    _portD6 = portD6;
    _portD7 = portD7;
    bits = FOUR;

    _delay_ms(40);
    setWriteMode();
    setCommandMode();

    writeBits(0b00110000, 4, 7);

    trigger();
    _delay_ms(5);
    trigger();
    _delay_ms(5);
    trigger();
    _delay_ms(5);
    writeBits(0b00100000, 4, 7);
    trigger();
}

void checkBusyFlag() {
    setCommandMode();
    setReadMode();

    while (TRUE) {
        uint8_t data = readData();
        int mask = 1 << 7;
        int result = data & mask;

        if (result == 0) {
            break;
        }
    }
}

void lcdFunctionSet(FontSize fontSize, LineCount lineCount) {
    checkBusyFlag();
    setCommandMode();
    setWriteMode();

    uint8_t data = 0;

    cbi(&data, 0);
    cbi(&data, 1);

    if (fontSize == SMALL) {
        cbi(&data, 2);
    } else if (fontSize == LARGE) {
        sbi(&data, 2);
    }

    if (lineCount == ONE) {
        cbi(&data, 3);
    } else if (lineCount == TWO) {
        sbi(&data, 3);

    }

    if (bits == FOUR) {
        cbi(&data, 4);
    } else if (bits == EIGHT) {
        sbi(&data, 4);
    }

    sbi(&data, 5);
    cbi(&data, 6);
    cbi(&data, 7);

    send(data);
}

void lcdClearScreen() {
    checkBusyFlag();
    setCommandMode();
    setWriteMode();

    send(0b00000001);
}

void lcdReturnHome() {
    checkBusyFlag();
    setCommandMode();
    setWriteMode();

    send(0b00000010);
}

void lcdEntryModeSet(Boolean shiftScreen, AddressShiftDirection addressShiftDirection) {
    checkBusyFlag();
    setCommandMode();
    setWriteMode();

    uint8_t data = 0;

    if (shiftScreen == FALSE) {
        cbi(&data, 0);
    } else if (shiftScreen == TRUE) {
        sbi(&data, 0);
    }

    if (addressShiftDirection == DECREMENT) {
        cbi(&data, 1);
    } else if (addressShiftDirection == INCREMENT) {
        sbi(&data, 1);
    }

    sbi(&data, 2);
    cbi(&data, 3);
    cbi(&data, 4);
    cbi(&data, 5);
    cbi(&data, 6);
    cbi(&data, 7);

    send(data);
}

void lcdDisplayOnOffControl(Boolean cursorBlinking, Boolean showCursor, Boolean displayOn) {
    checkBusyFlag();
    setCommandMode();
    setWriteMode();

    uint8_t data = 0;

    if (cursorBlinking == TRUE) {
        sbi(&data, 0);
    } else if (cursorBlinking == FALSE) {
        cbi(&data, 0);
    }

    if (showCursor == TRUE) {
        sbi(&data, 1);
    } else if (showCursor == FALSE) {
        cbi(&data, 1);
    }

    if (displayOn == TRUE) {
        sbi(&data, 2);
    } else if (displayOn == FALSE) {
        cbi(&data, 2);
    }

    sbi(&data, 3);
    cbi(&data, 4);
    cbi(&data, 5);
    cbi(&data, 6);
    cbi(&data, 7);

    send(data);
}

void lcdCursorOrDisplayShift(ShiftDirection shiftDirection, ShiftMode shiftMode) {
    checkBusyFlag();
    setCommandMode();
    setWriteMode();

    uint8_t data = 0;

    cbi(&data, 0);
    cbi(&data, 1);

    if (shiftDirection == LEFT) {
        cbi(&data, 2);
    } else if (shiftDirection == RIGHT) {
        sbi(&data, 2);
    }

    if (shiftMode == CURSOR) {
        clearDataBit(_portD3);
        cbi(&data, 3);
    } else if (shiftMode == DISPLAY) {
        sbi(&data, 3);
    }

    sbi(&data, 4);
    cbi(&data, 5);
    cbi(&data, 6);
    cbi(&data, 7);

    send(data);
}

void lcdSetDdramAddress(uint8_t address) {
    checkBusyFlag();
    setCommandMode();
    setWriteMode();

    sbi(&address, 7);
    send(address);
}

void lcdSetCgramAddress(uint8_t address) {
    checkBusyFlag();
    setCommandMode();
    setWriteMode();

    sbi(&address, 6);
    cbi(&address, 7);

    send(address);
}

void lcdSendData(uint8_t data) {
    setDataMode();
    setWriteMode();

    send(data);
}

void lcdSendString(char *string) {
    while (*string != '\0') {
        lcdSendData((uint8_t) *string);
        string++;
    }
}