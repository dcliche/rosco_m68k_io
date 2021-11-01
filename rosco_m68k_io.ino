/* 
    Rosco m68k I/O interface
*/

#include "ps2_keyboard.h"
#include "ps2_mouse.h"

const int ps2_keyboard_data_pin = 4;
const int ps2_keyboard_clk_pin = 2;

const int ps2_mouse_data_pin = 5;
const int ps2_mouse_clk_pin = 3;

const int rosco_mosi = 51;
const int rosco_miso = 50;
const int rosco_sck = 52;

bool is_first_pass = true;
char last_mstat, last_mx, last_my;

PS2Keyboard keyboard;
PS2Mouse mouse;

void setup()
{

    pinMode(rosco_mosi, INPUT);
    pinMode(rosco_miso, OUTPUT);
    pinMode(rosco_sck, INPUT);

    //delay(1000);
    keyboard.begin(ps2_keyboard_data_pin, ps2_keyboard_clk_pin);
    mouse.begin(ps2_mouse_data_pin, ps2_mouse_clk_pin);

    Serial.begin(9600);
    Serial.println("The rosco_m68k I/O interface is ready.");
}

void send_bit(int b)
{
    // wait until sck low
    while (digitalRead(rosco_sck) != LOW)
        ;

    // wait until sck high
    while (digitalRead(rosco_sck) != HIGH)
        ;

    digitalWrite(rosco_miso, b ? HIGH : LOW);

    // wait until sck low
    while (digitalRead(rosco_sck) != LOW)
        ;

    digitalWrite(rosco_miso, LOW);
}

void send_byte(unsigned char c)
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
    // Keyboard
    unsigned char scancode = 0;
    scancode = keyboard.read();

    if (scancode)
    {
        send_byte('K');
        send_byte(scancode);
    }

    // Mouse
    char mstat, mx, my;
    mouse.read(&mstat, &mx, &my);

    if (is_first_pass || mstat != last_mstat || mx != last_mx || my != last_my)
    {
        send_byte('M');
        send_byte(mstat);
        send_byte(mx);
        send_byte(my);

        last_mstat = mstat;
        last_mx = mx;
        last_my = my;
    }

    is_first_pass = false;
}
