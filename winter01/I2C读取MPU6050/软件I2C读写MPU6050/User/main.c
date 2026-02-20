#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "MPU6050.h"

int16_t AX,AY,AZ,GX,GY,GZ;

int main()
{
	OLED_Init();
	MPU6050_Init();
	
	while(1)
	{
		MPU6050_GetData(&AX,&AY,&AZ,&GX,&GY,&GZ);
		OLED_ShowSignedNum(2,1,AX,5);
		OLED_ShowSignedNum(3,1,AY,5);
		OLED_ShowSignedNum(4,1,AZ,5);
		OLED_ShowSignedNum(2,8,AX,5);
		OLED_ShowSignedNum(3,8,AY,5);
		OLED_ShowSignedNum(4,8,AZ,5);
	}
}
