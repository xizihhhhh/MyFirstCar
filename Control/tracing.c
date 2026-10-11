#include "stm32f10x.h"                  // Device header
#include "pid.h"
#include "tracing.h"

uint8_t ray[5] = {1, 1, 1, 1, 1};

float Out_T, Base_Speed = 10;
float Error_Now_T, Error_Last_T, Error_Add_T;

extern float Target_L, Target_R;

PID_Parameter Tracing_PID = {
	.Kp = 0.5,
	.Ki = 0.5,
	.Kd = 0,
};

/**
  * @brief Use PB5, PB0, PB1, PB10, PB11 for infrared transducer(From right to left).
  * When the object absorb the infrared ray, the corresponding channel sets the bit to 0.
  * On the contrary, the corresponding channel will sets the bit to 1.
  * 
  * From left to right, the number are 0, 1, 2, 3, 4.
  */
void Tracing_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5 | GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

uint8_t Tracing_Error(uint8_t channel)
{
	uint8_t value = 1;
	switch (channel)
	{
		case 0:
			if (PBin(5) == 0)
				value = 1;
			else
				value = 0;
			break;
		case 1:
			if (PBin(0) == 0)
				value = 1;
			else
				value = 0;
			break;
		case 2:
			if (PBin(1) == 0)
				value = 1;
			else
				value = 0;
			break;
		case 3:
			if (PBin(10) == 0)
				value = 1;
			else
				value = 0;
			break;
		case 4:
			if (PBin(11) == 0)
				value = 1;
			else
				value = 0;
			break;
	}
	return value;
}

void Tracing_Control(void)
{
	uint8_t i = 0;
	for (i = 0; i < 5; i++)
	{
		ray[i] = Tracing_Error(i);
	}
	
	Error_Last_T = Error_Now_T;
	Error_Now_T = -2.5 * ray[0] + -1 * ray[1] + 0 * ray[2] + 1 * ray[3] + 2.5 * ray[4];
	/*
	 *Error Right = -
	 *Error Left  = +
	 */
				
	/*误差积分*/
//	Error_Add_T += Error_Now_T;
					
	Out_T = Tracing_PID.Kp * Error_Now_T+ Tracing_PID.Kd * (Error_Now_T - Error_Last_T);
//	 + Tracing_PID.Ki * Error_Add_T 
	/*输出限幅*/
	if (Out_T > 3)
		Out_T = 3;
	if (Out_T < -3)
		Out_T = -3;
	
	/*积分限幅*/
	if (Error_Add_T >  2)
		Error_Add_T =  2;
	if (Error_Add_T < -2)
		Error_Add_T = -2;
	
	PWM_Control();
}

void PWM_Control(void)
{
		Target_L = Base_Speed - Out_T;
		Target_R = Base_Speed + Out_T;
}
