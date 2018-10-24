#ifndef __X10_RECEIVE_H
#define __X10_RECEIVE_H

#include "stm32f7xx_hal.h"
#include "stm32f7xx.h"
#include "stm32746g_discovery.h"
#include "cmsis_os.h"
#include "x10_send.h"

#define X10_1_TIMING_US 2250
#define X10_1_TIMING_US_LOW 2000
#define X10_1_TIMING_US_HIGH 2500
#define X10_0_TIMING_US 1125
#define X10_0_TIMING_US_LOW 1000
#define X10_0_TIMING_US_HIGH 1250
#define X10_INIT_TIMING_US 13500
#define X10_INIT_TIMING_US_LOW 13250
#define X10_INIT_TIMING_US_HIGH 13750

void x10Thread_receive(void const *argument);
void x10Init_receive(void);

#endif /* __X10_H */
