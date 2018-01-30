#include <stdint.h>
#include <math.h>
#include "ST7735.h"
#include "fixed.h"

int curXMin, curXMax, curYMin, curYMax;

#define scaleFactorX 128 / (curXMax-curXMin)
#define scaleFactorY 128 / (curYMax-curYMin)

void ST7735_sDecOut2(int32_t n){
	// check N's range
	if(n < -9999){
		ST7735_OutString("-**.**");
		return;
	}
	if(n > 9999){
		ST7735_OutString(" **.**");
		return;	
	}
	
	// determine sign
	int positiveOrZero = (n >= 0);
	char sign = ' ';
	if (!positiveOrZero) {
		n = -n;
		sign = '-';
	}
	char tens = (n / 1000) + '0';
	char ones = ((n / 100) % 10) + '0';
	char tenths = (n / 10) % 10 + '0';
	char hundredths = n % 10 + '0';

	char toPrint[7];
	toPrint[6] = '\0';
	if (tens == '0') {
		toPrint[0] = ' ';
		toPrint[1] = sign;
	}
	else{
		toPrint[0] = sign;
		toPrint[1] = tens;
	}
	toPrint[2] = ones;
	toPrint[3] = '.';
	toPrint[4] = tenths;
	toPrint[5] = hundredths;
	
	ST7735_OutString(toPrint);
}

void ST7735_uBinOut6(uint32_t n){
	// check N's range
	if (n > 63999) {
		ST7735_OutString("***.**");
		return;
	}

	// solve for integral part
	int integer = 0;
	int mask = 64;
	int mtpl = 1;
	for (int i = 0; i < 10; i++) {
		integer += mtpl * ((mask & n) != 0);
		mtpl *= 2;
		mask <<= 1;
	}
	char hundreds = (integer / 100) % 10 + '0';
	char tens = (integer / 10) % 10 + '0';
	char ones = integer % 10 + '0';
	
	int frac = 0;
	mask = 1;
	mtpl = 1;
	for (int i = 0; i < 6; i++) {
		frac += mtpl * ((mask & n) != 0);
		mtpl *= 2;
		mask <<= 1;
	}
	if (frac != 0) {
		frac = frac * 100 / 64;
	}
	char tenths = (frac / 10) % 10 + '0';
	char hundredths = frac % 10 + '0';
	
	char toPrint[7];
	toPrint[6] = '\0';
	if (hundreds == '0'){
		toPrint[0] = ' ';
		if(tens == '0'){
			toPrint[1] = ' ';
		}
		else{
			toPrint[1] = tens;
		}
	}
	else{
		toPrint[0] = hundreds;
		toPrint[1] = tens;
	}
	toPrint[2] = ones;
	toPrint[3] = '.';
	toPrint[4] = tenths;
	toPrint[5] = hundredths;
	
	ST7735_OutString(toPrint);
	return;
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
