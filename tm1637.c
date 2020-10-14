/*
 * Copyright (C) 2020 Daniel Efimenko
 *     github.com/Danya0x07
 */

#include "tm1637.h"
#include "tm1637_port.h"

// Command sets
#define CS_DATA     (1 << 6)
#define CS_DISPLAY  (2 << 6)
#define CS_ADDRESS  (3U << 6)  // 11000000

// Data command set bits
#define BIT_READKEY     1
#define BIT_NOAUTOINC   2
// #define BIT_TESTMODE 3  ???

// Display command set bits
#define BIT_DSS 3  // Display switch settings

/*
 *     A
 *    ---
 * F |   | B
 *    -G-
 * E |   | C
 *    ---
 *     D
 */
const uint8_t tm16_digits[0x10] = {
//    XGFEDCBA
    0b00111111,  // 0
    0b00000110,  // 1
    0b01011011,  // 2
    0b01001111,  // 3
    0b01100110,  // 4
    0b01101101,  // 5
    0b01111101,  // 6
    0b00000111,  // 7
    0b01111111,  // 8
    0b01101111,  // 9
    0b01110111,  // A
    0b01111100,  // b
    0b00111001,  // C
    0b01011110,  // d
    0b01111001,  // E
    0b01110001,  // F
};

static struct {
    uint8_t brightness;
    bool state;
} current = {1, 7};

static void transmission_start(void)
{
    _clk_high();
    _din_high();
    _delay_us(2);
    _din_low();
}

static void transmission_stop(void)
{
    _clk_low();
    _delay_us(2);
    _din_low();
    _delay_us(2);
    _clk_high();
    _delay_us(2);
    _din_high();
}

static inline void transmission_check_ack(void)
{
    _clk_low();
    _delay_us(5);
    _din_low();
    while (_din_is_high()) {}
    _din_high();
    _clk_high();
    _delay_us(2);
    _clk_low();
}

static void write_byte(uint8_t data)
{
    uint_fast8_t i;

    for (i = 0; i < 8; i++) {
        _clk_low();
        if (data & 1)
            _din_high();
        else
            _din_low();
        _delay_us(3);
        data >>= 1;
        _clk_high();
        _delay_us(3);
    }
    transmission_check_ack();
}

static void write_bytes(const uint8_t *bytes, uint8_t len)
{
    transmission_start();
    while (len--) {
        write_byte(*bytes++);
    }
    transmission_stop();
}

static void write_command(uint8_t command)
{
    transmission_start();
    write_byte(command);
    transmission_stop();
}

static void set_display(uint8_t brightness, bool state)
{
    write_command(CS_DISPLAY | brightness | state << BIT_DSS);
}

void tm1637_display_dec(int16_t number, bool dots)
{
    uint8_t bytes[5] = {0};
    uint8_t digit;

    if (number > 9999)
        number = 9999;
    else if (number < -999)
        number = -999;

    bytes[0] = CS_ADDRESS | 0;

    if (number < 0) {
        bytes[1] = TM16_MINUS;
        number = -number;
    } else {
        digit = number / 1000;
        bytes[1] = tm16_digits[digit];
        number -= digit * 1000;
    }
    digit = number / 100;
    bytes[2] = tm16_digits[digit] | dots << 7;
    number -= digit * 100;

    digit = number / 10;
    bytes[3] = tm16_digits[digit];
    number -= digit * 10;

    digit = number;
    bytes[4] = tm16_digits[digit];
    
    write_command(CS_DATA);  // cursor auto increment
    write_bytes(bytes, sizeof(bytes));
}

void tm1637_display_content(uint8_t content[4])
{
    uint_fast8_t i;

    write_command(CS_DATA);  // cursor auto increment
    transmission_start();
    write_byte(CS_ADDRESS | 0);
    for (i = 0; i < 4; i++)
        write_byte(content[i]);
    transmission_stop();
}

void tm1637_set_state(bool state)
{
    current.state = state;
    set_display(current.brightness, current.state);
}

bool tm1637_get_state(void)
{
    return current.state;
}

void tm1637_set_brightness(uint8_t brightness)
{
    current.brightness = brightness & 0x07;
    set_display(current.brightness, current.state);
}

uint8_t tm1637_get_brightness(void)
{
    return current.brightness;
}