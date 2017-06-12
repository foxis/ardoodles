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

extern void send(const unsigned char *b, unsigned char N);
extern void recv(unsigned char *b, unsigned char N);

#define DIR_SEND 1
#define DIR_RECV 0

extern void wait_start(void);
extern char get_b(char trans);
extern void set_b(char);
extern void set_dir(char dir);

