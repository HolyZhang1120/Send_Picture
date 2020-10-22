#ifndef _F_CRC
#define _F_CRC	161017	/* Revision ID */

#ifdef __cplusplus
extern "C" {
#endif

//�õ���ͷ�ļ�
#include <stdint.h>
//��������
/*
    ���ܣ�����CRC32ֵ
    ����1��uint8_t *pData��������CRC32������ָ��
    ����2��uint16_t Length��������CRC�����鳤��
    ����ֵ��uint32_t���������CRC32
*/
uint32_t CRC32Software(uint8_t *pData,uint32_t Length);
/*
    ���ܣ��ֶμ���CRC32ֵ
    ����1��uint8_t *pData��������CRC32������ָ��
    ����2��uint16_t Length��������CRC�����鳤��
    ����3��uint32_t mcrc���ϴε�CRC32ֵ
    ����ֵ��uint32_t���������CRC32
*/
uint32_t CRC32Software_(uint8_t *pData,uint32_t Length,uint32_t mcrc);
/*
    ���ܣ�����CRC32ֵ
    ����1��uint32_t add��������CRC32�ĵ�ַ�����ڼ����ڴ��CRC
    ����2��uint16_t Length��������CRC�����鳤��
    ����ֵ��uint32_t���������CRC32
*/
uint32_t CRC32SoftwarePointer(uint32_t add,uint32_t Length);

//��������
/****
    *   ����CRC16ֵ
    *   puchMsg:����������������
    *   usDataLen:������������������
    *   ����ֵ��ʮ��λ�޷���CRCֵ(С��ģʽ�����ֽ���ǰ)
    */
uint16_t Crc16Software(unsigned char *puchMsg,uint16_t usDataLen); // puchMsg message to calculate CRC upon

#ifdef __cplusplus
}
#endif

#endif /* F_CRC32 */

