/*
 * ps2.cpp - an interface library for ps2 mouse.
 * limitations:
 *      we do not handle parity errors.
 *      The timing constants are hard coded from the spec. Data rate is
 *         not impressive.
 *      probably lots of room for optimization.
 */

#include "ps2_mouse.h"

/*
 * the clock and data pins can be wired directly to the clk and data pins
 * of the PS2 connector.  No external parts are needed.
 */
PS2Mouse::PS2Mouse()
{
}

void PS2Mouse::begin(int data_pin, int clk_pin)
{
	_ps2clk = clk_pin;
	_ps2data = data_pin;
	gohi(_ps2clk);
	gohi(_ps2data);

	write(0xff); // reset
	read();		 // ack byte
	read();		 // blank */
	read();		 // blank */
	write(0xf0); // remote mode
	read();		 // ack
	delayMicroseconds(100);
}

/*
 * according to some code I saw, these functions will
 * correctly set the clock and data pins for
 * various conditions.  It's done this way so you don't need
 * pullup resistors.
 */
void PS2Mouse::gohi(int pin)
{
	pinMode(pin, INPUT);
	digitalWrite(pin, HIGH);
}

void PS2Mouse::golo(int pin)
{
	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);
}

/* write a byte to the PS2 device */
void PS2Mouse::write(unsigned char data)
{
	unsigned char i;
	unsigned char parity = 1;

	gohi(_ps2data);
	gohi(_ps2clk);
	delayMicroseconds(300);
	golo(_ps2clk);
	delayMicroseconds(300);
	golo(_ps2data);
	delayMicroseconds(10);
	gohi(_ps2clk); // start bit
	/* wait for device to take control of clock */
	while (digitalRead(_ps2clk) == HIGH)
		; // this loop intentionally left blank
	// clear to send data
	for (i = 0; i < 8; i++)
	{
		if (data & 0x01)
		{
			gohi(_ps2data);
		}
		else
		{
			golo(_ps2data);
		}
		// wait for clock
		while (digitalRead(_ps2clk) == LOW)
			;
		while (digitalRead(_ps2clk) == HIGH)
			;
		parity = parity ^ (data & 0x01);
		data = data >> 1;
	}
	// parity bit
	if (parity)
	{
		gohi(_ps2data);
	}
	else
	{
		golo(_ps2data);
	}
	// clock cycle - like ack.
	while (digitalRead(_ps2clk) == LOW)
		;
	while (digitalRead(_ps2clk) == HIGH)
		;
	// stop bit
	gohi(_ps2data);
	delayMicroseconds(50);
	while (digitalRead(_ps2clk) == HIGH)
		;
	// mode switch
	while ((digitalRead(_ps2clk) == LOW) || (digitalRead(_ps2data) == LOW))
		;
	// hold up incoming data
	golo(_ps2clk);
}

/*
 * read a byte of data from the ps2 device.  Ignores parity.
 */
unsigned char PS2Mouse::read(void)
{
	unsigned char data = 0x00;
	unsigned char i;
	unsigned char bit = 0x01;

	// start clock
	gohi(_ps2clk);
	gohi(_ps2data);
	delayMicroseconds(50);
	while (digitalRead(_ps2clk) == HIGH)
		;
	delayMicroseconds(5); // not sure why.
	while (digitalRead(_ps2clk) == LOW)
		; // eat start bit
	for (i = 0; i < 8; i++)
	{
		while (digitalRead(_ps2clk) == HIGH)
			;
		if (digitalRead(_ps2data) == HIGH)
		{
			data = data | bit;
		}
		while (digitalRead(_ps2clk) == LOW)
			;
		bit = bit << 1;
	}
	// eat parity bit, ignore it.
	while (digitalRead(_ps2clk) == HIGH)
		;
	while (digitalRead(_ps2clk) == LOW)
		;
	// eat stop bit
	while (digitalRead(_ps2clk) == HIGH)
		;
	while (digitalRead(_ps2clk) == LOW)
		;
	golo(_ps2clk); // hold incoming data

	return data;
}

void PS2Mouse::read(char *mstat,
					char *mx,
					char *my)
{
	// get a reading from the mouse
	write(0xeb); // give me data!
	read();		 // ignore ack
	*mstat = read();
	*mx = read();
	*my = read();
}