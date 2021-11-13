#ifndef USART3_H
#define USART3_H

#include "hardware.h"
#include "sys.h"
#include "delay.h"
#include "laser.h"
		
extern u8 u3buf[512];					//����3���ջ���
extern u32 u3len;						//����3���ܻ��������ݸ���


void usart3_init(u32 bound);			//����3��ʼ�� bound: ������

void usart3_senddat(u8 *buf,u8 len);	//ʹ�ô���3����һ������
										//buf:�������׵�ַ
										//len:���͵��ֽ���

#endif	   



