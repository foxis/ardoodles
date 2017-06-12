#include "hsv.h"
#include <stdio.h>

void main(void)
{
    int i;
    for ( i=0;i<512;i++ )
    {
        unsigned char r, g, b;
        hsv2rgb(i, 255, 255, &r, &g, &b);
        printf("%d %d %d\n", (int)r, (int)g,(int)b);
    }
}
