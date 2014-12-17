#ifndef _KEY_H_
#define _KEY_H_
#include "stm32f10x.h"


#define key_gotoapi 1
#define key_updateapp 2

u8 GetKeyIn (uint16_t GPIO_PIN);
void KeyInit (void);

#endif
