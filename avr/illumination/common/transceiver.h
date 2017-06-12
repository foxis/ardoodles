#if !defined(__TRANSCEIVER_H_DEFINED)
#define __TRANSCEIVER_H_DEFINED

#include "types.h"

typedef status_t (*wait_for_line_clear_t)(void)
typedef byte_t (*poll_one_bit_t)(void)
typedef sample_t (*poll_one_sample_t)(void)
typedef byte_t (*poll_one_byte_t)(void)
typedef status_t (*put_one_bit_t)(byte_t)
typedef status_t (*put_one_sample_t)(sample_t)
typedef status_t (*put_one_byte_t)(byte_t)

// kbps
void set_bitrate(int32_t);
// kHz
void set_modulation_frequency(int32_t);
// bitfield
void set_modulation(int32_t);

// These functions are intended for POLL type of receivers
//
//

// receive data using appropriate receiveing callback
// fills buffer up to maxsize and returns the size of received data in recved
// function returns PRS_OK on success, otherwise any negative error code
status_t receive_bits(poll_one_bit_t receiver, byte_t* buffer, size_t maxsize,size_t* recved);
status_t receive_samples(poll_one_sample_t receiver, byte_t* buffer, size_t maxsize,size_t* recved);
status_t receive_bytes(poll_one_byte_t receiver, byte_t* buffer, size_t maxsize,size_t* recved);

// sends data using appropriate sending callback
// waits for free line using waiter callback
// returns PRS_OK on success, otherwise any negative error code
// NOTE: sending callback should be as fast as possible and should 
// always complete in the same ammount of time
status_t send_bits(put_one_bit_t sender, wait_for_line_clear_t waiter, byte_t* buffer, size_t size);
status_t send_samples(put_one_sample_t sender,wait_for_line_clear_t waiter, byte_t* buffer, size_t size);
status_t send_bytes(put_one_byte_t sender,wait_for_line_clear_t waiter, byte_t* buffer, size_t size);


// These functions are intended for interrupt driven receivers
//
//

// resets any current data
// and sets specified buffer for receiver
status_t set_buffer(byte_t* buffer, size_t size);

// receives one bit from a stream.
// returns PRS_OK if succeded
// otherwise returns either an error(negative)
// or PRS_READY if packet is fully received
status_t one_bit_received(byte_t);
// the same as one_bit_received, but for whole byte
status_t one_byte_received(byte_t);

// the same as one_bit_received, but for bit sample
status_t one_sample_received(sample_t);



#endif // __TRANSCEIVER_H_DEFINED
