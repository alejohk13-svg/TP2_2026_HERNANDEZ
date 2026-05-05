#include "stm32f4xx.h"
#include "teclado.h"

#define LED_PORT GPIOD
#define LED_PIN  GPIO_Pin_10

// Delay base de 1ms

void delay(volatile int t) {
    for (volatile int i = 0; i < t * 10000; i++);
}

// VARIABLES LED

typedef enum {
    LED_IDLE,
    LED_ON,
    LED_OFF
} led_estado_t;

led_estado_t led_estado = LED_IDLE;

int tiempo_base = 50;
int contador = 0;
int parpadeos_restantes = 0;

// FUNCIONES

int convertir_tecla(char k)
{
    if (k >= '1' && k <= '9') return (k - '0') + 1;
    if (k == '0') return 1;
    return 0;
}

void actualizar_tiempo(char k)
{
    switch(k)
    {
        case 'A': tiempo_base = 50; break;
        case 'B': tiempo_base = 90; break;
        case 'C': tiempo_base = 110; break;
        case 'D': tiempo_base = 220; break;
    }
}

// Se llama cuando llega una tecla

void led_setTecla(char k)
{
    int veces = convertir_tecla(k);

    if (veces > 0)
    {
        parpadeos_restantes = veces * 2;
        led_estado = LED_ON;
        contador = 0;
    }

    actualizar_tiempo(k);
}

// FSM del LED
void led_update(void)
{
    switch(led_estado)
    {
        case LED_IDLE:
            break;

        case LED_ON:
            GPIO_SetBits(LED_PORT, LED_PIN);
            contador++;

            if (contador >= tiempo_base) {
                contador = 0;
                led_estado = LED_OFF;
            }
            break;

        case LED_OFF:
            GPIO_ResetBits(LED_PORT, LED_PIN);
            contador++;

            if (contador >= tiempo_base) {
                contador = 0;
                parpadeos_restantes--;

                if (parpadeos_restantes <= 0) {
                    led_estado = LED_IDLE;
                } else {
                    led_estado = LED_ON;
                }
            }
            break;
    }
}

// MAIN

int main(void)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Pin = LED_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_Init(LED_PORT, &GPIO_InitStruct);

    teclado_init();

    while (1)
    {
        teclado_update();
        led_update();

        char k = teclado_getKey();

        if (k != 0)
        {
            led_setTecla(k);
        }

        delay(1);
    }
}
