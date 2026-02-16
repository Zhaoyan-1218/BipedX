#include "stm32f10x.h"
#include "Serial.h"
#include "CAN_Motor.h"

int main(void)
{
    Serial_Init();
    CAN_Motor_Init();
    
    Serial_Printf("System Ready!\r\n");

    while(1)
	{
		if (Serial_GetRxFlag())
		{
			uint8_t cmd = Serial_GetRxData();
        
			switch (cmd) 
			{
				case '1': //第一组启动（电流给1000）
					Serial_Printf("Group1 Start!\r\n");
					Set_Motor_Group_Current(1, 1000, 0, 0, 0);
					break;
                
				case '2': //第二组启动（电流给1000）
					Serial_Printf("Group2 Start!\r\n");
					Set_Motor_Group_Current(2, 1000, 0, 0, 0);
					break;
                
				case '0': //全部停止
					Serial_Printf("All Motors Stop!\r\n");
					Set_Motor_Group_Current(1, 0, 0, 0, 0);
					Set_Motor_Group_Current(2, 0, 0, 0, 0);
					break;
                
				default:
					Serial_Printf("Unknown Command: %c\r\n", cmd);
					break;
			}
		}
	}
}
