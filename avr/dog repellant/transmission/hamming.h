/** 
 * Remote car alarm
 * 2004-2007 (c) Andrius Mikonis, vienas@delfi.lt
 *
 * You are free to use this code or parts of it in your applications
 * as long as you put a credit for me :)
 *
 * Also i cannot be held responsible for any harm caused 
 * by this or any derived code.
 * Use it at your own risk.
 *
 */

#if !defined(__HAMMING_H)
#define __HAMMING_H


unsigned char hamming_encode(unsigned char* out, const unsigned char* in, unsigned char cb);
unsigned char hamming_decode(unsigned char* out, const unsigned char* in, unsigned char cb);


#endif
