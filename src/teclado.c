#include "teclado.h"
#include "stm32f4xx_gpio.h"

// FILAS
#define FILA1_PORT GPIOE
#define FILA1_PIN  GPIO_Pin_8

#define FILA2_PORT GPIOE
#define FILA2_PIN  GPIO_Pin_9

#define FILA3_PORT GPIOE
#define FILA3_PIN  GPIO_Pin_10

#define FILA4_PORT GPIOE
#define FILA4_PIN  GPIO_Pin_11

// COLUMNAS
#define COL1_PORT GPIOC
#define COL1_PIN  GPIO_Pin_0

#define COL2_PORT GPIOC
#define COL2_PIN  GPIO_Pin_3

#define COL3_PORT GPIOC
#define COL3_PIN  GPIO_Pin_2

#define COL4_PORT GPIOA
#define COL4_PIN  GPIO_Pin_0

char mapa[4][4] = {
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

static char tecla = 0;

// MAQUINA DE ESTADOS

typedef enum {
    ESTADO_INICIAL,
    ESTADO_DETECCION,
    ESTADO_DEBOUNCE,
    ESTADO_CONFIRMACION,
    ESTADO_ESPERA_LIBERACION
} estado_t;

static estado_t estado = ESTADO_INICIAL;
static char tecla_detectada = 0;
static int contador = 0;

#define DEBOUNCE_DELAY 50000

void teclado_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // FILAS
    GPIO_InitStruct.GPIO_Pin = FILA1_PIN | FILA2_PIN | FILA3_PIN | FILA4_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOE, &GPIO_InitStruct);

    // COLUMNAS
    GPIO_InitStruct.GPIO_Pin = COL1_PIN | COL2_PIN | COL3_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = COL4_PIN;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    tecla = 0;
}

void filas_high(void) {
    GPIO_SetBits(FILA1_PORT, FILA1_PIN);
    GPIO_SetBits(FILA2_PORT, FILA2_PIN);
    GPIO_SetBits(FILA3_PORT, FILA3_PIN);
    GPIO_SetBits(FILA4_PORT, FILA4_PIN);
}

void activar_fila(int f) {
    filas_high();

    switch(f) {
        case 0: GPIO_ResetBits(FILA1_PORT, FILA1_PIN); break;
        case 1: GPIO_ResetBits(FILA2_PORT, FILA2_PIN); break;
        case 2: GPIO_ResetBits(FILA3_PORT, FILA3_PIN); break;
        case 3: GPIO_ResetBits(FILA4_PORT, FILA4_PIN); break;
    }
}

int leer_columna(void) {
    if (GPIO_ReadInputDataBit(COL1_PORT, COL1_PIN) == 0) return 0;
    if (GPIO_ReadInputDataBit(COL2_PORT, COL2_PIN) == 0) return 1;
    if (GPIO_ReadInputDataBit(COL3_PORT, COL3_PIN) == 0) return 2;
    if (GPIO_ReadInputDataBit(COL4_PORT, COL4_PIN) == 0) return 3;

    return -1;
}

static char escanear_teclado(void)
{
    int col;

    for (int i = 0; i < 4; i++) {
        activar_fila(i);

        col = leer_columna();

        if (col != -1) {
            return mapa[i][col];
        }
    }

    return 0;
}

void teclado_update(void)
{
    char t = escanear_teclado();

    switch(estado)
    {
        case ESTADO_INICIAL:
            tecla = 0;
            if (t != 0) {
                tecla_detectada = t;
                estado = ESTADO_DETECCION;
            }
            break;

        case ESTADO_DETECCION:
            contador = 0;
            estado = ESTADO_DEBOUNCE;
            break;

        case ESTADO_DEBOUNCE:
            contador++;
            if (contador > DEBOUNCE_DELAY) {
                estado = ESTADO_CONFIRMACION;
            }
            break;

        case ESTADO_CONFIRMACION:
            if (escanear_teclado() == tecla_detectada) {
                tecla = tecla_detectada;
                estado = ESTADO_ESPERA_LIBERACION;
            } else {
                estado = ESTADO_INICIAL;
            }
            break;

        case ESTADO_ESPERA_LIBERACION:
            if (escanear_teclado() == 0) {
                estado = ESTADO_INICIAL;
            }
            break;
    }
}

char teclado_getKey(void) {
    return tecla;
}
