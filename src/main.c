#include "stm32f4xx.h"
#include "teclado.h"

#define LED_PORT GPIOD
#define LED_PIN  GPIO_Pin_10

// Delay ajustado
void delay(volatile int t) {
    for (volatile int i = 0; i < t * 10000; i++);
}

// Base de tiempo
int tiempo_base = 50;

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
        case 'A': tiempo_base = 50; break;
        case 'B': tiempo_base = 90; break;
        case 'C': tiempo_base = 110; break;
        case 'D': tiempo_base = 220; break;
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
    GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;
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
            actualizar_tiempo(k);

            int veces = convertir_tecla(k);
            if (veces > 0) {
                parpadear(veces);
            }
            while(teclado_getKey() != 0) {
                teclado_update();
            }
        }
    }
}
