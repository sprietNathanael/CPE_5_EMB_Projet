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

#define A1_ON 0x11
#define A1_OFF 0x10
#define A2_ON 0x21
#define A2_OFF 0x20
#define A_BRIGHT 0xF1
#define A_DIMM 0xF0

#define A1_ON_STATUS 'A'
#define A1_OFF_STATUS 'a'
#define A2_ON_STATUS 'B'
#define A2_OFF_STATUS 'b'

#define X10_A1_ON_ADDR 0x60
#define X10_A1_ON_DATA 0x00
#define X10_A1_OFF_ADDR 0x60
#define X10_A1_OFF_DATA 0x20

#define X10_A2_ON_ADDR 0x60
#define X10_A2_ON_DATA 0x10
#define X10_A2_OFF_ADDR 0x60
#define X10_A2_OFF_DATA 0x30

#define X10_A_DIMM_UP_ADDR 0x60
#define X10_A_DIMM_UP_DATA 0x88
#define X10_A_BRIGHT_ADDR 0x60
#define X10_A_DIMM_DATA 0x98


void GPIO_Thread(void const *argument);
void x10_init(void);
void nextX10frame(void);
void x10sendFrame(uint8_t addr_frame, uint8_t data_frame);
void x10sendSingleFrame(uint8_t addr_frame, uint8_t data_frame);
void turnOnLamp1(void);
void turnOffLamp1(void);
void turnOnLamp2(void);
void turnOffLamp2(void);

#endif /* __X10_H */
