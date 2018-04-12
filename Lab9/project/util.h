#define	SAMPLE_LENGTH	51
#define ADC_VAL_OUT_OF_RANGE_ERROR 0xFFFFFFFF

uint32_t ADC_to_Fahrenheit2Dec(uint32_t);
void drawNextTemp(uint32_t);
void ST7735_XYplotInit(int32_t minX, int32_t maxX, int32_t minY, int32_t maxY);
void calibrate(void);
