#include "stm32f10x.h"                  // Device header
#include "Key.h"

Key_State keystate = IDLE;
extern float Target_L;
extern float Target_R;
extern volatile uint8_t PIDControl_Flag;

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void Key(void)
{
	uint8_t level = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);
	static uint8_t BoolKey = 0;
	
	switch (keystate)
	{
		case IDLE:
			if (level == 0)
			{	
				keystate = IS_PRESSED;
			}
				break;

		case IS_PRESSED:
			if (level == 0)
			{
				BoolKey = (BoolKey + 1) % 2;
				keystate = PRESSED;
				if (BoolKey == 0)
				{
					PIDControl_Flag = 0;
					Target_L = 0;
					Target_R = 0;
//					GPIO_ResetBits(GPIOB, GPIO_Pin_13);
				}
				else
				{
					PIDControl_Flag = 1;
					Target_L = 10;
					Target_R = 10;
//					GPIO_SetBits(GPIOB, GPIO_Pin_13);
				}
		
			}
			else
			{
				keystate = IDLE;
			}
			break;

		case PRESSED:
			if (level == 1)
			{
				keystate = RELEASE;
			}
			break;

		case RELEASE:
			if (level == 1)
			{
				keystate = IDLE;
			}
			else
			{
				keystate = PRESSED;
			}
			break;
    }
}
