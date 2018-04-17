#include <stdint.h>
#include "util.h"
#include "ST7735.h"

int curXMin, curXMax, curYMin, curYMax;
void ST7735_XYplotInit(int32_t minX, int32_t maxX, int32_t minY, int32_t maxY){
	ST7735_FillScreen(0);
	ST7735_PlotClear(minY, maxY);
	ST7735_SetCursor(0,0);
	curXMin = minX;
	curXMax = maxX;
	curYMin = minY;
	curYMax = maxY;
}

int curX = 0;
void drawNext(uint32_t y, uint16_t color, int addX) {
	if (curX == 0) ST7735_PlotClear(curYMin, curYMax);
	int realX = (curX - curXMin) * 128 / (curXMax - curXMin);
	int realY = 32 + (curYMax - y) * 128 / (curYMax - curYMin);
	ST7735_DrawPixel(realX, realY, color);
	if (addX) curX = (curX + 1)%128;
}
