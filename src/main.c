#include "stm32f4xx.h"
#include "teclado.h"

// ASIGNACION PARA LED
#define LED_PORT GPIOD
#define LED_PIN  GPIO_Pin_10

void delay(volatile int t) {
    while(t--);
}

int main(void)
{
    // CLOCKS

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // CONFIGURACION DE LED

    GPIO_InitTypeDef GPIO_InitStruct;

    GPIO_InitStruct.GPIO_Pin = LED_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

    GPIO_Init(LED_PORT, &GPIO_InitStruct);

    // TECLADO INIT
    teclado_init();

    char tecla;

    while (1)
    {
        teclado_update();
        tecla = teclado_getKey();

        if (tecla != 0) {
            GPIO_SetBits(LED_PORT, LED_PIN);
        } else {
            GPIO_ResetBits(LED_PORT, LED_PIN);
        }

        delay(100000);
    }
}
