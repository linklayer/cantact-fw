//
// Error: handling / reporting of system errors
//


#include "stm32f0xx_hal.h"
#include "error.h"


// Private variables
static uint32_t err_reg = 0;
static uint32_t err_time[ERR_MAX] = {0};


// Assert an error: sets err register bit and records timestamp
void error_assert(error_t err)
{
	if(err >= ERR_MAX)
		return;

	err_time[err] = HAL_GetTick();
	err_reg |= (1 << err);
}


// Get the systick at which an error last occurred, or 0 otherwise
uint32_t error_timestamp(error_t err)
{
	if(err >= ERR_MAX)
		return 0;

	return err_time[err];
}

// Returns 1 if the error has occurred since boot
uint8_t error_occurred(error_t err)
{
	if(err >= ERR_MAX)
		return 0;

	return (err_reg & (1 << err)) > 0;
}

// Return value of error register
uint32_t error_reg(void)
{
	return err_reg;
}
