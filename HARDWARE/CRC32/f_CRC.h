#ifndef _F_CRC
#define _F_CRC	161017	/* Revision ID */

#ifdef __cplusplus
extern "C" {
#endif

//用到的头文件
#include <stdint.h>
//函数申明
/*
    功能：计算CRC32值
    参数1：uint8_t *pData，待计算CRC32的数组指针
    参数2：uint16_t Length，带计算CRC的数组长度
    返回值：uint32_t，计算出的CRC32
*/
uint32_t CRC32Software(uint8_t *pData,uint32_t Length);
/*
    功能：分段计算CRC32值
    参数1：uint8_t *pData，待计算CRC32的数组指针
    参数2：uint16_t Length，带计算CRC的数组长度
    参数3：uint32_t mcrc，上次的CRC32值
    返回值：uint32_t，计算出的CRC32
*/
uint32_t CRC32Software_(uint8_t *pData,uint32_t Length,uint32_t mcrc);
/*
    功能：计算CRC32值
    参数1：uint32_t add，待计算CRC32的地址，用于计算内存的CRC
    参数2：uint16_t Length，带计算CRC的数组长度
    返回值：uint32_t，计算出的CRC32
*/
uint32_t CRC32SoftwarePointer(uint32_t add,uint32_t Length);

//函数申明
/****
    *   计算CRC16值
    *   puchMsg:待计算的数组变量、
    *   usDataLen:待计算的数组变量长度
    *   返回值：十六位无符号CRC值(小端模式，低字节在前)
    */
uint16_t Crc16Software(unsigned char *puchMsg,uint16_t usDataLen); // puchMsg message to calculate CRC upon

#ifdef __cplusplus
}
#endif

#endif /* F_CRC32 */

