#include "x10.h"

static void sendInit(void);
static void sendFrame(void);
void brightA(void);
void dimmA(void);

uint32_t currentFrame;
int currentDuration = 0;
int durationToReach = 0;
char timerActive = 0;
int currentFrameIndex = 0;
int initialPhase = 0;
int initialActive = 1;
int timerEnd = 0;
uint8_t addr_frame;
uint8_t data_frame;

char lampStatus_data;

osEvent sendMessageX10_event;
extern osMessageQId sendMessageX10;

extern osMessageQId touchscreenLamp1Status;
extern osMessageQId touchscreenLamp2Status;
extern osMessageQId httpLamp1Status;
extern osMessageQId httpLamp2Status;


void x10_init()
{
	GPIO_InitTypeDef  gpio_init_structure;
	/* Enable the GPIO clock */
	__HAL_RCC_GPIOG_CLK_ENABLE();

	/* Configure the GPIO pin */
	gpio_init_structure.Pin = GPIO_PIN_6;
	gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init_structure.Pull = GPIO_PULLUP;
	gpio_init_structure.Speed = GPIO_SPEED_HIGH;

	HAL_GPIO_Init(GPIOG, &gpio_init_structure);
	
	/* By default, turn off LED */
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
}

void GPIO_Thread(void const *argument)
{
  (void) argument;
	x10_init();
  uint32_t PreviousWakeTime = osKernelSysTick();
	int i  = 0;
  
  for(;;)
  {
		osDelay(500);
		sendMessageX10_event = osMessageGet(sendMessageX10, 0);
		if(sendMessageX10_event.status == osEventMessage)
		{
			if(sendMessageX10_event.value.v == A1_ON)
			{
				turnOnLamp1();
				x10sendFrame(addr_frame,data_frame);
			}
			else if(sendMessageX10_event.value.v == A1_OFF)
			{
				turnOffLamp1();
				x10sendFrame(addr_frame,data_frame);
			}
			else if(sendMessageX10_event.value.v == A2_ON)
			{
				turnOffLamp2();
				x10sendFrame(addr_frame,data_frame);
				osDelay(500);
				turnOnLamp2();
				x10sendFrame(addr_frame,data_frame);
			}
			else if(sendMessageX10_event.value.v == A2_OFF)
			{
				turnOffLamp2();
				x10sendFrame(addr_frame,data_frame);
			}
			else if(sendMessageX10_event.value.v == A_BRIGHT)
			{
				brightA();
				x10sendFrame(addr_frame,data_frame);
			}
			else if(sendMessageX10_event.value.v == A_DIMM)
			{
				dimmA();
				x10sendFrame(addr_frame,data_frame);
			}
		}
  }
}

void x10sendFrame(uint8_t addr_frame, uint8_t data_frame)
{
	int i  = 0;
	for(i = 0; i < 5; i++)
		{
			x10sendSingleFrame(addr_frame,data_frame);
			osDelay(150);
		}			
}

void turnOnLamp1(void)
{
	
	lampStatus_data = A1_ON_STATUS;
	osMessagePut(touchscreenLamp1Status, (uint32_t)lampStatus_data, 0);
	osMessagePut(httpLamp1Status, (uint32_t)lampStatus_data, 0);
	addr_frame = X10_A1_ON_ADDR;
	data_frame = X10_A1_ON_DATA;
}

void turnOffLamp1(void)
{
	
	lampStatus_data = A1_OFF_STATUS;
	osMessagePut(touchscreenLamp1Status, (uint32_t)lampStatus_data, 0);
	osMessagePut(httpLamp1Status, (uint32_t)lampStatus_data, 0);
	addr_frame = X10_A1_OFF_ADDR;
	data_frame = X10_A1_OFF_DATA;
}

void turnOnLamp2(void)
{
	
	lampStatus_data = A2_ON_STATUS;
	osMessagePut(touchscreenLamp2Status, (uint32_t)lampStatus_data, 0);
	osMessagePut(httpLamp2Status, (uint32_t)lampStatus_data, 0);
	addr_frame = X10_A2_ON_ADDR;
	data_frame = X10_A2_ON_DATA;
}

void turnOffLamp2(void)
{
	
	lampStatus_data = A2_OFF_STATUS;
	osMessagePut(touchscreenLamp2Status, (uint32_t)lampStatus_data, 0);
	osMessagePut(httpLamp2Status, (uint32_t)lampStatus_data, 0);
	addr_frame = X10_A2_OFF_ADDR;
	data_frame = X10_A2_OFF_DATA;
}

void brightA(void)
{
	lampStatus_data = A2_UNDETERMINED;
	osMessagePut(touchscreenLamp2Status, (uint32_t)lampStatus_data, 0);
	osMessagePut(httpLamp2Status, (uint32_t)lampStatus_data, 0);
	addr_frame = X10_A_BRIGHT_ADDR;
	data_frame = X10_A_BRIGHT_DATA;
}

void dimmA(void)
{
	lampStatus_data = A2_UNDETERMINED;
	osMessagePut(touchscreenLamp2Status, (uint32_t)lampStatus_data, 0);
	osMessagePut(httpLamp2Status, (uint32_t)lampStatus_data, 0);
	addr_frame = X10_A_DIMM_ADDR;
	data_frame = X10_A_DIMM_DATA;
}

void x10sendSingleFrame(uint8_t addr_frame, uint8_t data_frame)
{
	currentFrameIndex = 32;
	uint8_t addr_frame_inv = (~addr_frame);
	uint8_t data_frame_inv = (~data_frame);
	currentFrame = (addr_frame << 24) | (addr_frame_inv << 16) | (data_frame << 8) | data_frame_inv;
	timerActive = 1;
	timerEnd = 0;
	currentDuration = 0;
	durationToReach = 0;
	initialPhase = 2;
	initialActive = 1;
}

static void sendInit(void)
{
	if(currentDuration == durationToReach)
	{
		if(initialPhase == 2)
		{
			currentDuration = 0;
			durationToReach = X10_INIT_1_TIMING;
			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
		}
		else if(initialPhase == 1)
		{
			currentDuration = 0;
			durationToReach = X10_INIT_2_TIMING;
			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
		}
		initialPhase --;
		
	}
	currentDuration ++;
	if(initialPhase == -1)
	{
		initialActive = 0;
		currentDuration = 0;
		durationToReach = 0;
	}
}

static void sendFrame(void)
{
		if(currentFrameIndex > 0 || currentDuration != durationToReach)
		{
			if(currentDuration == durationToReach)
			{
				char currentBit = ((currentFrame & 0x80000000) >> 31)&1;
				durationToReach = currentBit ? X10_1_TIMING : X10_0_TIMING;
				currentDuration = 0;
				currentFrame = currentFrame << 1;
				currentFrameIndex--;
			}
			
			if(currentDuration < (durationToReach / 2))
			{
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
			}
			else
			{
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
			}
			currentDuration ++;
		}
		else
		{
			if(!timerEnd)
			{
				HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_SET);
				currentDuration = 0;
				durationToReach = 2;				
				timerEnd = 1;
			}
			else
			{
					HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
					timerActive = 0;
			}
		}
}

void nextX10frame(void)
{
	if(timerActive)
	{
		if(initialActive)
		{
			sendInit();
		}
		if(!initialActive)
		{
			sendFrame();
		}
		
	}
}