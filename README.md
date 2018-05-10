# HD44780 LCD library for AVR microcontroller

### Overview
This library supports 8-bit and 4-bit operation mode.
You can use any port on your AVR microcontroller

### Example
This example shows 8-bit operation mode.

```
uint8_t allOut = 0b11111111;
DDRA = allOut;
DDRB = allOut;
DDRC = allOut;
DDRD = allOut;

LcdPort portRs = {&PORTA, PIN0};
LcdPort portRw = {&PORTA, PIN1};
LcdPort portE = {&PORTA, PIN2};
LcdDataPort portD0 = {&DDRB, &PORTB, &PINB, PIN0};
LcdDataPort portD1 = {&DDRB, &PORTB, &PINB, PIN1};
LcdDataPort portD2 = {&DDRB, &PORTB, &PINB, PIN2};
LcdDataPort portD3 = {&DDRB, &PORTB, &PINB, PIN3};
LcdDataPort portD4 = {&DDRB, &PORTB, &PINB, PIN4};
LcdDataPort portD5 = {&DDRB, &PORTB, &PINB, PIN5};
LcdDataPort portD6 = {&DDRB, &PORTB, &PINB, PIN6};
LcdDataPort portD7 = {&DDRB, &PORTB, &PINB, PIN7};

lcdInit8Bit(&portRs, &portRw, &portE, &portD0, &portD1, &portD2, &portD3, &portD4, &portD5, &portD6, &portD7);
lcdFunctionSet(SMALL, TWO);
lcdEntryModeSet(FALSE, INCREMENT);
lcdClearScreen();
lcdDisplayOnOffControl(FALSE, FALSE, TRUE);
lcdSendString("Hello world!");
```