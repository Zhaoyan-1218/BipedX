#ifndef __CAN_MOTOR_H
#define __CAN_MOTOR_H

#include "stm32f10x.h"

//电机ID分组：一组控制1-4号电调，二组控制5-8号电调
#define CAN_CONTROL_ID_GROUP1	0x200
#define CAN_CONTROL_ID_GROUP2	0x1FF

void CAN_Motor_Init(void);
void Set_Motor_Group_Current(uint8_t group, int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4);

#endif


/*

1、控制量不是“转速”而是“电流”：
	大疆的电调（C610/C620）工作在力矩电流环。代码里发的1000并不是1000rpm，而是1000个单位的电流。
	要实现定速旋转，必须在单片机里写一个PID算法，根据反馈的转速来调整这个电流值。

2、ID映射规则：
电调上的拨码开关决定了它的ID（1到8）。
	CAN ID 0x200：控制1, 2, 3, 4号电机。
	CAN ID 0x1FF：控制5, 6, 7, 8号电机。

3、反馈帧（接收部分）：
电机会以0x201~0x208的ID每秒发送上千次反馈包。
	数据里包含：转子机械角度（0~8191）、转速（rpm）、实际转矩电流。
	难点：转子角度是14位的，通常需要在单片机里处理“过零处理”（即角度从8191跳变回0的瞬间）。

*/
