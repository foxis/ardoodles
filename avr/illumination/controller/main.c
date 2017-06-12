#include <inttypes.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "../common/hsv.h"
#include "../common/me.h"
#include "../common/protocol.h"
#include "../common/hsv.h"

/**
 *
 */
static volatile unsigned short gs_h[3];
static volatile unsigned char gs_s[3], gs_v[3], gs_dbank=0;


/**
 *
 */

void send_settings(void)
{
    packet_t p;
    unsigned char r,g,b;
    unsigned short buf[PACKET_NUM_BYTES];

    hsv2rgb(gs_h, gs_s, gs_v, &r, &g, &b);

    p.snRGB.addr = PACKET_ADDR_BROADCAST;
    p.snRGB.cmd = PRCMD_SET_RGB;
    p.snRGB.r = r;
    p.snRGB.g = g;
    p.snRGB.b = b;
    p.snRGB.crc = 0;
    p.snRGB.crc = crc8(p.bytes, sizeof(p.bytes));

    buf[0] = me_encode(p.bytes[0]);
    buf[1] = me_encode(p.bytes[1]);
    buf[2] = me_encode(p.bytes[2]);
    buf[3] = me_encode(p.bytes[3]);
    buf[4] = me_encode(p.bytes[4]);
}

int main(void)
{
    init_pins();

    while (1)
    {
        interpret_keyboard();

        send_settings();
    }

    return 0;
}
