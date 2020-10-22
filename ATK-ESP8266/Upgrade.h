#ifndef __UPGRADE_H
#define __UPGRADE_H

#include "sys.h"

#define BATA_SIZE    5120		//下行包有效数据大小
#define SEND_BATA_SIZE    2048		//上行包有效数据大小
#define Flash_ADDR   0x1000		//存入flash起始位置

extern u32 Crc_file;
u8 connect_server(void);
u8 send_server(u8 *buffer,u32 len);
u32 Get_Upgrade_size(void);
void *Get_Upgrade_package(u8 package,u16 size_num);
void Upgrade(void);
void SoftReset(void);
u32 rev_picture(u8 temp,u8 *buf,u32 send_size,u8 pack);
#endif


