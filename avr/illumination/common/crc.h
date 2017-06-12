/*
  Name  : CRC-8
  Poly  : 0x31     x^8 + x^5 + x^4 + 1
  Init  : 0xFF
  Revert: false
  XorOut: 0x00
  Check : 0xF7 ("123456789")
  MaxLen: 15 bytes (127 bits)
*/
unsigned char crc8(unsigned char *pcBlock, unsigned char len)

/*
  Name  : CRC-8
  Poly  : 0x31     x^8 + x^5 + x^4 + 1
  Init  : 0xFF
  Revert: false
  XorOut: 0x00
  Check : 0xF7 ("123456789")
  MaxLen: 15 bytes (127 bits)
*/
unsigned char crc8_fast(unsigned char *pcBlock, unsigned char len)

/*
  Name  : CRC-16 CCITT
  Poly  : 0x1021   x^16 + x^12 + x^5 + 1
  Init  : 0xFFFF
  Revert: false
  XorOut: 0x0000
  Check : 0x29B1 ("123456789")
  MaxLen: 4095 bytes (32767 bits)
*/
unsigned short crc16(unsigned char *pcBlock, unsigned short len)

/*
  Name  : CRC-16 CCITT
  Poly  : 0x1021   x^16 + x^12 + x^5 + 1
  Init  : 0xFFFF
  Revert: false
  XorOut: 0x0000
  Check : 0x29B1 ("123456789")
  MaxLen: 4095 bytes (32767 bits)
*/
 
unsigned short crc16_fast(unsigned char * pcBlock, unsigned short len)


/*
  Name  : CRC-32
  Poly  : 0x04C11DB7       x^32 + x^26 + x^23 + x^22 + x^16 + x^12 + x^11 
                       + x^10 + x^8 + x^7 + x^5 + x^4 + x^2 + x + 1
  Init  : 0xFFFFFFFF
  Revert: true
  XorOut: 0xFFFFFFFF
  Check : 0xCBF43926 ("123456789")
  MaxLen: 268 435 455 bytes (2 147 483 647 bits)
*/
unsigned long crc32(unsigned char *buf, unsigned long len)
