#include "crc32.h"
#include "f_CRC.h"
	
//接收数据CRC32

u8 crc32_rev(uint8_t revBuffer[], uint16_t revBufferLeng)
{
	u32 putBuffer1,putBuffer2;
	putBuffer1 = CRC32Software(revBuffer,revBufferLeng-4);
	putBuffer2 = revBuffer[revBufferLeng-4]*0x1000000+revBuffer[revBufferLeng-3]*0x10000+revBuffer[revBufferLeng-2]*0x100+revBuffer[revBufferLeng-1];
	if(putBuffer1 == putBuffer2)
		return 1;
	else return 0;
}

/*发送数据CRC32   
引用：
	u8 *size_t;
  size_t = crc32_send((u8 *)DataBuffer,BUFFER_SIZE); */

void *crc32_send(uint8_t putBuffer[], uint16_t putBufferLeng)
{
	u32 putBuffer1;
	u8 i;
	putBuffer1 = CRC32Software((uint8_t *)putBuffer,putBufferLeng);
	for(i=0;i<4;i++)
	{
		putBuffer[putBufferLeng+3-i] = putBuffer1>>(8*i);
	}
	return putBuffer;
}






