#include <stdint.h>
#include <math.h>
#include "ST7735.h"
#include <stdlib.h>


int curXMin, curXMax, curYMin, curYMax;

int max(int a, int b){
	if(a > b) return a;
	else return b;
}

void ST7735_XYplotInit(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY){
	ST7735_FillScreen(0);
	ST7735_PlotClear(minY, maxY);
	ST7735_SetCursor(0,0);
	ST7735_OutString(title);
	curXMin = minX;
	curXMax = maxX;
	curYMin = minY;
	curYMax = maxY;
}

void ST7735_XYplot(uint32_t num, int32_t bufX[], int32_t bufY[]){
	for(int i = 0; i < num; i++){
		int x = bufX[i];
		int y = bufY[i];
		if(x <= curXMax && x >= curXMin && y <= curYMax && y >= curYMin){
			int realX = (x - curXMin) * 128 / (curXMax - curXMin);
			int realY = 32 + (curYMax - y) * 128 / (curYMax - curYMin);
			ST7735_DrawPixel(realX, realY, ST7735_BLUE);
		}
	}
}

void ST7735_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
	int xLength = x2 - x1;
	int yLength = y2 - y1;
  int longerLength = max(abs(xLength), abs(yLength));
	int x;
	int y;
	for(int i = 0; i < longerLength; i ++){
		x = x1 + i * xLength / longerLength;
		y = (160 - (y1 + i * yLength / longerLength));
		ST7735_DrawPixel(x, y, ST7735_BLUE);
	}
}

//void ST7735_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
//	int dy,dx,m,i;
//	uint16_t y,x,y_end,x_end;
//	if (abs(y1-y2)<abs(x1-x2)) {
//		if (x2>x1) {
//			dx = x2-x1;
//			dy = y2-y1;
//			x = x1;
//			x_end = x2;
//			y = y1;
//			y_end = y2;
//			i = 1;
//			if (dy<0) {
//				dy = -dy;
//				i = -1;
//			}
//			m = 2*dy-dx;
//			while (x<x_end) {
//				ST7735_DrawPixel(x,y,color);
//				if (m>0) {
//					x = x + i;
//					m = m -2*dx;
//				}
//				m = m + 2*dy;
//				x += 1;
//			}
//		}
//		else {
//			dx = x1-x2;
//			dy = y1-y2;
//			x = x2;
//			x_end = x1;
//			y = y2;
//			y_end = y1;
//			i = 1;
//			if (dy<0) {
//				dy = -dy;
//				i = -1;
//			}
//			m = 2*dy-dx;
//			while (x<x_end) {
//				ST7735_DrawPixel(x,y,color);
//				if (m>0) {
//					x = x + i;
//					m = m -2*dx;
//				}
//				m = m + 2*dy;
//				x += 1;
//			}
//		}
//	}
//	else {
//		if (y2>y1) {
//			dx = x2-x1;
//			dy = y2-y1;
//			x = x1;
//			x_end = x2;
//			y = y1;
//			y_end = y2;
//			i = 1;
//			if (dx<0) {
//				dx = -dx;
//				i = -1;
//			}
//			m = 2*dx-dy;
//			while (y<y_end) {
//				ST7735_DrawPixel(x,y,color);
//				if (m>0) {
//					x = x + i;
//					m = m -2*dy;
//				}
//				m = m + 2*dx;
//				y += 1;
//			}
//		}
//		else {
//			dx = x1-x2;
//			dy = y1-y2;
//			x = x2;
//			x_end = x1;
//			y = y2;
//			y_end = y1;
//			i = 1;
//			if (dx<0) {
//				dx = -dx;
//				i = -1;
//			}
//			m = 2*dx-dy;
//			while (x<x_end) {
//				ST7735_DrawPixel(x,y,color);
//				if (m>0) {
//					x = x + i;
//					m = m - 2*dy;
//				}
//				m = m + 2*dx;
//				y += 1;
//			}
//		}
//	}
//}
