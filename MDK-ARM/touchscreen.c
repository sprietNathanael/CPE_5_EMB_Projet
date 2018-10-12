#include "touchscreen.h"



void TouchScreen_Thread(void const *argument)
{
	static TS_StateTypeDef  TS_State;
	int status = 0;
	int x = 0;
	int y = 0;
	const int onX = BSP_LCD_GetXSize()/5;
	const int onY = 2*(BSP_LCD_GetYSize() / 4) -15;
	const int onSizeX = BSP_LCD_GetXSize()/5;
	const int onSizeY = 45;
	
	const int offX = 3*(BSP_LCD_GetXSize()/5);
	const int offY = 2*(BSP_LCD_GetYSize() / 4) -15;
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
					turnOnLamp();
				}
				else if(pointInRect(offX, offY, offSizeX, offSizeY, x, y))
				{
					sprintf((char*)text, "%d ; %d of rect (%d;%d)[%d - %d] = OFF  ", x, y, offX, offY, offSizeX, offSizeY);
					turnOffLamp();
				}
				else
				{
					sprintf((char*)text, "%d ; %d NO   ", x, y);
				}
					
				BSP_LCD_DisplayStringAt(15, BSP_LCD_GetYSize() - 40, (uint8_t *)&text, LEFT_MODE);
				

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