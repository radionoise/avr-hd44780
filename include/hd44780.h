/*
 * HD44780 library for AVR.
 * Supports 8-bit and 4-bit operation mode.
 * Version 2.0.0
 */

#ifndef AVR_HD44780_HD44780_H
#define AVR_HD44780_HD44780_H

#include <stdbool.h>
#include <avr/io.h>

typedef struct Hd44780Port {
    volatile uint8_t * avrDdrPort;
    volatile uint8_t * avrPort;
    volatile uint8_t * avrPinPort;
    uint8_t avrPin;
} Hd44780Port;

typedef struct Hd44780Ports {
    Hd44780Port *portRs;
    Hd44780Port *portRw;
    Hd44780Port *portE;
    Hd44780Port *portD0; // not needed for 4-bit mode
    Hd44780Port *portD1; // not needed for 4-bit mode
    Hd44780Port *portD2; // not needed for 4-bit mode
    Hd44780Port *portD3; // not needed for 4-bit mode
    Hd44780Port *portD4;
    Hd44780Port *portD5;
    Hd44780Port *portD6;
    Hd44780Port *portD7;
} Hd44780Ports;

typedef enum Hd44780FontSize {
    HD44780_SMALL,
    HD44780_LARGE
} Hd44780FontSize;

typedef enum Hd44780LineCount {
    HD44780_ONE,
    HD44780_TWO
} Hd44780LineCount;

typedef enum Hd44780AddressShiftDirection {
    HD44780_DECREMENT,
    HD44780_INCREMENT
} Hd44780AddressShiftDirection;

typedef enum ShiftDirection {
    HD44780_LEFT,
    HD44780_RIGHT
} Hd44780ShiftDirection;

typedef enum Hd44780ShiftMode {
    HD44780_CURSOR,
    HD44780_DISPLAY
} Hd44780ShiftMode;

/*
 * Performs init sequence for LCD
 */
void hd44780Init(Hd44780Ports *ports);

/*
 * Sets LCD parameters:
 *
 * - fontSize - Sets the character font
 * - lineCount - Sets the number of display lines
 */
void hd44780FunctionSet(Hd44780FontSize fontSize, Hd44780LineCount lineCount, Hd44780Ports *ports);

/*
 * Writes space code 20H (character pattern for character code 20H must be a blank pattern)
 * into all DDRAM addresses. It then sets DDRAM address 0 into the address counter,
 * and returns the display to its original status if it was shifted.
 * In other words, the display disappears and the cursor or blinking goes to the left edge of the display
 * (in the first line if 2 lines are displayed).
 * It also sets Hd44780AddressShiftDirection to INCREMENT in entry mode. bool shiftScreen of entry mode does not change.
 */
void hd44780ClearScreen(Hd44780Ports *ports);

/*
 * Sets DDRAM address 0 into the address counter, and returns the display to its original status
 * if it was shifted. The DDRAM contents do not change.
 */
void hd44780ReturnHome(Hd44780Ports *ports);

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
void hd44780EntryModeSet(bool shiftScreen, Hd44780AddressShiftDirection addressShiftDirection, Hd44780Ports *ports);

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
void hd44780DisplayOnOffControl(bool cursorBlinking, bool showCursor, bool displayOn, Hd44780Ports *ports);

/*
 * Shifts the cursor position or display to the right or left without writing or reading display data.
 * This function is used to correct or search the display.
 * In a 2-line display, the cursor moves to the second line when it passes the 40th digit of the first line.
 * Note that the first and second line displays will shift at the same time.
 * When the displayed data is shifted repeatedly each line moves only horizontally.
 * The second line display does not shift into the first line position.
 * The address counter contents will not change if the only action performed is a display shift.
 */
void hd44780CursorOrDisplayShift(Hd44780ShiftDirection shiftDirection, Hd44780ShiftMode shiftMode, Hd44780Ports *ports);

/*
 * Sets the DDRAM address into the address counter. Data is then written to or read from the MPU for DDRAM.
 * For 1-line display, address can be 0x0 to 0x4f.
 * For 2-line display, address can be 0x0 to 0x27 for the first line, and 0x40 to 0x67.
 */
void hd44780SetDdramAddress(uint8_t address, Hd44780Ports *ports);

/*
 * Sets the CGRAM address into the address counter. Data is then written to or read from the MPU for CGRAM.
 */
void hd44780SetCgramAddress(uint8_t address, Hd44780Ports *ports);

/*
 * Writes data into DDRAM or CGRAM. The data can be a character or raw bytes.
 */
void hd44780SendData(uint8_t data, Hd44780Ports *ports);

/*
 * Writes array of characters.
 */
void hd44780SendString(char *string, Hd44780Ports *ports);

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
uint8_t hd44780ReadBusyFlagAndAddress(Hd44780Ports *ports);

/*
 * Reads data from DDRAM or CGRAM.
 */
uint8_t hd44780ReadData(Hd44780Ports *ports);

#endif //AVR_HD44780_HD44780_H
