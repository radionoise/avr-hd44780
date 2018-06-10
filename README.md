# HD44780 LCD library for AVR microcontroller

### Overview
This library supports 8-bit and 4-bit operation mode.
You can use any port on your AVR microcontroller

### Example
This example shows 8-bit operation mode.

```
Hd44780Port portRs = {&DDRA, &PORTA, &PINA, PIN0};
Hd44780Port portRw = {&DDRA, &PORTA, &PINA, PIN1};
Hd44780Port portE = {&DDRA, &PORTA, &PINA, PIN2};
Hd44780Port portD4 = {&DDRB, &PORTB, &PINB, PIN4};
Hd44780Port portD5 = {&DDRB, &PORTB, &PINB, PIN5};
Hd44780Port portD6 = {&DDRB, &PORTB, &PINB, PIN6};
Hd44780Port portD7 = {&DDRB, &PORTB, &PINB, PIN7};

Hd44780Ports hd44780Ports = {
        .portRs = &portRs,
        .portRw = &portRw,
        .portE = &portE,
        .portD4 = &portD4,
        .portD5 = &portD5,
        .portD6 = &portD6,
        .portD7 = &portD7,
};

hd44780Init(&hd44780Ports);
hd44780FunctionSet(HD44780_SMALL, HD44780_TWO, &hd44780Ports);
hd44780EntryModeSet(false, HD44780_INCREMENT, &hd44780Ports);
hd44780ClearScreen(&hd44780Ports);
hd44780DisplayOnOffControl(FALSE, FALSE, TRUE, &hd44780Ports);
hd44780SendString("Hello world!", &hd44780Ports);
```