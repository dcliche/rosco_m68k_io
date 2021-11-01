/*
 * ps2_mouse.h - a library to interface with ps2 mouse. See comments in ps2_mouse.cpp.
 * Written by Chris J. Kiick, January 2008.
 * Modified by Daniel Cliche.
 * Release into public domain.
 */

#ifndef ps2_mouse_h
#define ps2_mouse_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h" // for attachInterrupt, FALLING
#else
#include "WProgram.h"
#endif

class PS2Mouse
{
public:
	PS2Mouse();
	void begin(int data_pin, int clk_pin);
	void PS2Mouse::read(char *mstat,
						char *mx,
						char *my);

private:
	int _ps2clk;
	int _ps2data;
	void golo(int pin);
	void gohi(int pin);
	void write(unsigned char data);
	unsigned char read();
};

#endif /* ps2_mouse_h */
