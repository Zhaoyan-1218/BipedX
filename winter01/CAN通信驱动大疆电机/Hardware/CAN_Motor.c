#include "CAN_Motor.h"
#include "Serial.h"

//CAN1初始化
void CAN_Motor_Init(void)
{
    //开启时钟(CAN1在APB1总线)
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
    
    //GPIO初始化: PA11(Rx)，PA12(Tx)
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		//复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		//上拉输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //CAN初始化(针对3508电机，CAN波特率通常设为1Mbps，逻辑电平抗干扰能力强)
    CAN_InitTypeDef CAN_InitStructure;
    CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = ENABLE;	//自动离线管理：若总线拥堵则自动恢复，防止失控
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = DISABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    
    //配置为1Mbps(以F103的36MHz APB1时钟为例)
	// 1Mbps = 36MHz / Prescaler / (SJW + BS1 + BS2)
    CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;
    CAN_InitStructure.CAN_BS1 = CAN_BS1_3tq;
    CAN_InitStructure.CAN_BS2 = CAN_BS2_5tq;
    CAN_InitStructure.CAN_Prescaler = 4; 
    CAN_Init(CAN1, &CAN_InitStructure);

    //过滤器配置(必须配置，否则收不到电机回传的数据)
	//此处配置为全通模式（Mask=0），确保能接收所有ID的反馈数据
	//3508电机反馈ID为0x201~0x208
    CAN_FilterInitTypeDef Filter;
    Filter.CAN_FilterNumber = 0;
    Filter.CAN_FilterMode = CAN_FilterMode_IdMask;
    Filter.CAN_FilterScale = CAN_FilterScale_32bit;
    Filter.CAN_FilterIdHigh = 0x0000;
    Filter.CAN_FilterIdLow = 0x0000;
    Filter.CAN_FilterMaskIdHigh = 0x0000;
    Filter.CAN_FilterMaskIdLow = 0x0000;
    Filter.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
    Filter.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&Filter);
}


//发送电流指令给电调（控制力矩）
//group: 传入1代表控制1-4号，传入2代表控制5-8号
//iq1~iq4：目标电流值，范围[-16384,16384]
void Set_Motor_Group_Current(uint8_t group, int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4)
{
    CanTxMsg TxMessage;
    
    //根据组别选择 CAN ID
    if (group == 1)
	{
        TxMessage.StdId = CAN_CONTROL_ID_GROUP1; 	//0x200
    }
	else
	{
        TxMessage.StdId = CAN_CONTROL_ID_GROUP2; 	//0x1FF
    }
    
    TxMessage.IDE = CAN_Id_Standard;
    TxMessage.RTR = CAN_RTR_Data;
    TxMessage.DLC = 8;

    //填充数据 (无论哪一组，组内的相对偏移是一样的)
    TxMessage.Data[0] = (uint8_t)(iq1 >> 8);
    TxMessage.Data[1] = (uint8_t)iq1;
    TxMessage.Data[2] = (uint8_t)(iq2 >> 8);
    TxMessage.Data[3] = (uint8_t)iq2;
    TxMessage.Data[4] = (uint8_t)(iq3 >> 8);
    TxMessage.Data[5] = (uint8_t)iq3;
    TxMessage.Data[6] = (uint8_t)(iq4 >> 8);
    TxMessage.Data[7] = (uint8_t)iq4;

    CAN_Transmit(CAN1, &TxMessage);
}
