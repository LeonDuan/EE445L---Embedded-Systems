#include <stdint.h>
void Timer0A_Init(void(*task)(void), uint32_t period);
void Timer0A_Handler(void);
void Timer1A_Init(void(*task)(void), uint32_t period);
void Timer1A_Handler(void);
