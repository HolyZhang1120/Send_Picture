#ifndef __CRC32_H
#define __CRC32_H		
#include "sys.h"	 

u8 crc32_rev(uint8_t revBuffer[], uint16_t revBufferLeng);
void *crc32_send(uint8_t putBuffer[], uint16_t putBufferLeng);

#endif
