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

unsigned short
me_encode(unsigned char c)
{
	unsigned short tmp=0;
	unsigned char i,t;
	for ( i=0;i<8;i++ )
	{
		t = c&1;

		tmp >>= 2;
		if ( t )
			tmp |= 0x8000;
		else
			tmp |= 0x4000;
		c >>= 1;
	}

  	return tmp;
}

unsigned char
me_decode16(unsigned short m)
{
	unsigned char i,c;

	c = 0;
	for ( i=0;i<8;i++ )
	{
		c<<=1;
		c |= (m&0xc000)==0x8000;
		m<<=2;
	}

	return c;
}

unsigned char
me_valid(unsigned short m)
{
	char i,t;
	register char a, b;
	for ( i=0,t=0;i<8;i++ )
	{
		a = !!(m&1);
		b = !!(m&2);
		t += (!(a||b)) || (a&&b) ;
		m>>=2;
	}
	return !t;
}

