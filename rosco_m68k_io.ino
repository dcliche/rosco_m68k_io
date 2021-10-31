/*  PS2Keyboard library example

  PS2Keyboard now requries both pins specified for begin()

  keyboard.begin(data_pin, irq_pin);

  Valid irq pins:
     Arduino Uno:  2, 3
     Arduino Due:  All pins, except 13 (LED)
     Arduino Mega: 2, 3, 18, 19, 20, 21
     Teensy 2.0:   All pins, except 13 (LED)
     Teensy 2.0:   5, 6, 7, 8
     Teensy 1.0:   0, 1, 2, 3, 4, 6, 7, 16
     Teensy++ 2.0: 0, 1, 2, 3, 18, 19, 36, 37
     Teensy++ 1.0: 0, 1, 2, 3, 18, 19, 36, 37
     Sanguino:     2, 10, 11

  for more information you can read the original wiki in arduino.cc
  at http://www.arduino.cc/playground/Main/PS2Keyboard
  or http://www.pjrc.com/teensy/td_libs_PS2Keyboard.html

  Like the Original library and example this is under LGPL license.

  Modified by Cuninganreset@gmail.com on 2010-03-22
  Modified by Paul Stoffregen <paul@pjrc.com> June 2010
*/

#include "ps2_keyboard.h"

const int ps2_data_pin = 4;
const int ps2_clk_pin = 3;

const int rosco_mosi = 51;
const int rosco_miso = 50;
const int rosco_sck = 52;

PS2Keyboard keyboard;

void setup()
{

    pinMode(rosco_mosi, INPUT);
    pinMode(rosco_miso, OUTPUT);
    pinMode(rosco_sck, INPUT);

    delay(1000);
    keyboard.begin(ps2_data_pin, ps2_clk_pin);
    Serial.begin(9600);
    Serial.println("The rosco_m68k I/O interface is ready.");
}

void send_bit(int b)
{
    // wait until sck high
    while (digitalRead(rosco_sck) != HIGH)
        ;

    digitalWrite(rosco_miso, b ? HIGH : LOW);

    // wait until sck low
    while (digitalRead(rosco_sck) != LOW)
        ;
}

void send_char(char c)
{
    // send start bit
    send_bit(1);

    for (int i = 0; i < 8; ++i)
    {
        send_bit(c & 0x80);
        c <<= 1;
    }
}

void loop()
{
    char c = -1;

    if (keyboard.available())
    {

        // read the next key
        c = keyboard.read();

        // check for some of the special keys
        if (c == PS2_ENTER)
        {
            Serial.println();
        }
        else if (c == PS2_TAB)
        {
            Serial.print("[Tab]");
        }
        else if (c == PS2_ESC)
        {
            Serial.print("[ESC]");
        }
        else if (c == PS2_PAGEDOWN)
        {
            Serial.print("[PgDn]");
        }
        else if (c == PS2_PAGEUP)
        {
            Serial.print("[PgUp]");
        }
        else if (c == PS2_LEFTARROW)
        {
            Serial.print("[Left]");
        }
        else if (c == PS2_RIGHTARROW)
        {
            Serial.print("[Right]");
        }
        else if (c == PS2_UPARROW)
        {
            Serial.print("[Up]");
        }
        else if (c == PS2_DOWNARROW)
        {
            Serial.print("[Down]");
        }
        else if (c == PS2_DELETE)
        {
            Serial.print("[Del]");
        }
    }

    if (c > 0)
    {
        send_char(c);
    }
    else
    {
        send_char(0);
    }
}
