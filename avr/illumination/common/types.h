#if !defined(__TYPES_H_DEFINED)
#define __TYPES_H_DEFINED

// status definitions
#define PRS_OK  0
#define PRS_READY   1
#define PRS_E_FAILED    -1
#define PRS_E_MEMORY    -2
#define PRS_E_UNKNOWN   -3
#define PRS_E_TIMEOUT   -4
#define PRS_E_CRC_CHECK_FAILED  -5
#define PRS_E_SCRUMBLED_DATA    -6
#define PRS_E_NOT_IMPLEMENTED   -7

typedef unsigned char byte_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed long int32_t;
typedef uint8_t size_t;
typedef int8_t status_t;
typedef uint16_t sample_t;

#endif // __TYPES_H_DEFINED
