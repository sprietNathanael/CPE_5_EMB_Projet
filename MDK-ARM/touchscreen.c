#include "touchscreen.h"

#define BULB1_X BSP_LCD_GetXSize()/4
#define BULB2_X 3*(BSP_LCD_GetXSize()/4)

static void displayBaseBulb(int bulbNumer);
static void displayButton(int x, int y, int sizeX, int sizeY, uint8_t* text, uint32_t color);
static void displayBulbState(int bulbNumer, int state);
int statusBulbState = -1;
extern osMessageQId touchscreenLamp1Status;
extern osMessageQId touchscreenLamp2Status;
extern osMessageQId sendMessageX10;
char sendMessageX10_data;

void TouchScreen_Thread(void const *argument)
{
	
	osEvent touchscreenLamp1Status_event;
	osEvent touchscreenLamp2Status_event;
	static TS_StateTypeDef  TS_State;
	int status = 0;
	int x = 0;
	int y = 0;
	
	const int on1X = BSP_LCD_GetXSize()/10;
	const int on1Y = (BSP_LCD_GetYSize() / 4);
	const int on1SizeX = BSP_LCD_GetXSize()/10;
	const int on1SizeY = 35;
	
	const int off1X = 3*(BSP_LCD_GetXSize()/10);
	const int off1Y = (BSP_LCD_GetYSize() / 4);
	const int off1SizeX = BSP_LCD_GetXSize()/10;
	const int off1SizeY = 35;
	
	
	const int on2X = 6*(BSP_LCD_GetXSize()/10);
	const int on2Y = (BSP_LCD_GetYSize() / 4);
	const int on2SizeX = BSP_LCD_GetXSize()/10;
	const int on2SizeY = 35;
	
	const int off2X = 8*(BSP_LCD_GetXSize()/10);
	const int off2Y = (BSP_LCD_GetYSize() / 4);
	const int off2SizeX = BSP_LCD_GetXSize()/10;
	const int off2SizeY = 35;
	
	const int brightX = 8*(BSP_LCD_GetXSize()/10);
	const int brightY = 3*(BSP_LCD_GetYSize() / 4);
	const int brightSizeX = BSP_LCD_GetXSize()/10;
	const int brightSizeY = 35;
	
	const int dimmX = 6*(BSP_LCD_GetXSize()/10);
	const int dimmY = 3*(BSP_LCD_GetYSize() / 4);
	const int dimmSizeX = BSP_LCD_GetXSize()/10;
	const int dimmSizeY = 35;
	uint8_t  text[50];
	displayButton(on1X, on1Y, on1SizeX, on1SizeY, (uint8_t*)"ON", LCD_COLOR_YELLOW);
	displayButton(off1X, off1Y, off1SizeX, off1SizeY, (uint8_t*)"OFF", LCD_COLOR_LIGHTGRAY);
	displayButton(on2X, on2Y, on2SizeX, on2SizeY, (uint8_t*)"ON", LCD_COLOR_YELLOW);
	displayButton(off2X, off2Y, off2SizeX, off2SizeY, (uint8_t*)"OFF", LCD_COLOR_LIGHTGRAY);
	displayButton(brightX, brightY, brightSizeX, brightSizeY, (uint8_t*)"+20%", LCD_COLOR_YELLOW);
	displayButton(dimmX, dimmY, dimmSizeX, dimmSizeY, (uint8_t*)"-20%", LCD_COLOR_LIGHTGRAY);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_DisplayStringAt(2*(BSP_LCD_GetXSize()/10)+12, (BSP_LCD_GetYSize() / 4)+11, (uint8_t*)"A1", LEFT_MODE);
	BSP_LCD_DisplayStringAt(7*(BSP_LCD_GetXSize()/10)+12, (BSP_LCD_GetYSize() / 4)+11, (uint8_t*)"A2", LEFT_MODE);
	BSP_LCD_SetFont(&Font12);
	
	
	
	status = BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());

  if (status != TS_OK)
  {
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 95, (uint8_t *)"ERROR", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 80, (uint8_t *)"Touchscreen cannot be initialized", CENTER_MODE);
  }
	
	displayBaseBulb(1);
	displayBaseBulb(2);
	displayBulbState(1,-1);
	displayBulbState(2,-1);
	
	for(;;)
  {
		if (status == TS_OK)
    {
      /* Check in polling mode in touch screen the touch status and coordinates */
      /* if touch occurred                                                      */
      BSP_TS_GetState(&TS_State);
      if(TS_State.touchDetected)
      {
        /* Get X and Y position of the touch post calibrated */
        x = TS_State.touchX[0];
        y = TS_State.touchY[0];

        BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				if(pointInRect(on1X, on1Y, on1SizeX, on1SizeY, x, y))
				{
					sendMessageX10_data = A1_ON;
					osMessagePut(sendMessageX10, (uint32_t)sendMessageX10_data, 0);
				}
				else if(pointInRect(off1X, off1Y, off1SizeX, off1SizeY, x, y))
				{
					sendMessageX10_data = A1_OFF;
					osMessagePut(sendMessageX10, (uint32_t)sendMessageX10_data, 0);
				}
				else if(pointInRect(on2X, on2Y, on2SizeX, on2SizeY, x, y))
				{
					sendMessageX10_data = A2_ON;
					osMessagePut(sendMessageX10, (uint32_t)sendMessageX10_data, 0);
				}
				else if(pointInRect(off2X, off2Y, off2SizeX, off2SizeY, x, y))
				{
					sendMessageX10_data = A2_OFF;
					osMessagePut(sendMessageX10, (uint32_t)sendMessageX10_data, 0);
				}
				else if(pointInRect(brightX, brightY, brightSizeX, brightSizeY, x, y))
				{
					sendMessageX10_data = A_BRIGHT;
					osMessagePut(sendMessageX10, (uint32_t)sendMessageX10_data, 0);
				}				
				else if(pointInRect(dimmX, dimmY, dimmSizeX, dimmSizeY, x, y))
				{
					sendMessageX10_data = A_DIMM;
					osMessagePut(sendMessageX10, (uint32_t)sendMessageX10_data, 0);
				}

			}
		}
		touchscreenLamp1Status_event = osMessageGet(touchscreenLamp1Status, 0);
		if(touchscreenLamp1Status_event.status == osEventMessage)
		{
			if(touchscreenLamp1Status_event.value.v == A1_ON_STATUS)
			{
				displayBulbState(1,1);
			}
			else if(touchscreenLamp1Status_event.value.v == A1_OFF_STATUS)
			{
				displayBulbState(1,0);
			}
		}
		touchscreenLamp2Status_event = osMessageGet(touchscreenLamp2Status, 0);
		if(touchscreenLamp2Status_event.status == osEventMessage)
		{
			if(touchscreenLamp2Status_event.value.v == A2_ON_STATUS)
			{
				displayBulbState(2,1);
			}
			else if(touchscreenLamp2Status_event.value.v == A2_OFF_STATUS)
			{
				displayBulbState(2,0);
			}
			else if(touchscreenLamp2Status_event.value.v == A2_UNDETERMINED)
			{
				displayBulbState(2,-1);
			}
		}
		osDelay(50);
	}
}

static int pointInRect(int rectX, int rectY, int rectSizeX, int rectSizeY, int pointX, int pointY)
{
	if(pointX < rectX) return 0;
	if(pointY < rectY) return 0;
	if(pointX > rectX + rectSizeX) return 0;
	if(pointY > rectY + rectSizeY) return 0;
	
	return 1;
}

static void displayBaseBulb(int bulbNumber)
{
	int x;
	if(bulbNumber == 1)
	{
		x = BULB1_X;
	}
	else
	{
		x = BULB2_X;
	}
	int radius = 30;
	int border = 5;
	int rectY = 20;
	int rectWidth = 40;
	int rectHeight = 20;
	int y =  BSP_LCD_GetYSize()/2;
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillCircle(x, y+rectY+rectHeight, (rectWidth/2)-1);
	BSP_LCD_FillCircle(x, y, radius);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(x-(rectWidth/2),y+rectY, rectWidth, rectHeight);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect((x-(rectWidth/2)),y+rectY+rectHeight, rectWidth, border);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
}

static void displayButton(int x, int y, int sizeX, int sizeY, uint8_t* text, uint32_t color)
{
	BSP_LCD_SetTextColor(color);
	BSP_LCD_FillRect(x, y, sizeX, sizeY);
	BSP_LCD_SetBackColor(color);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(x+15, y +15, text, LEFT_MODE);
	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
}

static void displayBulbState(int bulbNumber, int state)
{
	uint32_t color;
	
	int radius = 30;
	int border = 5;
	int rectY = 20;
	int rectWidth = 40;
	int rectHeight = 20;
	int x;
	if(bulbNumber == 1)
	{
		x = BULB1_X;
	}
	else
	{
		x = BULB2_X;
	}
	
	int y =  BSP_LCD_GetYSize()/2;
	if(state == 1)
	{
		color = LCD_COLOR_YELLOW;
	
	}
	else
	{
		color = LCD_COLOR_WHITE;
	}
	BSP_LCD_SetTextColor(color);
	BSP_LCD_FillCircle(x, y, radius - border);
	BSP_LCD_FillRect((x-(rectWidth/2) + border),y+rectY, rectWidth-(border * 2), rectHeight-border);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	
	if(state == -1)
	{
		BSP_LCD_SetFont(&Font24);
		BSP_LCD_DisplayStringAt(x-6, y-12, (uint8_t *)"?", LEFT_MODE);
		BSP_LCD_SetFont(&Font12);
	}
}
