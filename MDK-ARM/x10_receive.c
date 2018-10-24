#include "x10_receive.h"

static uint32_t getUs(void);
static void x10_bit_received(int bit);
static void x10_init_received(void);
static void readBuffer(void);
static void sendMessage(uint8_t addr, uint8_t data);

extern osMessageQId touchscreenLamp1Status;
extern osMessageQId touchscreenLamp2Status;
extern osMessageQId httpLamp1Status;
extern osMessageQId httpLamp2Status;

uint32_t currentReceivedFrame = 0;
uint32_t frameBuffer[10];
char bufferIndex = 0;
char bitCounter = 0;
char reading = 0;

uint32_t previousReceivedTime = 0;
uint32_t previousGoodReceivedTime = 0;



void x10Init_receive()
{
	GPIO_InitTypeDef  gpio_init_structure;
	
	
	/* Enable the GPIO clock */
	__HAL_RCC_GPIOI_CLK_ENABLE();

	/* Configure the GPIO pin D7 */
	gpio_init_structure.Pin = GPIO_PIN_3;
	gpio_init_structure.Mode = GPIO_MODE_IT_RISING;
	gpio_init_structure.Pull = GPIO_PULLDOWN;
	gpio_init_structure.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOI, &gpio_init_structure);
	
	HAL_NVIC_SetPriority(EXTI3_IRQn, 3, 0);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);
}

void x10Thread_receive(void const *argument)
{
  (void) argument;
  uint32_t PreviousWakeTime = osKernelSysTick();
	int i  = 0;
	x10Init_receive();
  
  for(;;)
  {
		osDelay(100);
		readBuffer();
		/*if(HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_7))
		{
			osMessagePut(touchscreenLamp2Status, (uint32_t)A2_ON_STATUS, 0);
			osMessagePut(httpLamp2Status, (uint32_t)A2_ON_STATUS, 0);
		}
		else
		{
			osMessagePut(touchscreenLamp2Status, (uint32_t)A2_OFF_STATUS, 0);
			osMessagePut(httpLamp2Status, (uint32_t)A2_OFF_STATUS, 0);
		}*/
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint32_t currentTime = getUs();
	uint32_t delta;
	if(previousReceivedTime != 0)
	{
		delta = currentTime - previousReceivedTime;
		if(delta >= X10_0_TIMING_US_LOW)
		{
			if(delta > X10_0_TIMING_US_LOW && delta < X10_0_TIMING_US_HIGH)
			{
				x10_bit_received(0);
			}
			else if(delta > X10_1_TIMING_US_LOW && delta < X10_1_TIMING_US_HIGH)
			{
				x10_bit_received(1);
			}
			else if(delta > X10_INIT_TIMING_US_LOW && delta < X10_INIT_TIMING_US_HIGH)
			{
				x10_init_received();
			}
			previousReceivedTime = currentTime;
		}
	}
	else
	{
		previousReceivedTime = currentTime;
	}
}

static void x10_bit_received(int bit)
{
	if(reading)
	{
		currentReceivedFrame = currentReceivedFrame << 1;
		currentReceivedFrame += bit;
		bitCounter++;
		if(bitCounter == 32)
		{
			frameBuffer[bufferIndex] = currentReceivedFrame;
			bufferIndex++;
			reading = 0;
		}
	}
}

static void x10_init_received()
{
	currentReceivedFrame = 0;
	bitCounter = 0;
	reading = 1;
}

static void readBuffer(void)
{
	uint8_t addr, data, addr_inv, data_inv, test;
	if(bufferIndex > 0)
	{
		data_inv = frameBuffer[bufferIndex-1];
		data = frameBuffer[bufferIndex-1] >> 8;
		test = ~data;
		addr_inv = frameBuffer[bufferIndex-1] >> 16;
		addr = frameBuffer[bufferIndex-1] >> 24;
		if((data_inv == (uint8_t)(~data)) && (addr_inv == (uint8_t)(~addr)))
		{
			sendMessage(addr,data);
		}
		bufferIndex --;
	}
}

static void sendMessage(uint8_t addr, uint8_t data)
{
	if(addr == X10_A1_ON_ADDR && data == X10_A1_ON_DATA)
	{
		osMessagePut(touchscreenLamp1Status, (uint32_t)A1_ON_STATUS, 0);
		osMessagePut(httpLamp1Status, (uint32_t)A1_ON_STATUS, 0);
	}
	else if(addr == X10_A1_OFF_ADDR && data == X10_A1_OFF_DATA)
	{
		osMessagePut(touchscreenLamp1Status, (uint32_t)A1_OFF_STATUS, 0);
		osMessagePut(httpLamp1Status, (uint32_t)A1_OFF_STATUS, 0);
	}
	else if(addr == X10_A2_ON_ADDR && data == X10_A2_ON_DATA)
	{
		osMessagePut(touchscreenLamp2Status, (uint32_t)A2_ON_STATUS, 0);
		osMessagePut(httpLamp2Status, (uint32_t)A2_ON_STATUS, 0);
	}
	else if(addr == X10_A2_OFF_ADDR && data == X10_A2_OFF_DATA)
	{
		osMessagePut(touchscreenLamp2Status, (uint32_t)A2_OFF_STATUS, 0);
		osMessagePut(httpLamp2Status, (uint32_t)A2_OFF_STATUS, 0);
	}
}

static uint32_t getUs(void)
{
	uint32_t usTicks = HAL_RCC_GetSysClockFreq() / 1000000;
	register uint32_t ms, cycle_cnt;
	
	do{
		ms = HAL_GetTick();
		cycle_cnt = SysTick->VAL;
	}while(ms != HAL_GetTick());
	
	return(ms*1000) + (usTicks*1000 - cycle_cnt) / usTicks;
}
