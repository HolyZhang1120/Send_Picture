#include "Upgrade.h"
#include "wifista.h"
#include "usart.h"
#include "usart3.h"
#include "delay.h"
#include "malloc.h"
#include "crc32.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "w25qxx.h" 
#include "f_CRC.h"
#include <stdbool.h>

//远程升级端口	
#define Upgrade_PORTNUM 	"40204"
////远程升级服务器IP
#define Upgrade_SERVERIP 	"cn-zj-dx.sakurafrp.com"

//远程升级端口	
//#define Upgrade_PORTNUM 	"25052"
//远程升级服务器IP
//#define Upgrade_SERVERIP 	"192.168.2.105"

u32 Crc_file;
bool Upgrade_Flag = false;

//连接服务器
u8 connect_server(void)
{
	u8 *p;
	u8 res;
//	u8 ipbuf[16]; 	//IP缓存
	p=mymalloc(SRAMIN,40);							//申请40字节内存
	sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",Upgrade_SERVERIP,Upgrade_PORTNUM);    //配置目标TCP服务器
	res = atk_8266_send_cmd(p,"OK",500);//连接到目标TCP服务器	
	if(res==1)
	{
		printf("disconnect server \r\n");
		myfree(SRAMIN,p);
		return 1;
	}
	else 
	{
		atk_8266_send_cmd("AT+CIPMODE=1","OK",100);      //传输模式为：透传	
		atk_8266_send_cmd("AT+CIPSEND","OK",300);
		printf("connect server \r\n");
		myfree(SRAMIN,p);
		return 0;
	}
}

u8 send_server(u8 *buffer,u32 len)
{
	u16 i=0,j=0,k;
//	if(atk_8266_consta_check()==0)
//		printf("start \r\n");
	
	atk_8266_send_cmd("AT+CIPMODE=1","OK",100);      //传输模式为：透传	
//	atk_8266_get_wanip(ipbuf);//获取WAN IP
	
	USART3_RX_STA=0;
	atk_8266_send_cmd("AT+CIPSEND","OK",300);	
	delay_ms(10);
	printf("start trans...\r\n");
	USART3_RX_STA=0;	//清零串口3数据
	Usart_SendString(USART3,(u8 *)buffer,len);
//	delay_ms(10);//延时20ms返回的是指令发送成功的状态
//	atk_8266_at_response(1);
	delay_ms(1000);
//	printf("1\r\n");
//	atk_8266_at_response(0);
	
	do
	{
		i++;
		if(i==600)
				j++;
		if(j==800)
			return 1;	
	}while(!(USART3_RX_STA&0X8000));		//等待时间到达  
	USART3_RX_STA=0;	//清零串口3数据
	printf("it is OK\r\n");
//	IWDG_Feed();//喂狗
//	printf("p1：%s\r\n",RX_buf);
	return 0;
}


//请求升级
u32 Get_Upgrade_size(void)
{
	u32 size;
//	u8 *size_t;
	u8 buffer_send[3]={0xAA,0xAB};
	buffer_send[2]=BATA_SIZE/1024;
	while(send_server(crc32_send((u8 *)buffer_send,3),7))
		return NULL;
	if((USART3_RX_BUF1[0]==0xAA)&&(USART3_RX_BUF1[1]==0xAB)&&(crc32_rev((u8 *)USART3_RX_BUF1, 14)))
	{
		Crc_file = USART3_RX_BUF1[6]*0x1000000+USART3_RX_BUF1[7]*0x10000+USART3_RX_BUF1[8]*0x100+USART3_RX_BUF1[9];
		size = USART3_RX_BUF1[2]*0x1000000+USART3_RX_BUF1[3]*0x10000+USART3_RX_BUF1[4]*0x100+USART3_RX_BUF1[5];
//				sscanf((char *)size, "%d", &size);
		Upgrade_Flag = true;
		return size;			
	}
	else return NULL;
}

//请求包数
void *Get_Upgrade_package(u8 package,u16 size_num)
{
	u32 size;
//	u8 *size_t;
	u8 buffer_send[3]={0xAA,0xAC};
	buffer_send[2] = package;
//	crc32_ret=crc32_send((u8 *)buffer_send,3);
	delay_ms(50);
	while(send_server((u8 *)crc32_send((u8 *)buffer_send,3),7))
		return NULL;
	printf("pack：%d\r\n",USART3_RX_BUF1[2]);
//	if(USART3_RX_BUF1[2]==0x10)
//	{for(kl=0;kl<size_num;kl++)
//		printf("USART3_RX_BUF1[%d]：%x\r\n",kl,USART3_RX_BUF1[kl]);
//	}
	if((USART3_RX_BUF1[0]==0xAA)&&(USART3_RX_BUF1[1]==0xAC)&&(crc32_rev((u8 *)USART3_RX_BUF1, size_num)))
	{
		size = USART3_RX_BUF1[3]*0x1000000+USART3_RX_BUF1[4]*0x10000+USART3_RX_BUF1[5]*0x100+USART3_RX_BUF1[6];
		sscanf((char *)size, "%d", &size);
		if(size == (size_num - 11))
			return USART3_RX_BUF1;
		else return NULL; 
	}
	else return NULL;
}

void Upgrade(void)
{
	u32 crc32_file,Size;
	u16 size_num,j;	
	u8 pack,i,k=0,*write_buf1,*write_buf,write_buf2[4] = {0};	
  bool Upgrade_Flag_net = true;
	
	if(Upgrade_Flag_net == true)
	{	
		Size = Get_Upgrade_size();
		printf("Crc_file为0x:%x\n", Crc_file);
		printf("Size为0x:%d\n", Size);
		atk_8266_quit_trans();//退出透传
	//		atk_8266_send_cmd("AT+CIPCLOSE","OK",50);         //关闭连接
		while(Size == NULL)
		{
			atk_8266_quit_trans();//退出透传
			atk_8266_send_cmd("AT+CIPCLOSE","OK",50);         //关闭连接
			delay_ms(1000);
			connect_server();
			Size = Get_Upgrade_size();
			k++;
			if(k==3)
			{
				k = 0;
				Upgrade_Flag = false;
				printf("Get Size err\r\n");
				break;
			}
		}
		printf("Size：%d\r\n",Size);
	}
	if(Upgrade_Flag==true)
	{
		write_buf = mymalloc(SRAMIN,BATA_SIZE+50);
		write_buf1 = mymalloc(SRAMIN,BATA_SIZE+50);
//		write_buf2 = mymalloc(SRAMIN,20);
	
		if(Size%BATA_SIZE != 0) 
			pack = Size/BATA_SIZE + 1;
		else pack = Size/BATA_SIZE;
			printf("pack：%d\r\n",pack);
		for(i=1;i<(pack+1);i++)
		{
//				IWDG_Feed();//喂狗
			if(i == pack)
				size_num = Size - (BATA_SIZE*(i-1)) + 11;
			else size_num = BATA_SIZE+11;
			printf("size_num：%d\r\n",size_num);
			write_buf1 = Get_Upgrade_package(i,size_num);
			atk_8266_quit_trans();//退出透传
//				atk_8266_send_cmd("AT+CIPCLOSE","OK",50);         //关闭连接
			while(write_buf1 == NULL)
			{
				atk_8266_quit_trans();//退出透传
				atk_8266_send_cmd("AT+CIPCLOSE","OK",50);         //关闭连接
				delay_ms(1000);
				printf("NULL\r\n");
				connect_server();
				write_buf1 = Get_Upgrade_package(i,size_num);
				k++;
				if(k==10)
				{
					k = 0;
					printf("Get package err\r\n");
					break;
				}
			}
			if(write_buf1 == NULL)
			{
				printf("err\r\n");
				break;
			}
			else
			{
				for(j=0;j<(size_num-11);j++)
				{
					write_buf[j] = write_buf1[j+7];
				}
				delay_ms(50);
				W25QXX_Write((u8*)write_buf,(Flash_ADDR+4+BATA_SIZE*(i-1)),(size_num-11));						
				if(i==1)
					crc32_file = CRC32Software((u8 *)write_buf,BATA_SIZE);
				else crc32_file = CRC32Software_((u8 *)write_buf,(size_num-11),(uint32_t)crc32_file);
			}				
			delay_ms(100);	
			printf("crc32_file为0x:%x\n", crc32_file);	 
			printf("11\n");
		}
		printf("22\n");
		
		myfree(SRAMIN,write_buf);
		myfree(SRAMIN,write_buf1);
		
		if((crc32_file == Crc_file)&&(Size != NULL))//判断文件CRC32
		{	
			printf("固件正确\r\n");
			Crc_file = 0;
			Upgrade_Flag = false;
//			Upgrade_Flag_net = false;
			write_buf2[0] = 0xF6;
			write_buf2[1] = Size >> 16;
			write_buf2[2] = Size >> 8;
			write_buf2[3] = Size;
			W25QXX_Write((u8*)write_buf2,Flash_ADDR,4);
			SoftReset();   //复位
		}
	}
}

void SoftReset(void)
{
    __set_FAULTMASK(1); // 关闭所有中断
    NVIC_SystemReset(); // 复位
}


u32 rev_picture(u8 temp,u8 *buf,u32 send_size,u8 pack)
{
	u8 pack_ack;
	u16 i,kk;
	u32 size;
	u8 buffer_send[SEND_BATA_SIZE+30]={0xBB};

//	buf = mymalloc(SRAMIN,BATA_SIZE+50);
//	buffer_send = mymalloc(SRAMIN,BATA_SIZE+50);
//	buffer_send[0]=0xBB;
	if(temp==0)  //发送大小
	{
		buffer_send[1] = 0xAB;
		for(i=0;i<4;i++)
			buffer_send[5-i]=send_size>>(8*i);
		while(send_server(crc32_send((u8 *)buffer_send,6),10))
		{
//			myfree(SRAMIN,buf);
//			myfree(SRAMIN,buffer_send);
			return NULL;
		}
		if((USART3_RX_BUF1[0]==0xBB)&&(USART3_RX_BUF1[1]==0xAB)&&(crc32_rev((u8 *)USART3_RX_BUF1, 10)))
		{
			printf("12\n");
			size = USART3_RX_BUF1[2]*0x1000000+USART3_RX_BUF1[3]*0x10000+USART3_RX_BUF1[4]*0x100+USART3_RX_BUF1[5];
			sscanf((char *)size, "%d", &size);
//			myfree(SRAMIN,buf);
//			myfree(SRAMIN,buffer_send);
			return size;
		}
		else 
		{
//			myfree(SRAMIN,buf);
//			myfree(SRAMIN,buffer_send);
			return NULL;
		}
	}
	if(temp==1)  
	{
		buffer_send[1] = 0xAC;
		buffer_send[2] = pack;
		for(i=0;i<4;i++)
			buffer_send[6-i]=send_size>>(8*i);
		for(i=0;i<send_size;i++)
			buffer_send[i+7]=buf[i];
		while(send_server(crc32_send((u8 *)buffer_send,(send_size+7)),(send_size+11)))
		{
//			myfree(SRAMIN,buf);
//			myfree(SRAMIN,buffer_send);
			return NULL;
		}
		if((USART3_RX_BUF1[0]==0xBB)&&(USART3_RX_BUF1[1]==0xAC)&&(crc32_rev((u8 *)USART3_RX_BUF1, 7)))
		{
			printf("123\n");
			pack_ack = USART3_RX_BUF1[2];
			//sscanf((char *)pack_ack, "%d", &pack_ack);
			if (pack == pack_ack)
			{
//				myfree(SRAMIN,buf);
//				myfree(SRAMIN,buffer_send);
				return 1;
			}
			else 
			{
//				myfree(SRAMIN,buf);
//				myfree(SRAMIN,buffer_send);
				return NULL;	
			}				
		}
		else 
		{
//			myfree(SRAMIN,buf);
//			myfree(SRAMIN,buffer_send);
			return NULL;	
		}
	}
	if(temp==2)  
	{
		buffer_send[1] = 0xAD;
		for(i=0;i<4;i++)
			buffer_send[5-i]=send_size>>(8*i);
		printf("crc_pic:%x\n", send_size);
		while(send_server(crc32_send((u8 *)buffer_send,6),10))
		{
			printf("112233\n");
//			myfree(SRAMIN,buf);
//			myfree(SRAMIN,buffer_send);
			return NULL;
		}
		if((USART3_RX_BUF1[0]==0xBB)&&(USART3_RX_BUF1[1]==0xAD)&&(USART3_RX_BUF1[2]==0xAA)&&(crc32_rev((u8 *)USART3_RX_BUF1,7)))
		{
			printf("1234\n");
//			myfree(SRAMIN,buf);
//			myfree(SRAMIN,buffer_send);
			return 1;
		}
		else 
		{
//			myfree(SRAMIN,buf);
//			myfree(SRAMIN,buffer_send);
			return NULL;
		}
	}
	else 
	{
//		myfree(SRAMIN,buf);
//		myfree(SRAMIN,buffer_send);
		return NULL;
	}
}




