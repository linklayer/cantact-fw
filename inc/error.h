#ifndef INC_ERROR_H_
#define INC_ERROR_H_


typedef enum _error_t
{
	ERR_PERIPHINIT = 0,
	ERR_USBTX_BUSY,
	ERR_CAN_TXFAIL,
	ERR_CANRXFIFO_OVERFLOW,
	ERR_FULLBUF_CANTX,
	ERR_FULLBUF_USBRX,

	ERR_MAX
} error_t;


// Prototypes
void error_assert(error_t err);


#endif /* INC_ERROR_H_ */
