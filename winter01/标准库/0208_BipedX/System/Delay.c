#include "stm32f4xx.h"

/**
  * @brief  微秒级延时
  * @param  xus 延时时长，范围：0~233015
  * @retval 无
  */
void Delay_us(uint32_t xus)
{
    // SystemCoreClock 是标准库定义的全局变量，代表当前主频（单位Hz）
    // SystemCoreClock / 1000000 就能得到 1us 对应的周期数
    SysTick->LOAD = (SystemCoreClock / 1000000) * xus; 
    SysTick->VAL = 0x00;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk; 
    
    while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    
    SysTick->CTRL = 0; // 关闭定时器
}

/**
  * @brief  毫秒级延时
  * @param  xms 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_ms(uint32_t xms)
{
	while(xms--)
	{
		Delay_us(1000);
	}
}
 
/**
  * @brief  秒级延时
  * @param  xs 延时时长，范围：0~4294967295
  * @retval 无
  */
void Delay_s(uint32_t xs)
{
	while(xs--)
	{
		Delay_ms(1000);
	}
} 
