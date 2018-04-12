#include <stdint.h>
#include "util.h"
#include "ST7735.h"
#include "calib.h"

uint32_t temp[SAMPLE_LENGTH] = {0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128, 136, 144, 152, 160, 168, 176, 184, 192, 200, 208, 216, 224, 232, 240, 248, 256, 264, 272, 280, 288, 296, 304, 312, 320, 328, 336, 344, 352, 360, 368, 376, 384, 392, 400};
uint32_t ADC_values[SAMPLE_LENGTH] = {3191, 3113, 3035, 2958, 2881, 2805, 2729, 2654, 2579, 2505, 2432, 2359, 2287, 2215, 2145, 2074, 2005, 1936, 1867, 1800, 1733, 1666, 1601, 1535, 1471, 1407, 1344, 1281, 1220, 1158, 1098, 1038, 979, 920, 862, 805, 748, 692, 636, 582, 527, 474, 421, 368, 317, 265, 215, 165, 116, 67, 19};

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
void drawNextTemp(uint32_t temp2Dec){
	if (curX == 0) ST7735_PlotClear(curYMin, curYMax);
	int realX = (curX - curXMin) * 128 / (curXMax - curXMin);
	int realY = 32 + (curYMax - temp2Dec) * 128 / (curYMax - curYMin);
	ST7735_DrawPixel(realX, realY, ST7735_BLUE);
	curX = (curX + 1)%128;
}
	
uint32_t ADC_to_Fahrenheit2Dec(uint32_t ADC_val) {
	if (ADC_val > 3191 || ADC_val < 19) return ADC_VAL_OUT_OF_RANGE_ERROR;
	for (int i = 0; i < SAMPLE_LENGTH - 1; i++) {
		if (ADC_val == ADC_values[i]) return temp[i];
		else if (ADC_val < ADC_values[i] && ADC_val > ADC_values[i + 1]) {
			uint32_t ADC_range = ADC_values[i] - ADC_values[i + 1];
			uint32_t ADC_point = ADC_values[i] - ADC_val;
			uint32_t temp_range = temp[i + 1] - temp[i];
			
			uint32_t toAdd = temp_range * ADC_point/ADC_range;
			uint32_t result = temp[i] + toAdd;
			return result * 10;
		}
	}
	return ADC_VAL_OUT_OF_RANGE_ERROR;
}

void calibrate(void) {
	for (int i = 0; i < SAMPLE_LENGTH; i++) { // skip 0
		
		int curTemp = temp[i];
		volatile int calibrate_k = CALIBRATE_K_TIMES_100;
		volatile int calibrate_b = CALIBRATE_B_TIMES_100;
		temp[i] = (curTemp * CALIBRATE_K_TIMES_100) / 100 + CALIBRATE_B_TIMES_100 / 10;
	}
}
