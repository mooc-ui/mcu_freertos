/******************************************************************************/
/** \file key.h
 **
 ** Device define
 **
 **   - 2020-07-23
 **
 *****************************************************************************/

#ifndef  __CRC_H__
#define  __CRC_H__

#include "target.h"
#include <stdint.h>
#include "lib_adc.h"

uint32_t crc32(uint32_t crc, const uint8_t *buf, uint32_t size);

#endif /* __CRC_H__ */

/*******************************************************************************
 * EOF (not truncated)                                                        
 ******************************************************************************/
