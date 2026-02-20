#include "stm32f10x.h"
#include "MPU6050_Reg.h"

#define MPU6050_ADDRESS		0xD0

void MPU6050_WaitEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT)   //直接将原来的CheckEvent封装起来更好
{
	uint32_t Timeout;
	Timeout = 10000;
	while(I2C_CheckEvent(I2Cx,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS)
	{
		Timeout --;
		if(Timeout == 0)
		{
			break;
		}
	}
}


// 写寄存器
void MPU6050_WriteReg(uint8_t RegAddress,uint8_t Data)
{
	I2C_GenerateSTART(I2C2,ENABLE);		// 起始信号
	
//	超时退出机制，防止死等
//	uint32_t Timeout;
//	Timeout = 10000;
//	while(I2C_CheckEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT) != SUCCESS)
//	{
//		Timeout --;
//		if(Timeout == 0)
//		{
//			break;   //也可以用return跳出整个WritrReg函数
//		}
//	}
	
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);	// 起始信号发出去了吗?
	
	I2C_Send7bitAddress(I2C2,MPU6050_ADDRESS,I2C_Direction_Transmitter);	// 写从机地址
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);	 // 从机地址发完且对方回ACK了吗?
	
	I2C_SendData(I2C2,RegAddress);		// 写寄存器地址
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTING);	  // 寄存器地址发出去了吗？
	
	I2C_SendData(I2C2,Data);		// 写数据
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED);	// 这8位数据确实传完了吗?
	
	I2C_GenerateSTOP(I2C2,ENABLE);		// 停止信号
}


// 读寄存器
uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	
	I2C_GenerateSTART(I2C2,ENABLE);		// 起始信号
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C2,MPU6050_ADDRESS,I2C_Direction_Transmitter);	// 写从机地址
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
	
	I2C_SendData(I2C2,RegAddress);		// 写寄存器地址
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	
	I2C_GenerateSTART(I2C2,ENABLE);		// 重复起始信号
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_MODE_SELECT);
	
	I2C_Send7bitAddress(I2C2,MPU6050_ADDRESS,I2C_Direction_Receiver);	// 读数据(方向改为Receiver)
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
	
	I2C_AcknowledgeConfig(I2C2,DISABLE);	// 关闭应答
				// 当硬件接收完这个字节后，会自动向从机发送 NACK
	I2C_GenerateSTOP(I2C2,ENABLE);	  // 停止信号
				// 硬件在发送完 NACK 后，紧接着会自动发送停止位
	MPU6050_WaitEvent(I2C2,I2C_EVENT_MASTER_BYTE_RECEIVED);	  // 等待事件
				// 此时硬件已经把从机(MPU6050)发来的8位数据收完了，并搬到了DR寄存器里
	Data = I2C_ReceiveData(I2C2);	// 读取数据
				// 从数据寄存器(DR)读出数据
	
	I2C_AcknowledgeConfig(I2C2,ENABLE);		// 开启应答
	
	return Data;
}


void MPU6050_Init(void)
{
//	MyI2C_Init();
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	
	I2C_InitTypeDef I2C_Initstructure;
	I2C_Initstructure.I2C_Mode = I2C_Mode_I2C;
	I2C_Initstructure.I2C_ClockSpeed = 50000;
	I2C_Initstructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_Initstructure.I2C_Ack = I2C_Ack_Enable;
	I2C_Initstructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_Initstructure.I2C_OwnAddress1 = 0x00;
	I2C_Init(I2C2,&I2C_Initstructure);
	
	I2C_Cmd(I2C2,ENABLE);
	
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1,0x01);
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2,0x00);
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV,0x09);
	MPU6050_WriteReg(MPU6050_CONFIG,0x06);
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG,0x18);
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG,0x18);
}


void MPU6050_GetData(int16_t *AccX,int16_t *AccY,int16_t *AccZ,int16_t *GyroX,int16_t *GyroY,int16_t *GyroZ)
{
	uint8_t DataH,DataL;
	
	DataH =MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
	DataL =MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
	*AccX = (int16_t)((DataH << 8) | DataL);
	
	DataH =MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
	DataL =MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
	*AccY = (int16_t)((DataH << 8) | DataL);
	
	DataH =MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
	DataL =MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
	*AccZ = (int16_t)((DataH << 8) | DataL);
	
	DataH =MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
	DataL =MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
	*GyroX = (int16_t)((DataH << 8) | DataL);
	
	DataH =MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
	DataL =MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
	*GyroY = (int16_t)((DataH << 8) | DataL);
	
	DataH =MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
	DataL =MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
	*GyroZ = (int16_t)((DataH << 8) | DataL);
}
/*
在处理像*AccX这种16位有符号数时，加上(int16_t)强制转换是一个好习惯
这能确保当最高位为1时，单片机能正确识别它是一个负数
*/
