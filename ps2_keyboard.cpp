/*
  PS2Keyboard.cpp - PS2Keyboard library
  Copyright (c) 2021 Free Software Foundation.  All right reserved.
  Written by Christian Weichel <info@32leaves.net>

  ** Mostly rewritten Paul Stoffregen <paul@pjrc.com> 2010, 2011
  ** Modified for use beginning with Arduino 13 by L. Abraham Smith, <n3bah@microcompdesign.com> * 
  ** Modified for easy interrup pin assignement on method begin(datapin,irq_pin). Cuningan <cuninganreset@gmail.com> **
  ** Modified by Daniel Cliche **

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "ps2_keyboard.h"

#define BUFFER_SIZE 45
static volatile uint8_t buffer[BUFFER_SIZE];
static volatile uint8_t head, tail;
static uint8_t DataPin;

// The ISR for the external interrupt
static void ps2interrupt(void)
{
	static uint8_t bitcount = 0;
	static uint8_t incoming = 0;
	static uint32_t prev_ms = 0;
	uint32_t now_ms;
	uint8_t n, val;

	val = digitalRead(DataPin);
	now_ms = millis();
	if (now_ms - prev_ms > 250)
	{
		bitcount = 0;
		incoming = 0;
	}
	prev_ms = now_ms;
	n = bitcount - 1;
	if (n <= 7)
	{
		incoming |= (val << n);
	}
	bitcount++;
	if (bitcount == 11)
	{
		uint8_t i = head + 1;
		if (i >= BUFFER_SIZE)
			i = 0;
		if (i != tail)
		{
			buffer[i] = incoming;
			head = i;
		}
		bitcount = 0;
		incoming = 0;
	}
}

uint8_t PS2Keyboard::read(void)
{
	uint8_t c, i;

	i = tail;
	if (i == head)
		return 0;
	i++;
	if (i >= BUFFER_SIZE)
		i = 0;
	c = buffer[i];
	tail = i;
	return c;
}

PS2Keyboard::PS2Keyboard()
{
	// nothing to do here, begin() does it all
}

void PS2Keyboard::begin(uint8_t data_pin, uint8_t irq_pin)
{
	uint8_t irq_num = 255;

	DataPin = data_pin;

	// initialize the pins
#ifdef INPUT_PULLUP
	pinMode(irq_pin, INPUT_PULLUP);
	pinMode(data_pin, INPUT_PULLUP);
#else
	pinMode(irq_pin, INPUT);
	digitalWrite(irq_pin, HIGH);
	pinMode(data_pin, INPUT);
	digitalWrite(data_pin, HIGH);
#endif

#ifdef CORE_INT_EVERY_PIN
	irq_num = irq_pin;

#else
	switch (irq_pin)
	{
#ifdef CORE_INT0_PIN
	case CORE_INT0_PIN:
		irq_num = 0;
		break;
#endif
#ifdef CORE_INT1_PIN
	case CORE_INT1_PIN:
		irq_num = 1;
		break;
#endif
#ifdef CORE_INT2_PIN
	case CORE_INT2_PIN:
		irq_num = 2;
		break;
#endif
#ifdef CORE_INT3_PIN
	case CORE_INT3_PIN:
		irq_num = 3;
		break;
#endif
#ifdef CORE_INT4_PIN
	case CORE_INT4_PIN:
		irq_num = 4;
		break;
#endif
#ifdef CORE_INT5_PIN
	case CORE_INT5_PIN:
		irq_num = 5;
		break;
#endif
#ifdef CORE_INT6_PIN
	case CORE_INT6_PIN:
		irq_num = 6;
		break;
#endif
#ifdef CORE_INT7_PIN
	case CORE_INT7_PIN:
		irq_num = 7;
		break;
#endif
#ifdef CORE_INT8_PIN
	case CORE_INT8_PIN:
		irq_num = 8;
		break;
#endif
#ifdef CORE_INT9_PIN
	case CORE_INT9_PIN:
		irq_num = 9;
		break;
#endif
#ifdef CORE_INT10_PIN
	case CORE_INT10_PIN:
		irq_num = 10;
		break;
#endif
#ifdef CORE_INT11_PIN
	case CORE_INT11_PIN:
		irq_num = 11;
		break;
#endif
#ifdef CORE_INT12_PIN
	case CORE_INT12_PIN:
		irq_num = 12;
		break;
#endif
#ifdef CORE_INT13_PIN
	case CORE_INT13_PIN:
		irq_num = 13;
		break;
#endif
#ifdef CORE_INT14_PIN
	case CORE_INT14_PIN:
		irq_num = 14;
		break;
#endif
#ifdef CORE_INT15_PIN
	case CORE_INT15_PIN:
		irq_num = 15;
		break;
#endif
#ifdef CORE_INT16_PIN
	case CORE_INT16_PIN:
		irq_num = 16;
		break;
#endif
#ifdef CORE_INT17_PIN
	case CORE_INT17_PIN:
		irq_num = 17;
		break;
#endif
#ifdef CORE_INT18_PIN
	case CORE_INT18_PIN:
		irq_num = 18;
		break;
#endif
#ifdef CORE_INT19_PIN
	case CORE_INT19_PIN:
		irq_num = 19;
		break;
#endif
#ifdef CORE_INT20_PIN
	case CORE_INT20_PIN:
		irq_num = 20;
		break;
#endif
#ifdef CORE_INT21_PIN
	case CORE_INT21_PIN:
		irq_num = 21;
		break;
#endif
#ifdef CORE_INT22_PIN
	case CORE_INT22_PIN:
		irq_num = 22;
		break;
#endif
#ifdef CORE_INT23_PIN
	case CORE_INT23_PIN:
		irq_num = 23;
		break;
#endif
	}
#endif

	head = 0;
	tail = 0;
	if (irq_num < 255)
	{
		attachInterrupt(irq_num, ps2interrupt, FALLING);
	}
}
