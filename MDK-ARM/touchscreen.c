#include "touchscreen.h"

static void displayBaseBulb();
static void displayBulbState(int state);
int statusBulbState = -1;
extern osMessageQId touchscreenLampStatus;
extern osMessageQId sendMessageX10;
char sendMessageX10_data;

void TouchScreen_Thread(void const *argument)
{
	
	osEvent touchscreenLampStatus_event;
	static TS_StateTypeDef  TS_State;
	int status = 0;
	int x = 0;
	int y = 0;
	const int onX = BSP_LCD_GetXSize()/5;
	const int onY = (BSP_LCD_GetYSize() / 4);
	const int onSizeX = BSP_LCD_GetXSize()/5;
	const int onSizeY = 45;
	
	const int offX = 3*(BSP_LCD_GetXSize()/5);
	const int offY = (BSP_LCD_GetYSize() / 4);
	const int offSizeX = BSP_LCD_GetXSize()/5;
	const int offSizeY = 45;
	uint8_t  text[50];
	BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
	BSP_LCD_FillRect(onX, onY, onSizeX, onSizeY);
	BSP_LCD_SetBackColor(LCD_COLOR_YELLOW);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(onX + 40, onY +15, (uint8_t*)"ON", LEFT_MODE);
	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	
	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
	BSP_LCD_FillRect(offX, offY, offSizeX, offSizeY);
	BSP_LCD_SetBackColor(LCD_COLOR_LIGHTGRAY);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(offX + 40, offY +15, (uint8_t*)"OFF", LEFT_MODE);
	BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	
	status = BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());

  if (status != TS_OK)
  {
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 95, (uint8_t *)"ERROR", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize() - 80, (uint8_t *)"Touchscreen cannot be initialized", CENTER_MODE);
  }
	
	displayBaseBulb();
	displayBulbState(-1);
	
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
				if(pointInRect(onX, onY, onSizeX, onSizeY, x, y))
				{
					sprintf((char*)text, "%d ; %d of rect (%d;%d)[%d - %d] = ON  ", x, y, onX, onY, onSizeX, onSizeY);
					sendMessageX10_data = 'a';
					osMessagePut(sendMessageX10, (uint32_t)sendMessageX10_data, 0);
				}
				else if(pointInRect(offX, offY, offSizeX, offSizeY, x, y))
				{
					sprintf((char*)text, "%d ; %d of rect (%d;%d)[%d - %d] = OFF  ", x, y, offX, offY, offSizeX, offSizeY);
					sendMessageX10_data = 'e';
					osMessagePut(sendMessageX10, (uint32_t)sendMessageX10_data, 0);
				}
				else
				{
					sprintf((char*)text, "%d ; %d NO   ", x, y);
				}
					
				BSP_LCD_DisplayStringAt(15, BSP_LCD_GetYSize() - 40, (uint8_t *)&text, LEFT_MODE);
				

			}
		}
		touchscreenLampStatus_event = osMessageGet(touchscreenLampStatus, 0);
		if(touchscreenLampStatus_event.status == osEventMessage)
		{
			if(touchscreenLampStatus_event.value.v == 'a')
			{
				displayBulbState(1);
			}
			else if(touchscreenLampStatus_event.value.v == 'e')
			{
				displayBulbState(0);
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

static void displayBaseBulb(void)
{
	int radius = 30;
	int border = 5;
	int rectY = 20;
	int rectWidth = 40;
	int rectHeight = 20;
	int x = BSP_LCD_GetXSize()/2;
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

static void displayBulbState(int state)
{
	uint32_t color;
	
	int radius = 30;
	int border = 5;
	int rectY = 20;
	int rectWidth = 40;
	int rectHeight = 20;
	int x = BSP_LCD_GetXSize()/2;
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
