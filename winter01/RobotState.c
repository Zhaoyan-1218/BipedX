#include <stdio.h>

//定义结构体
typedef struct
{
    float angle;      //关节角度
    float velocity;   //关节角速度
    int is_active;    //运行状态：1为使能，0为停止
}RobotState;

//指针函数：通过传入地址，直接修改结构体内部的值
void UpdateRobotStatus(RobotState *ptr, float new_angle, float new_vel)
{
    //先检查指针是否为空
    if (ptr != NULL)
    { 
        ptr->angle = new_angle;
        ptr->velocity = new_vel;
        ptr->is_active = 1;
    }
}

int main()
{
    //初始化一个结构体变量
    RobotState myRobot = {0.0f, 0.0f, 0};

    printf("--- 修改前的状态 ---\n");
    printf("角度: %.2f, 速度: %.2f, 运行状态: %d\n", myRobot.angle, myRobot.velocity, myRobot.is_active);

    //调用函数，注意要用 & 符号取地址传递给指针
    UpdateRobotStatus(&myRobot, 45.5f, 120.0f);

    printf("\n--- 修改后的状态 ---\n");
    printf("角度: %.2f, 速度: %.2f, 运行状态: %d\n", myRobot.angle, myRobot.velocity, myRobot.is_active);

    return 0;
}
