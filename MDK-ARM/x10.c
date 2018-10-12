#include "x10.h"

static void sendInit(void);
static void sendFrame(void);

uint32_t currentFrame;
int currentDuration = 0;
int durationToReach = 0;
char timerActive = 0;
int currentFrameIndex = 0;
int initialPhase = 0;
int initialActive = 1;
int timerEnd = 0;


void gpioPG6_init()
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
  uint32_t PreviousWakeTime = osKernelSysTick();
	int i  = 0;
	//x10sendFrame(0x6666);
  
  for(;;)
  {
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_6, GPIO_PIN_RESET);
		for(i = 0; i < 5; i++)
		{
			x10sendFrame(0x60,0x10);
			osDelay(150);
		}			
		osDelay(13000);
		for(i = 0; i < 5; i++)
		{
			x10sendFrame(0x60,0x30);
			osDelay(150);
		}
		osDelay(13000);
		// D2
		//HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_6);
  }
}

void x10sendFrame(uint8_t addr_frame, uint8_t data_frame)
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