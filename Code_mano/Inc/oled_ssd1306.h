#ifndef OLED_SSD1306_H
#define OLED_SSD1306_H
#ifndef STM32F401xE
#define STM32F401xE
#endif
#include "stm32f4xx.h"
#include <stdint.h>


#define OLED_ADDR 0x78

void OLED_Init(void);
void OLED_Clear(void);
void OLED_SetCursor(uint8_t page, uint8_t col);
void OLED_PutChar(char ch, uint8_t page, uint8_t col);
void OLED_PrintStr(const char *str, uint8_t page, uint8_t col);
void OLED_PrintNum(uint32_t num, uint8_t page, uint8_t col);

#endif
