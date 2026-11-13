#include "motor.h"

// Variables para medición de posición
volatile float angulo_total = 0.0f;

// Variables para el lazo de Control PID
volatile float referencia = 0.0f;
volatile float e = 0.0f;
volatile float e_1 = 0.0f;
volatile float int_e = 0.0f;
volatile float der_e = 0.0f;

// Parámetros PID (Basados en tu modelo)
volatile float Kp = 10.0f;
volatile float Ti = 9999.0f;       // Inicia inactivo (número grande)
volatile float Td = 0.02f;
volatile float T = 0.005f;         // 5ms (200Hz) del TIM10
volatile int32_t u = 0;

void Motor_Init(void) {
    // 1. Habilitar relojes (GPIOA, GPIOB, TIM1, TIM3, TIM10)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN | RCC_APB2ENR_TIM10EN;

    // 2. Configurar Encoder por Hardware: PA6 y PA7 (TIM3)
    GPIOA->MODER &= ~((3U << (6 * 2)) | (3U << (7 * 2)));
    GPIOA->MODER |=  ((2U << (6 * 2)) | (2U << (7 * 2)));
    GPIOA->AFR[0] &= ~((15U << (6 * 4)) | (15U << (7 * 4)));
    GPIOA->AFR[0] |=  ((2U << (6 * 4)) | (2U << (7 * 4)));
    GPIOA->PUPDR &= ~((3U << (6 * 2)) | (3U << (7 * 2)));
    GPIOA->PUPDR |=  ((1U << (6 * 2)) | (1U << (7 * 2)));

    TIM3->CCMR1 &= ~(TIM_CCMR1_CC1S | TIM_CCMR1_CC2S);
    TIM3->CCMR1 |= (1U << TIM_CCMR1_CC1S_Pos) | (1U << TIM_CCMR1_CC2S_Pos);
    TIM3->SMCR &= ~TIM_SMCR_SMS;
    TIM3->SMCR |= (3U << TIM_SMCR_SMS_Pos);
    TIM3->CR1 |= TIM_CR1_CEN;

    // 3. Configurar Motor PWM: PA8 (TIM1_CH1)
    GPIOA->MODER &= ~(3U << (8 * 2));
    GPIOA->MODER |=  (2U << (8 * 2));
    GPIOA->AFR[1] &= ~(15U << ((8 - 8) * 4));
    GPIOA->AFR[1] |=  (1U << ((8 - 8) * 4));

    // 4. Configurar Pines de Dirección (PB12, PB13) y STBY (PB5)
    GPIOB->MODER &= ~((3U << (12 * 2)) | (3U << (13 * 2)) | (3U << (5 * 2)));
    GPIOB->MODER |=  ((1U << (12 * 2)) | (1U << (13 * 2)) | (1U << (5 * 2)));

    GPIOB->BSRR = (1U << 5); // STBY HIGH
    GPIOB->BSRR = (1U << (12 + 16)) | (1U << (13 + 16)); // Detenido

    TIM1->PSC = 0;
    TIM1->ARR = 999;
    TIM1->CCR1 = 0;
    TIM1->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM1->CCMR1 |= (6U << TIM_CCMR1_OC1M_Pos) | TIM_CCMR1_OC1PE;
    TIM1->CCER |= TIM_CCER_CC1E;
    TIM1->BDTR |= TIM_BDTR_MOE;
    TIM1->CR1 |= TIM_CR1_CEN;

    // 5. Lazo de Control (TIM10 a 200 Hz / 5ms)
    TIM10->PSC = 16000 - 1;
    TIM10->ARR = 5 - 1;
    TIM10->DIER |= TIM_DIER_UIE;
    TIM10->CR1 |= TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
}

float Motor_GetCurrentAngle(void) {
    int16_t conteo = (int16_t)TIM3->CNT; // Usando el hardware en lugar de EXTI
    return ((float)conteo / PULSOS_POR_VUELTA) * 360.0f;
}

void Motor_SetTargetAngle(float angle) {
    referencia = angle;
}

// Interrupción del Lazo de Control (TIM10) - Integrando tu lógica PID
void TIM1_UP_TIM10_IRQHandler(void) {
    if (TIM10->SR & TIM_SR_UIF) {
        TIM10->SR &= ~TIM_SR_UIF;

        angulo_total = Motor_GetCurrentAngle();
        e = referencia - angulo_total;

        // Zona muerta
        if (e > -2.0f && e < 2.0f) {
            e = 0.0f;
        }

        // Término Integral con Anti-Windup
        int_e = int_e + (T * e);
        if (int_e > 50000.0f) int_e = 50000.0f;
        if (int_e < -50000.0f) int_e = -50000.0f;

        // Término Derivativo
        der_e = (e - e_1) / T;

        // Acción Integral
        float accion_I = 0.0f;
        if (Ti > 0.0001f) {
            accion_I = (Kp / Ti) * int_e;
        }

        // Ecuación PID
        u = (int32_t)((Kp * e) + accion_I + (Kp * Td * der_e));
        e_1 = e;

        // Saturación al rango del TIM1 (0 a 1000)
        if (u > 1000) u = 1000;
        if (u < -1000) u = -1000;

        // Aplicar al hardware (TB6612FNG)
        if (u > 0) {
            if (u < 150) u = 150; // Fricción
            GPIOB->BSRR = (1U << 12) | (1U << (13 + 16)); // Horario
            TIM1->CCR1 = (uint32_t)u;
        }
        else if (u < 0) {
            if (u > -150) u = -150; // Fricción
            GPIOB->BSRR = (1U << (12 + 16)) | (1U << 13); // Antihorario
            TIM1->CCR1 = (uint32_t)(-u);
        }
        else {
            GPIOB->BSRR = (1U << 12) | (1U << 13); // Freno corto
            TIM1->CCR1 = 0;
        }
    }
}
