/*
 * HD44780 library for AVR.
 * Supports 8-bit and 4-bit operation mode.
 */

#ifndef AVR_HD44780_HD44780_H
#define AVR_HD44780_HD44780_H

#include <stdbool.h>
#include <avr/io.h>

typedef struct LcdPort {
    volatile uint8_t * avrPort;
    uint8_t avrPin;
} LcdPort;

typedef struct LcdDataPort {
    volatile uint8_t * avrDdrPort;
    volatile uint8_t * avrPort;
    volatile uint8_t * avrPinPort;
    uint8_t avrPin;
} LcdDataPort;

typedef enum FontSize {
    SMALL,
    LARGE
} FontSize;

typedef enum LineCount {
    ONE,
    TWO
} LineCount;

typedef enum Bits {
    EIGHT,
    FOUR
} Bits;

typedef enum AddressShiftDirection {
    DECREMENT,
    INCREMENT
} AddressShiftDirection;

typedef enum ShiftDirection {
    LEFT,
    RIGHT
} ShiftDirection;

typedef enum ShiftMode {
    CURSOR,
    DISPLAY
} ShiftMode;

/*
 * Performs init sequence for LCD to operate in 8-bit mode
 */
void hd44780Init8Bit(
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
);

/*
 * Performs init sequence for LCD to operate in 4-bit mode
 */
void hd44780Init4Bit(
        LcdPort *portRs,
        LcdPort *portRw,
        LcdPort *portE,
        LcdDataPort *portD4,
        LcdDataPort *portD5,
        LcdDataPort *portD6,
        LcdDataPort *portD7
);

/*
 * Sets LCD parameters:
 *
 * - fontSize - Sets the character font
 * - lineCount - Sets the number of display lines
 */
void hd44780FunctionSet(FontSize fontSize, LineCount lineCount);

/*
 * Writes space code 20H (character pattern for character code 20H must be a blank pattern)
 * into all DDRAM addresses. It then sets DDRAM address 0 into the address counter,
 * and returns the display to its original status if it was shifted.
 * In other words, the display disappears and the cursor or blinking goes to the left edge of the display
 * (in the first line if 2 lines are displayed).
 * It also sets AddressShiftDirection to INCREMENT in entry mode. bool shiftScreen of entry mode does not change.
 */
void hd44780ClearScreen();

/*
 * Sets DDRAM address 0 into the address counter, and returns the display to its original status
 * if it was shifted. The DDRAM contents do not change.
 */
void hd44780ReturnHome();

/*
 * Sets entry mode:
 *
 * - addressShiftDirection - Increments or decrements the DDRAM address by 1 when a character code is
 * written into or read from DDRAM. The cursor or blinking moves to the right when incremented by 1
 * and to the left when decremented by 1. The same applies to writing and reading of CGRAM.
 *
 * - shiftScreen - Shifts the entire display either to the right or to the left.
 * The display does not shift if shiftScreen is false.
 * If shiftScreen is true, it will seem as if the cursor does not move but the display does.
 * The display does not shift when reading from DDRAM.
 * Also, writing into or reading out from CGRAM does not shift the display.
 */
void hd44780EntryModeSet(bool shiftScreen, AddressShiftDirection addressShiftDirection);

/*
 * Controls display:
 *
 * - cursorBlinking - The character indicated by the cursor blinks when cursorBlinking is true.
 * The blinking is displayed as switching between all blank dots and displayed characters.
 * The cursor and blinking can be set to display simultaneously.
 *
 * - showCursor - The cursor is displayed when showCursor is true and not displayed when set to false.
 * Even if the cursor disappears, the function of INCREMENT/DECREMENT or other specifications
 * will not change during display data write.
 * The cursor is displayed using 5 dots in the 8th line for 5 × 8 dot character font selection
 * and in the 11th line for the 5 × 10 dot character font selection.
 *
 * - displayOn - The display is on when set to true and off when set to false.
 * When off, the display data remains in DDRAM, but can be displayed instantly by setting displayOn to true.
 */
void hd44780DisplayOnOffControl(bool cursorBlinking, bool showCursor, bool displayOn);

/*
 * Shifts the cursor position or display to the right or left without writing or reading display data.
 * This function is used to correct or search the display.
 * In a 2-line display, the cursor moves to the second line when it passes the 40th digit of the first line.
 * Note that the first and second line displays will shift at the same time.
 * When the displayed data is shifted repeatedly each line moves only horizontally.
 * The second line display does not shift into the first line position.
 * The address counter contents will not change if the only action performed is a display shift.
 */
void hd44780CursorOrDisplayShift(ShiftDirection shiftDirection, ShiftMode shiftMode);

/*
 * Sets the DDRAM address into the address counter. Data is then written to or read from the MPU for DDRAM.
 * For 1-line display, address can be 0x0 to 0x4f.
 * For 2-line display, address can be 0x0 to 0x27 for the first line, and 0x40 to 0x67.
 */
void hd44780SetDdramAddress(uint8_t address);

/*
 * Sets the CGRAM address into the address counter. Data is then written to or read from the MPU for CGRAM.
 */
void hd44780SetCgramAddress(uint8_t address);

/*
 * Writes data into DDRAM or CGRAM. The data can be a character or raw bytes.
 */
void hd44780SendData(uint8_t data);

/*
 * Writes array of characters.
 */
void hd44780SendString(char *string);

/*
 * Reads the busy flag (7th bit) indicating that the system is now internally operating
 * on a previously received instruction and current address from the address counter.
 * If the busy flag (7th bit) is 1, the internal operation is in progress.
 * The next instruction will not be accepted until busy flag is reset to 0.
 * No need to check the busy flag explicitly before the instruction.
 * Every function in this library already checks the busy flag and proceeds on success.
 * At the same time, the value of the address counter in binary (7 bits) is read out.
 * This address counter is used by both CG and DDRAM addresses, and its value is determined by the previous instruction.
 * The address contents are the same as for instructions set CGRAM address and set DDRAM address.
 */
uint8_t hd44780ReadBusyFlagAndAddress();

/*
 * Reads data from DDRAM or CGRAM.
 */
uint8_t hd44780ReadData();

#endif //AVR_HD44780_HD44780_H
