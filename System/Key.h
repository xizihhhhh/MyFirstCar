#ifndef __KEY_H
#define __KEY_H

typedef enum
{
	IDLE = 0,
	IS_PRESSED = 1,
	PRESSED = 2,
	RELEASE = 3,
} Key_State;

void Key_Init(void);
void Key(void);

#endif
