#include "intconfig.h"


NVIC_InitTypeDef USART1_NVIC_CONFIG =			
{									//����1�����ж����ȼ���ʼ���ṹ��
	USART1_IRQn,					
	3,							
	3,								
	ENABLE							//ʹ��
};

NVIC_InitTypeDef USART2_NVIC_CONFIG =			
{									//����2�����ж����ȼ���ʼ���ṹ��
	USART2_IRQn,					
	2,								
	2,								
	ENABLE							
};

NVIC_InitTypeDef USART3_NVIC_CONFIG =			
{									//����3�����ж����ȼ���ʼ���ṹ��
	USART3_IRQn,					
	2,								
	2,								
	ENABLE							
};

NVIC_InitTypeDef TIM2_NVIC_CONFIG =
{									//TIM2�ж����ȼ���ʼ���ṹ��
	TIM2_IRQn,			
	1,
	1,		
	ENABLE
};


NVIC_InitTypeDef TIM3_NVIC_CONFIG =
{									//TIM3�ж����ȼ���ʼ���ṹ��
	TIM3_IRQn,			
	0,
	0,		
	ENABLE
};

NVIC_InitTypeDef TIM4_NVIC_CONFIG =
{									//TIM4�ж����ȼ���ʼ���ṹ��
	TIM4_IRQn,			
	0,
	0,		
	ENABLE
};

NVIC_InitTypeDef DMA1_Channel7_NVIC_CONFIG = 			
{									//DMA1ȫ���жϳ�ʼ���ṹ��
	DMA1_Channel7_IRQn,
	2,
	2,
	ENABLE	
};

NVIC_InitTypeDef EXIT1_NVIC_CONFIG = 			
{									//�ⲿ�ж�1ȫ���жϳ�ʼ���ṹ��
	EXTI1_IRQn,
	3,
	3,
	ENABLE	
};













