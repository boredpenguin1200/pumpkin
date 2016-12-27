/* Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/  ALL RIGHTS RESERVED  */

#ifndef CRC8_H_
#define CRC8_H_

#include <stdint.h>

/** Calculate CRC-8 from a buffer
Calculates CRC-8 from a buffer
@param dataPtr pointer to buffer
@param size size of buffer
@return CRC
*/
uint8_t calcCRC8(uint8_t * dataPtr, uint8_t size);

#endif /* CRC8_H_ */

/** @} */
