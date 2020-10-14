/*
 * Copyright (C) 2020 Daniel Efimenko
 *     github.com/Danya0x07
 */

/*------------------------------------------------------------
 * Portable interface to be implemented for specific platform. 
 *-----------------------------------------------------------*/

#ifndef _TM1637_PORT_H
#define _TM1637_PORT_H

// Include any headers you need to implement this interface.

typedef uint8_t uint_fast8_t;  // Can be removed if your headers provide this type.

/* TM1637 gpio control macro.
 * Attention: GPIO ports initialization must be done by user. */
#define _clk_low()	// SOME_PORT &= ~(1 << SOME_BIT) ...
#define _clk_high() 
#define _din_low()  
#define _din_high() 
#define _din_is_high() 

/* Timeout timer control macro.
 * Used in ack checking function. Can be stubbed, but if it is,
 * the ack checking function will block forever if the connection
 * to the TM1637 is broken.
 */
#define _us_timer_set(us)   ((void)0)
#define _us_timer_get()     (0xFF)

/* Ultraseconds delay function.
 * Can be stubbed with something that waits for more than 5 us.
 */
static void _delay_us(uint16_t us)
{
    _us_timer_set(0);
    while (_us_timer_get() < us) {}
}

#endif  // _TM1637_PORT_H
