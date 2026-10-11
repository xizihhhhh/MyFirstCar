#include "stm32f10x.h"	// Device header
#include "Timer.h"
#include "GPIO.h"
#include "OLED.h"
#include "Key.h"
#include "Encoder.h"
#include "Serial.h"
#include "pid.h"
#include "tracing.h"
 
float Target_L, Actual_L, Out_L;
float Error_Now_L, Error_Last_L, Error_Add_L;

float Target_R, Actual_R, Out_R;
float Error_Now_R, Error_Last_R, Error_Add_R;

volatile uint8_t TIM2_L_Flag = 0;
volatile uint8_t TIM2_R_Flag = 0;
volatile uint8_t Serial_Flag = 0;
volatile uint8_t PIDControl_Flag = 0;

PID_Parameter L_PID = {
	.Kp = 1,
	.Ki = 0.2,
	.Kd = 0,
};

PID_Parameter R_PID = {
	.Kp = 1,
	.Ki = 0.2,
	.Kd = 0,
};

int main(void)
{
//	OLED_Init();
	Serial_Init();
	
	GPIOA_PWM_Init();
	GPIOB_STBY_Init();
	Key_Init();
	Timer1_Init();
	Timer2_Init();
	PWM_Timer2_CH1_Init();
	PWM_Timer2_CH2_Init();
	GPIO_MotorDirection_Init();
	EncoderL_Init();
	EncoderR_Init();
	Tracing_Init();
	
//	OLED_ShowString(0, 0, "CHT:", OLED_8X16);
//	
//	OLED_Update();
	
	Target_L = 0;
	Target_R = 0;
	while (1)
	{	

		if (TIM2_L_Flag == 1)
		{
			/*Left*/

			Actual_L = Encoder_Get();
			
//			OLED_Printf(0, 0, OLED_8X16, "CHTL:%f", Actual_L);
			
			Error_Last_L = Error_Now_L;
			Error_Now_L = Target_L - Actual_L;
				
			/*误差积分*/
			Error_Add_L += Error_Now_L;
					
			Out_L = L_PID.Kp * Error_Now_L + L_PID.Ki * Error_Add_L + L_PID.Kd * (Error_Now_L - Error_Last_L);
			
			if (Out_L > 100)
			{
				Out_L = 100;
			}
			if (Out_L < 0)
			{
				Out_L = 0;
			}
			
//			OLED_Printf(0, 33, OLED_8X16, "OUTL:%.4f", Out_L);
//			OLED_Update();
			TIM_SetCompare2(TIM2, Out_L);
			
			TIM2_L_Flag = 0;
		}
		
		if (TIM2_R_Flag == 1)
		{
			/*Right*/
			Actual_R = EncoderR_Get();
			
//			OLED_Printf(0, 17, OLED_8X16, "CHTR:%f", Actual_R);
			
			Error_Last_R = Error_Now_R;
			Error_Now_R = Target_R - Actual_R;
				
			/*误差积分*/
			Error_Add_R += Error_Now_R;
					
			Out_R = R_PID.Kp * Error_Now_R + R_PID.Ki * Error_Add_R + R_PID.Kd * (Error_Now_R - Error_Last_R);
			
			if (Out_R > 100)
			{
				Out_R = 100;
			}
			if (Out_R < 0)
			{
				Out_R = 0;
			}
			
//			OLED_Printf(0, 49, OLED_8X16, "OUTR:%.4f", Out_R);
//			OLED_Update();
			TIM_SetCompare1(TIM2, Out_R);
			
			TIM2_R_Flag = 0;
		}
		
		if (Serial_Flag == 1)
		{
			Serial_Printf("%.2f\n", Actual_L);
			Serial_Flag = 0;
		}
	}
}

void TIM2_IRQHandler(void)
{
	static uint16_t i = 0;
	
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
	{
		i = (i + 1) % 100;
		
		if (i == 99)
		{
			Key();
			TIM2_L_Flag = 1;
			TIM2_R_Flag = 1;
			Serial_Flag = 1;
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

void TIM1_UP_IRQHandler(void)
{
	static uint16_t i = 0;
	
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		if (PIDControl_Flag == 1)
		{
			Tracing_Control();
		}
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
	}
}
