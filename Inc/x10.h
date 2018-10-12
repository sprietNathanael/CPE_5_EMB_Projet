#ifndef __X10_H
#define __X10_H

#include "stm32f7xx_hal.h"
#include "stm32746g_discovery.h"
#include "cmsis_os.h"

#define X10_BASE_TIMING 562
#define X10_1_TIMING 4
#define X10_0_TIMING 2
#define X10_INIT_1_TIMING 16
#define X10_INIT_2_TIMING 8

void GPIO_Thread(void const *argument);
void x10_init(void);
void nextX10frame(void);
void x10sendFrame(uint8_t addr_frame, uint8_t data_frame);
void x10sendSingleFrame(uint8_t addr_frame, uint8_t data_frame);
void turnOnLamp(void);
void turnOffLamp(void);

#endif /* __X10_H */
