#include "stm32f10x.h"
#include "Delay.h"

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   //上拉输入(读取按键，按下则低电平点亮)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
}

uint8_t Key_GetNum(void)   //读取按键值，返回值类型为unsighed char
{
	uint8_t KeyNum = 0;   //按键键码默认为0（未按下时为0）
	
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 0)
	{
		Delay_ms(20);   //削弱按键抖动的影响
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1) == 0);
		Delay_ms(20);
		KeyNum = 1;
	}
	
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11) == 0)
	{
		Delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11) == 0);
		Delay_ms(20);
		KeyNum = 2;
	}
	
	return KeyNum;
}
