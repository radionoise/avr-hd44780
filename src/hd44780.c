#include <hd44780.h>
#include <util/delay.h>
#include <stddef.h>

#define sbi(port,bit) (port) |= (1 << (bit))
#define cbi(port,bit) (port) &= ~(1 << (bit))

typedef enum Hd44780BitMode {
    HD44780_EIGHT,
    HD44780_FOUR
} Hd44780BitMode;

void hd44780SetBit(Hd44780Port *lcdPort) {
    sbi(*lcdPort->avrPort, lcdPort->avrPin);
}

void hd44780ClearBit(Hd44780Port *lcdPort) {
    cbi(*lcdPort->avrPort, lcdPort->avrPin);
}

void hd44780SetDdrBit(Hd44780Port *lcdPort) {
    sbi(*lcdPort->avrDdrPort, lcdPort->avrPin);
}

void hd44780ClearDdrBit(Hd44780Port *lcdPort) {
    cbi(*lcdPort->avrDdrPort, lcdPort->avrPin);
}

void hd44780WriteBits(uint8_t data, uint8_t startBit, uint8_t endBit, Hd44780Ports *ports) {
    for (int bit = startBit; bit <= endBit; bit++) {
        Hd44780Port *port;

        switch (bit) {
            case 0: port =ports->portD0;
                break;
            case 1: port = ports->portD1;
                break;
            case 2: port = ports->portD2;
                break;
            case 3: port = ports->portD3;
                break;
            case 4: port = ports->portD4;
                break;
            case 5: port = ports->portD5;
                break;
            case 6: port = ports->portD6;
                break;
            case 7: port = ports->portD7;
                break;
        }

        if (((data >> bit) & 1) == 1) {
            hd44780SetBit(port);
        } else {
            hd44780ClearBit(port);
        }
    }
}

uint8_t hd44780ReadBits(uint8_t startBit, uint8_t endBit, Hd44780Ports *ports) {
    uint8_t data = 0;

    for (int bit = startBit; bit <= endBit; bit++) {
        Hd44780Port *port;

        switch (bit) {
            case 0: port = ports->portD0;
                break;
            case 1: port = ports->portD1;
                break;
            case 2: port = ports->portD2;
                break;
            case 3: port = ports->portD3;
                break;
            case 4: port = ports->portD4;
                break;
            case 5: port = ports->portD5;
                break;
            case 6: port = ports->portD6;
                break;
            case 7: port = ports->portD7;
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

void hd44780Trigger(Hd44780Ports *ports) {
    hd44780SetDdrBit(ports->portE);
    hd44780SetBit(ports->portE);
    _delay_ms(1);
    hd44780ClearBit(ports->portE);
}

uint8_t hd44780TriggerRead(uint8_t startBit, uint8_t endBit, Hd44780Ports *ports) {
    hd44780SetBit(ports->portE);
    _delay_ms(1);
    uint8_t result = hd44780ReadBits(startBit, endBit, ports);
    hd44780ClearBit(ports->portE);

    return result;
}

Hd44780BitMode getBitOperationMode(Hd44780Ports *ports) {
    return ports->portD0 == NULL ||
            ports->portD1 == NULL ||
            ports->portD2 == NULL ||
            ports->portD3 == NULL ? HD44780_FOUR : HD44780_EIGHT;
}

void hd44780Send(uint8_t data, Hd44780Ports *ports) {
    Hd44780BitMode mode = getBitOperationMode(ports);
    if (mode == HD44780_EIGHT) {
        hd44780WriteBits(data, 0, 7, ports);
        hd44780Trigger(ports);
    } else if (mode == HD44780_FOUR) {
        hd44780WriteBits(data, 4, 7, ports);
        hd44780Trigger(ports);
        data <<= 4;
        hd44780WriteBits(data, 4, 7, ports);
        hd44780Trigger(ports);
    }
}

uint8_t hd44780ReadBitData(Hd44780Ports *ports) {
    Hd44780BitMode mode = getBitOperationMode(ports);
    if (mode == HD44780_EIGHT) {
        return hd44780TriggerRead(0, 7, ports);
    } else if (mode == HD44780_FOUR) {
        uint8_t highOrder = hd44780TriggerRead(4, 7, ports);
        uint8_t lowOrder = hd44780TriggerRead(4, 7, ports) >> 4;

        return highOrder | lowOrder;
    }
}

void hd44780SetWriteMode(Hd44780Ports *ports) {
    Hd44780BitMode mode = getBitOperationMode(ports);

    if (mode == HD44780_EIGHT) {
        hd44780SetDdrBit(ports->portD0);
        hd44780SetDdrBit(ports->portD1);
        hd44780SetDdrBit(ports->portD2);
        hd44780SetDdrBit(ports->portD3);

        hd44780ClearBit(ports->portD0);
        hd44780ClearBit(ports->portD1);
        hd44780ClearBit(ports->portD2);
        hd44780ClearBit(ports->portD3);
    }

    hd44780SetDdrBit(ports->portD4);
    hd44780SetDdrBit(ports->portD5);
    hd44780SetDdrBit(ports->portD6);
    hd44780SetDdrBit(ports->portD7);

    hd44780ClearBit(ports->portD4);
    hd44780ClearBit(ports->portD5);
    hd44780ClearBit(ports->portD6);
    hd44780ClearBit(ports->portD7);

    hd44780SetDdrBit(ports->portRw);
    hd44780ClearBit(ports->portRw);
}

void hd44780SetReadMode(Hd44780Ports *ports) {
    Hd44780BitMode mode = getBitOperationMode(ports);

    if (mode == HD44780_EIGHT) {
        hd44780ClearDdrBit(ports->portD0);
        hd44780ClearDdrBit(ports->portD1);
        hd44780ClearDdrBit(ports->portD2);
        hd44780ClearDdrBit(ports->portD3);

        hd44780SetBit(ports->portD0);
        hd44780SetBit(ports->portD1);
        hd44780SetBit(ports->portD2);
        hd44780SetBit(ports->portD3);
    }

    hd44780ClearDdrBit(ports->portD4);
    hd44780ClearDdrBit(ports->portD5);
    hd44780ClearDdrBit(ports->portD6);
    hd44780ClearDdrBit(ports->portD7);

    hd44780SetBit(ports->portD4);
    hd44780SetBit(ports->portD5);
    hd44780SetBit(ports->portD6);
    hd44780SetBit(ports->portD7);

    hd44780SetDdrBit(ports->portRw);
    hd44780SetBit(ports->portRw);
}

void hd44780SetCommandMode(Hd44780Ports *ports) {
    hd44780SetDdrBit(ports->portRs);
    hd44780ClearBit(ports->portRs);
}

void hd44780SetDataMode(Hd44780Ports *ports) {
    hd44780SetDdrBit(ports->portRw);
    hd44780SetBit(ports->portRs);
}

uint8_t hd44780ReadBusyFlagAndAddress(Hd44780Ports *ports) {
    hd44780SetCommandMode(ports);
    hd44780SetReadMode(ports);

    return hd44780ReadBitData(ports);
}

uint8_t hd44780ReadData(Hd44780Ports *ports) {
    hd44780SetDataMode(ports);
    hd44780SetReadMode(ports);

    return hd44780ReadBitData(ports);
}

void hd44780Init(Hd44780Ports *ports) {
    Hd44780BitMode mode = getBitOperationMode(ports);

    _delay_ms(40);
    hd44780SetWriteMode(ports);
    hd44780SetCommandMode(ports);

    if (mode == HD44780_EIGHT) {
        hd44780WriteBits(0b00110000, 0, 7, ports);

        hd44780Trigger(ports);
        _delay_ms(5);
        hd44780Trigger(ports);
        _delay_ms(5);
        hd44780Trigger(ports);
    } else if (mode == HD44780_FOUR) {
        hd44780WriteBits(0b00110000, 4, 7, ports);

        hd44780Trigger(ports);
        _delay_ms(5);
        hd44780Trigger(ports);
        _delay_ms(5);
        hd44780Trigger(ports);
        _delay_ms(5);
        hd44780WriteBits(0b00100000, 4, 7, ports);
        hd44780Trigger(ports);
    }
}

void hd44780CheckBusyFlag(Hd44780Ports *ports) {
    hd44780SetCommandMode(ports);
    hd44780SetReadMode(ports);

    while (true) {
        uint8_t data = hd44780ReadBitData(ports);
        int mask = 1 << 7;
        int result = data & mask;

        if (result == 0) {
            break;
        }
    }
}

void hd44780FunctionSet(Hd44780FontSize fontSize, Hd44780LineCount lineCount, Hd44780Ports *ports) {
    hd44780CheckBusyFlag(ports);
    hd44780SetCommandMode(ports);
    hd44780SetWriteMode(ports);

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

    Hd44780BitMode mode = getBitOperationMode(ports);
    if (mode == HD44780_FOUR) {
        cbi(data, 4);
    } else if (mode == HD44780_EIGHT) {
        sbi(data, 4);
    }

    sbi(data, 5);
    cbi(data, 6);
    cbi(data, 7);

    hd44780Send(data, ports);
}

void hd44780ClearScreen(Hd44780Ports *ports) {
    hd44780CheckBusyFlag(ports);
    hd44780SetCommandMode(ports);
    hd44780SetWriteMode(ports);

    hd44780Send(0b00000001, ports);
    hd44780SetDdramAddress(0, ports);
}

void hd44780ReturnHome(Hd44780Ports *ports) {
    hd44780CheckBusyFlag(ports);
    hd44780SetCommandMode(ports);
    hd44780SetWriteMode(ports);

    hd44780Send(0b00000010, ports);
}

void hd44780EntryModeSet(bool shiftScreen, Hd44780AddressShiftDirection addressShiftDirection, Hd44780Ports *ports) {
    hd44780CheckBusyFlag(ports);
    hd44780SetCommandMode(ports);
    hd44780SetWriteMode(ports);

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

    hd44780Send(data, ports);
}

void hd44780DisplayOnOffControl(bool cursorBlinking, bool showCursor, bool displayOn, Hd44780Ports *ports) {
    hd44780CheckBusyFlag(ports);
    hd44780SetCommandMode(ports);
    hd44780SetWriteMode(ports);

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

    hd44780Send(data, ports);
}

void hd44780CursorOrDisplayShift(Hd44780ShiftDirection shiftDirection, Hd44780ShiftMode shiftMode, Hd44780Ports *ports) {
    hd44780CheckBusyFlag(ports);
    hd44780SetCommandMode(ports);
    hd44780SetWriteMode(ports);

    uint8_t data = 0;

    cbi(data, 0);
    cbi(data, 1);

    if (shiftDirection == HD44780_LEFT) {
        cbi(data, 2);
    } else if (shiftDirection == HD44780_RIGHT) {
        sbi(data, 2);
    }

    if (shiftMode == HD44780_CURSOR) {
        hd44780ClearBit(ports->portD3);
        cbi(data, 3);
    } else if (shiftMode == HD44780_DISPLAY) {
        sbi(data, 3);
    }

    sbi(data, 4);
    cbi(data, 5);
    cbi(data, 6);
    cbi(data, 7);

    hd44780Send(data, ports);
}

void hd44780SetDdramAddress(uint8_t address, Hd44780Ports *ports) {
    hd44780CheckBusyFlag(ports);
    hd44780SetCommandMode(ports);
    hd44780SetWriteMode(ports);

    sbi(address, 7);
    hd44780Send(address, ports);
}

void hd44780SetCgramAddress(uint8_t address, Hd44780Ports *ports) {
    hd44780CheckBusyFlag(ports);
    hd44780SetCommandMode(ports);
    hd44780SetWriteMode(ports);

    sbi(address, 6);
    cbi(address, 7);

    hd44780Send(address, ports);
}

void hd44780SendData(uint8_t data, Hd44780Ports *ports) {
    hd44780SetDataMode(ports);
    hd44780SetWriteMode(ports);

    hd44780Send(data, ports);
}

void hd44780SendString(char *string, Hd44780Ports *ports) {
    while (*string != '\0') {
        hd44780SendData((uint8_t) *string, ports);
        string++;
    }
}