#ifndef MOTOR_H
#define MOTOR_H

#ifndef STM32F401xE
#define STM32F401xE
#endif

#include "stm32f4xx.h"
#include <stdint.h>

// Valor temporal hasta calibrar el motor físicamente
#define PULSOS_POR_VUELTA 1980.0f

void Motor_Init(void);
void Motor_SetTargetAngle(float angle);
float Motor_GetCurrentAngle(void); // Útil para ver la posición real en Live Expressions

void Motor_Stop(void);
void Motor_Brake(void);
void Motor_Standby(void);

#endif
