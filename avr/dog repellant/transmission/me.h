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

#ifndef __ME_H__
#define __ME_H__

extern unsigned char  me_valid(unsigned short m);
extern unsigned short me_encode(unsigned char c);
extern unsigned char  me_decode16(unsigned short m);

#endif /* __ME_H__ */

