#if !defined(__PROTOCOL_H_DEFINED)
#define __PROTOCOL_H_DEFINED

#include "types.h"

/**
 * protokolo esme:
 *
 * paketas yra 32bitu ilgio zodis
 * pirmi 8 bitai yra sleivo adresas
 * sekantys 3 bitai - komanda
 * like 21 bitas - komandos parametras
 *
 * jeigu sleivas yra RGB, tuomet kiekvienai spalvai
 * gaunasi 7 bitai. is  viso 2097152 spalvu.
 *
 * modifikacija - jeigu komandu nereikia, o tik nustatineti
 * sviestuko spalva, tuomet komandos 3 bitus galima priskirti
 * sleivo adresui. viso adresui turesime 11 bitu.
 *
 *
 **/


// command definitions
#define PRCMD_IDENTIFY
#define PRCMD_SET_RGB
#define PRCMD_SET_VALUE
#define PRCMD_AUTOADJUST_ON
#define PRCMD_AUTOADJUST_OFF
#define PRCMD_FADE_ON
#define PRCMD_FADE_OFF

#define PACKET_ENC_NUM_BYTES ( sizeof(packet_union)*2 )
#define PACKET_NUM_BYTES ( 2 )

#define PROTOCOL_BROADCAST  0xFF

typedef union packet_union
{
    byte_t bytes[5];
    int32_t l;
    struct
    {
        int32_t addr:8;
        int32_t crc:8;
        int32_t cmd:3;
        int32_t R:7;
        int32_t G:7;
        int32_t B:7;
    } snRGB;
    struct
    {
        int32_t addr:8;
        int32_t crc:8;
        int32_t cmd:3;
        int32_t VAL: 8;
        int32_t reserved:13;
    } snVAL;

} packet_t;


#endif //__PROTOCOL_H_DEFINED

