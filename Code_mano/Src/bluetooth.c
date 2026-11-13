	/*
 * bluetooth.c
 *
 *  Created on: Nov 11, 2026
 *      Author: lamir
 */
#include "bluetooth.h"

// Búfer circular / de línea
static char rx_buffer[BT_BUFFER_SIZE];
static volatile uint8_t rx_index = 0;
static volatile uint8_t command_ready = 0;

void Bluetooth_Init(void) {
    // 1. Habilitar reloj para GPIOA y USART2
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // 2. Configurar PA2 (TX) y PA3 (RX) en Función Alternativa (AF7)
    GPIOA->MODER &= ~((3U << (2 * 2)) | (3U << (3 * 2)));
    GPIOA->MODER |=  ((2U << (2 * 2)) | (2U << (3 * 2))); // Modo AF

    GPIOA->AFR[0] &= ~((15U << (2 * 4)) | (15U << (3 * 4)));
    GPIOA->AFR[0] |=  ((7U  << (2 * 4)) | (7U  << (3 * 4))); // AF7 para USART2

    // 3. Configurar USART2 a 9600 baudios (Asumiendo reloj base de 16 MHz)
    // BRR = Fck / (16 * BaudRate) -> 16000000 / 9600 = 1666.6 -> 0x0682 o 0x0683
    USART2->BRR = 16000000 / 38400;

    // 4. Habilitar USART, Transmisor (TE), Receptor (RE) e Interrupción de RX (RXNEIE)
    USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;

    // 5. Habilitar interrupción en el NVIC
    NVIC_EnableIRQ(USART2_IRQn);
}

// Envía un solo carácter
void Bluetooth_SendChar(char c) {
    while (!(USART2->SR & USART_SR_TXE)); // Esperar a que el registro esté libre
    USART2->DR = c;
}

// Envía una cadena de texto
void Bluetooth_SendString(const char* str) {
    while (*str) {
        Bluetooth_SendChar(*str++);
    }
}

// Envía un número (útil para telemetría)
void Bluetooth_SendNum(uint32_t num) {
    char buf[12];
    int i = 0;
    if (num == 0) buf[i++] = '0';
    while (num > 0) {
        buf[i++] = (num % 10) + '0';
        num /= 10;
    }
    // Imprimir invertido
    for (int j = i - 1; j >= 0; j--) {
        Bluetooth_SendChar(buf[j]);
    }
}

// Verifica si llegó un comando completo
uint8_t Bluetooth_HasCommand(void) {
    return command_ready;
}

// Copia el comando recibido y limpia la bandera
void Bluetooth_GetCommand(char* out_buffer) {
    strcpy(out_buffer, rx_buffer);
    command_ready = 0;
}

// --- Rutina de Interrupción (Se ejecuta sola cada que llega un byte) ---
void USART2_IRQHandler(void) {
    if (USART2->SR & USART_SR_RXNE) {
        char data = (char)USART2->DR; // Leer el dato limpia la bandera RXNE

        // Si llega un salto de línea (Enter), terminamos de recibir el comando
        if (data == '\n' || data == '\r') {
            if (rx_index > 0) {
                rx_buffer[rx_index] = '\0'; // Cerrar la cadena tipo C
                command_ready = 1;          // Avisar al bucle principal
                rx_index = 0;               // Reiniciar índice
            }
        }
        // Si no es salto de línea y hay espacio, guardar el carácter
        else if (rx_index < (BT_BUFFER_SIZE - 1) && !command_ready) {
            rx_buffer[rx_index++] = data;
        }
    }
}

