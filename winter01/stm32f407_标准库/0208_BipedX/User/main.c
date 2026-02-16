#include "stm32f4xx.h"
#include "main.h"
#include "Delay.h"

void LED_Init(void)
{
    //开启GPIOC的时钟（F4系列的时钟在AHB1总线上）
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	//配置引脚参数
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;           // 引脚 13
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;        // 输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;     // 高速
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;         // 上拉
    
    //初始化GPIOC
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

int main(void)
{
	SystemCoreClockUpdate(); 	//同步主频变量（只要用到Delay 就要加上这一行）
    LED_Init();

    while (1)
	{
        //假设低电平点亮：
		GPIO_ResetBits(GPIOC, GPIO_Pin_13); // 点亮
        Delay_ms(500);
        GPIO_SetBits(GPIOC, GPIO_Pin_13);   // 熄灭
        Delay_ms(500);
    }
}

void TimingDelay_Decrement(void)
{
		//空着，保证编译通过
}
