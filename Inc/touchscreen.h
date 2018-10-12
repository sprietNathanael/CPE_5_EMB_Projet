#ifndef __TOUCHSCREEN_H
#define __TOUCHSCREEN_H

#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
#include "cmsis_os.h"
#include "x10.h"

int pointInRect(int rectX, int rectY, int rectSizeX, int rectSizeY, int pointX, int pointY);

void changeBulbState(int state);
void TouchScreen_Thread(void const *argument);

#endif /* __TOUCHSCREEN_H */