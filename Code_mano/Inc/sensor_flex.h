#ifndef SENSOR_FLEX_H_
#define SENSOR_FLEX_H_
#ifndef STM32F401xE
#define STM32F401xE
#endif
#include "stm32f4xx.h"
#include <stdint.h>

void ADC1_PA0_Init(void);
uint16_t ADC1_Read(void);
uint8_t CalcularPorcentajeFlexion(uint16_t valorADC);
#endif
