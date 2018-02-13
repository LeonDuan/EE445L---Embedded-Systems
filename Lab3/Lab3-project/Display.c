#include <stdint.h>
#include <stdio.h>
#include "ST7735.h"
#include <stdlib.h>

int curXMin, curXMax, curYMin, curYMax;

const int32_t CircleXbuf[180] = {2000, 1999, 1995, 1989, 1981, 1970, 1956, 1941, 1923, 1902, 1879, 1854, 1827, 1798, 1766, 1732, 1696, 1658, 1618, 1576, 1532, 1486, 1439, 1389, 1338, 1286, 1231, 1176, 1118, 1060, 1000, 939, 877, 813, 749, 684, 618, 551, 484, 416, 347, 278, 209, 140, 70, 0, -70, -140, -209, -278, -347, -416, -484, -551, -618, -684, -749, -813, -877, -939, -1000, -1060, -1118, -1176, -1231, -1286, -1338, -1389, -1439, -1486, -1532, -1576, -1618, -1658, -1696, -1732, -1766, -1798, -1827, -1854, -1879, -1902, -1923, -1941, -1956, -1970, -1981, -1989, -1995, -1999, -2000, -1999, -1995, -1989, -1981, -1970, -1956, -1941, -1923, -1902, -1879, -1854, -1827, -1798, -1766, -1732, -1696, -1658, -1618, -1576, -1532, -1486, -1439, -1389, -1338, -1286, -1231, -1176, -1118, -1060, -1000, -939, -877, -813, -749, -684, -618, -551, -484, -416, -347, -278, -209, -140, -70, 0, 70, 140, 209, 278, 347, 416, 484, 551, 618, 684, 749, 813, 877, 939, 1000, 1060, 1118, 1176, 1231, 1286, 1338, 1389, 1439, 1486, 1532, 1576, 1618, 1658, 1696, 1732, 1766, 1798, 1827, 1854, 1879, 1902, 1923, 1941, 1956, 1970, 1981, 1989, 1995, 1999
};
const int32_t CircleYbuf[180] = {0, 70, 140, 209, 278, 347, 416, 484, 551, 618, 684, 749, 813, 877, 939, 1000, 1060, 1118, 1176, 1231, 1286, 1338, 1389, 1439, 1486, 1532, 1576, 1618, 1658, 1696, 1732, 1766, 1798, 1827, 1854, 1879, 1902, 1923, 1941, 1956, 1970, 1981, 1989, 1995, 1999, 2000, 1999, 1995, 1989, 1981, 1970, 1956, 1941, 1923, 1902, 1879, 1854, 1827, 1798, 1766, 1732, 1696, 1658, 1618, 1576, 1532, 1486, 1439, 1389, 1338, 1286, 1231, 1176, 1118, 1060, 1000, 939, 877, 813, 749, 684, 618, 551, 484, 416, 347, 278, 209, 140, 70, 0, -70, -140, -209, -278, -347, -416, -484, -551, -618, -684, -749, -813, -877, -939, -1000, -1060, -1118, -1176, -1231, -1286, -1338, -1389, -1439, -1486, -1532, -1576, -1618, -1658, -1696, -1732, -1766, -1798, -1827, -1854, -1879, -1902, -1923, -1941, -1956, -1970, -1981, -1989, -1995, -1999, -2000, -1999, -1995, -1989, -1981, -1970, -1956, -1941, -1923, -1902, -1879, -1854, -1827, -1798, -1766, -1732, -1696, -1658, -1618, -1576, -1532, -1486, -1439, -1389, -1338, -1286, -1231, -1176, -1118, -1060, -1000, -939, -877, -813, -749, -684, -618, -551, -484, -416, -347, -278, -209, -140, -70
};
const int32_t clockNumsX[12] = {62, 37, 19, 13, 19, 38, 63, 88, 106, 113, 106, 88};
const int32_t clockNumsY[12] = {45, 51, 70, 95, 120, 138, 145, 138, 120, 95, 70, 51};

int max(int a, int b){
	if(a > b) return a;
	else return b;
}

void initClock(char *title, int32_t minX, int32_t maxX, int32_t minY, int32_t maxY){
	ST7735_FillScreen(0);
	ST7735_PlotClear(minY, maxY);
	ST7735_SetCursor(0,0);
	ST7735_OutString(title);
	curXMin = minX;
	curXMax = maxX;
	curYMin = minY;
	curYMax = maxY;
}

void drawClock(uint32_t num, int32_t bufX[], int32_t bufY[]){
	int counter = 0;
	for(int i = 0; i < num; i++){
		counter++;

		int x = bufX[i] * 7 / 6;
		int y = bufY[i] * 7 / 6;
		if(x <= curXMax && x >= curXMin && y <= curYMax && y >= curYMin){
			int realX = (x - curXMin) * 128 / (curXMax - curXMin);
			int realY = 32 + (curYMax - y) * 128 / (curYMax - curYMin);
			ST7735_DrawPixel(realX, realY, ST7735_BLUE);
			ST7735_DrawPixel(realX+1, realY, ST7735_BLUE);
			ST7735_DrawPixel(realX, realY+1, ST7735_BLUE);
			ST7735_DrawPixel(realX+1, realY+1, ST7735_BLUE);
		}
	}
	for(int i = 11; i >= 0; i--){
		int toPrint = 12 - i;
		if(toPrint >= 10){
			ST7735_DrawCharS(clockNumsX[i]-2, clockNumsY[i] - 1, '1', 0, ST7735_Color565(228,228,228), 1);
			ST7735_DrawCharS(clockNumsX[i]+2, clockNumsY[i] - 1, toPrint % 10 + '0', 0, ST7735_Color565(228,228,228), 1);

		}
		else{
			ST7735_DrawCharS(clockNumsX[i]-2, clockNumsY[i] - 1, toPrint + '0', 0, ST7735_Color565(228,228,228), 1);
		}	
	}
}

void draw_ClockHand(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
	int xLength = x1 - x0;
	int yLength = y1 - y0;
  int longerLength = max(abs(xLength), abs(yLength));
	int x;
	int y;
	for(int i = 0; i < longerLength; i ++){
		x = x0 + i * xLength / longerLength;
		y = (y0 + i * yLength / longerLength);
		ST7735_DrawPixel(x, y, color);
	}
}

void draw_DigitalClock(uint16_t hour, uint16_t minute) {
	char hour_1 = (hour/10) + '0';
	char hour_2 = hour%10 + '0';
	char colon = ':';
	char min_1 = (minute/10) + '0';
	char min_2 = minute%10 + '0';

	ST7735_DrawChar(5,80,hour_1,ST7735_BLACK,ST7735_Color565(228,228,228),4);
	ST7735_DrawChar(32,80,hour_2,ST7735_BLACK,ST7735_Color565(228,228,228),4);
	ST7735_DrawChar(52,80,colon,ST7735_BLACK,ST7735_Color565(228,228,228),4);
	ST7735_DrawChar(76,80,min_1,ST7735_BLACK,ST7735_Color565(228,228,228),4);
	ST7735_DrawChar(100,80,min_2,ST7735_BLACK,ST7735_Color565(228,228,228),4);
}

void draw_AnalogClock(void) {
	initClock("Alarm Clock",-2500, 2500, -2500, 2500);
	drawClock(180,(int32_t *)CircleXbuf,(int32_t *)CircleYbuf);
}


void init_LCD(void){
	ST7735_InitR(INITR_REDTAB);
  ST7735_PlotClear(0,127);
}

void clear_Screen(char * message){
	ST7735_PlotClear(0,127);
	ST7735_OutString(message);
}
