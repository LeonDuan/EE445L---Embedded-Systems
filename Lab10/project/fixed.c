#include <stdint.h>
#include <math.h>
#include "ST7735.h"
#include "fixed.h"

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
