#include "sensor_flex.h"

uint16_t adc_reposo = 900;
uint16_t adc_flexionado = 350;

void ADC1_PA0_Init(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    GPIOA->MODER |= (3U << (0 * 2));
    ADC1->CR2 = 0;
    ADC1->SQR3 = 0;
    ADC1->SQR1 = 0;
    ADC1->CR2 |= ADC_CR2_ADON;
}
uint16_t ADC1_Read(void) {
    ADC1->CR2 |= ADC_CR2_SWSTART;
    while ((ADC1->SR & ADC_SR_EOC) == 0);
    return (uint16_t)ADC1->DR;
}
uint8_t CalcularPorcentajeFlexion(uint16_t valorADC) {
    if (valorADC >= adc_reposo) return 0;
    if (valorADC <= adc_flexionado) return 100;

    uint32_t Porcentaje = ((uint32_t)(adc_reposo - valorADC) * 100) / (adc_reposo - adc_flexionado);
    return (uint8_t)Porcentaje;
}

