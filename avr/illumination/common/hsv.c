#include "hsv.h"

void hsv2rgb(unsigned short h, unsigned char _s, unsigned char _v, unsigned char* r, unsigned char* g, unsigned char* b)
{
  if ( _s == 0 )
  {
     *r = _v;
     *g = _v;
     *b = _v;
  }
  else
  {
     register unsigned short s=_s, v=_v;
     register unsigned short var_h = ((h * 6)&0x1FF)>>1;
     unsigned char var_i = (h * 6)>>9;
     unsigned char var_1 = (v * ( 255 - s ))>>8;
     unsigned char var_2 = (v * ( 255 - ((s * var_h)>>8) ))>>8;
     unsigned char var_3 = (v * ( 255 - ((s * ( 255 - var_h ))>>8) ) )>>8;
 
     switch ( var_i )
     {
     case 0:
     case 6: { *r = _v    ; *g = var_3 ; *b = var_1 ; break ; }
     case 1: { *r = var_2 ; *g = _v    ; *b = var_1 ; break ; }
     case 2: { *r = var_1 ; *g = _v    ; *b = var_3 ; break ; }
     case 3: { *r = var_1 ; *g = var_2 ; *b = _v    ; break ; }
     case 4: { *r = var_3 ; *g = var_1 ; *b = _v    ; break ; }
     case 5: { *r = _v    ; *g = var_1 ; *b = var_2 ; break ; }
     }
  }
}

