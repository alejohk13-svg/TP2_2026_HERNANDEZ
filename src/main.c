#include "stm32f4xx.h"
#include "teclado.h"

#define LED_PORT GPIOD
#define LED_PIN  GPIO_Pin_10

void delay(volatile int t) {
    while(t--);
}

// Base de tiempo (modificable)
int tiempo_base = 3000000;

// Convierte tecla a cantidad de parpadeos
int convertir_tecla(char k)
{
    if (k >= '1' && k <= '9') return (k - '0') + 1;
    if (k == '0') return 1;

    return 0;
}

// Cambia velocidad con teclas A B C D
void actualizar_tiempo(char k)
{
    switch(k)
    {
        case 'A': tiempo_base = 500000; break;   // ~50ms
        case 'B': tiempo_base = 900000; break;   // ~90ms
        case 'C': tiempo_base = 1100000; break;  // ~110ms
        case 'D': tiempo_base = 2200000; break;  // ~220ms
    }
}

void parpadear(int veces)
{
    for (int i = 0; i < veces; i++) {
        GPIO_SetBits(LED_PORT, LED_PIN);
        delay(tiempo_base);

        GPIO_ResetBits(LED_PORT, LED_PIN);
        delay(tiempo_base);
    }
}

int main(void)
{
    // Clock LED
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    // Clock GPIO teclado
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // LED salida
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

        char k = teclado_getKey();

        if (k != 0)
        {
            // Si es letra → cambia velocidad
            actualizar_tiempo(k);

            // Si es número → parpadea
            int veces = convertir_tecla(k);
            if (veces > 0) {
                parpadear(veces);
            }

            // Espera liberación
            while(teclado_getKey() != 0) {
                teclado_update();
            }
        }
    }
}
