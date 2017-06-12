#include "transceiver.h"

status_t receive_bits(poll_one_bit_t receiver, byte_t* buffer, size_t maxsize,size_t* recved)
{
    return PRS_OK;
}
status_t receive_samples(poll_one_sample_t receiver, byte_t* buffer, size_t maxsize,size_t* recved)
{
    return PRS_OK;
}
status_t receive_bytes(poll_one_byte_t receiver, byte_t* buffer, size_t maxsize,size_t* recved)
{
    return PRS_OK;
}

// sends data using appropriate sending callback
// waits for free line using waiter callback
// returns PRS_OK on success, otherwise any negative error code
// NOTE: sending callback should be as fast as possible and should 
// always complete in the same ammount of time
status_t send_bits(put_one_bit_t sender, wait_for_line_clear_t waiter, byte_t* buffer, size_t size)
{
    return PRS_OK;
}
status_t send_samples(put_one_sample_t sender,wait_for_line_clear_t waiter, byte_t* buffer, size_t size)
{
    return PRS_OK;
}
status_t send_bytes(put_one_byte_t sender,wait_for_line_clear_t waiter, byte_t* buffer, size_t size)
{
    return PRS_OK;
}


// These functions are intended for interrupt driven receivers
//
//

// resets any current data
// and sets specified buffer for receiver
status_t set_buffer(byte_t* buffer, size_t size)
{
    return PRS_OK;
}

// receives one bit from a stream.
// returns PRS_OK if succeded
// otherwise returns either an error(negative)
// or PRS_READY if packet is fully received
status_t one_bit_received(byte_t)
{
    return PRS_OK;
}
// the same as one_bit_received, but for whole byte
status_t one_byte_received(byte_t)
{
    return PRS_OK;
}

// the same as one_bit_received, but for bit sample
status_t one_sample_received(sample_t)
{
    return PRS_OK;
}



#endif // __TRANSCEIVER_H_DEFINED

