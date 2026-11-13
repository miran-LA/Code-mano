/*
 * bluetooth.h
 *
 *  Created on: Nov 11, 2026
 *      Author: lamir
 */
#ifndef BLUETOOTH_H
#define BLUETOOTH_H

// Declarar el microcontrolador objetivo antes de incluir la librería
#ifndef STM32F401xE
#define STM32F401xE
#endif

#include "stm32f4xx.h"
#include <stdint.h>
#include <string.h>

#define BT_BUFFER_SIZE 64

// Inicializa el puerto serie USART2 a 9600 baudios
void Bluetooth_Init(void);

// Funciones para enviar datos al celular
void Bluetooth_SendChar(char c);
void Bluetooth_SendString(const char* str);
void Bluetooth_SendNum(uint32_t num);

// Funciones para procesar comandos recibidos
uint8_t Bluetooth_HasCommand(void);
void Bluetooth_GetCommand(char* out_buffer);

#endif /* BLUETOOTH_H */
