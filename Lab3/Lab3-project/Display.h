// Display.h
// Display module for Lab 3 i.e Alarm Clock
// Tahir Haideri & Tianyun Duan
// EE 445L

#ifndef Display
#define Display

#define CLOCK_ORIGIN 63,95

struct clock_hand {
	uint32_t x;
	uint32_t y;
};

struct clock_hand Hour_Hand[12] = {
	{63,65},{73,75},{83,85},{93,95},{83,105},{73,115},{63,125},{53,115},{43,105},{33,95},{43,85},{53,75}
};
struct clock_hand Minute_Hand[60] = {
	{63,45},{68,46},{73,47},{78,48},{83,49},{88,52},{92,56},{96,60},{99,64},{102,67},{104,72},{106,77},{108,81},{109,85},{110,90},{110,95},{110,101},
	{109,106},{108,111},{106,116},{104,121},{102,126},{99,129},{96,132},{92,136},{88,140},{83,143},{78,144},{73,145},{68,146},{63,147},{58,146},{53,145},{48,144},
	{43,143},{38,140},{34,136},{30,132},{27,129},{24,126},{22,121},{20,116},{18,111},{17,106},{16,101},{16,95},{16,90},{17,85},{18,81},{20,77},{22,72},{24,67},
	{27,64},{30,60},{34,56},{38,52},{43,49},{48,48},{53,47},{58,46}
};

void draw_AnalogClock(void);
void draw_ClockHand(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void draw_DigitalClock(uint16_t hour, uint16_t minute);
void init_LCD(void);
void clear_Screen(char * message);

#endif
