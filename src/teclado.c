#include "teclado.h"
#include "stm32f4xx_gpio.h"

// ESTRUCTURA DE PIN

typedef struct {
    GPIO_TypeDef* puerto;
    uint16_t pin;
} pin_t;

// DEFINICION DE PINES

pin_t filas[4] = {
    {GPIOE, GPIO_Pin_8},
    {GPIOE, GPIO_Pin_9},
    {GPIOE, GPIO_Pin_10},
    {GPIOE, GPIO_Pin_11}
};

pin_t columnas[4] = {
    {GPIOC, GPIO_Pin_0},
    {GPIOC, GPIO_Pin_3},
    {GPIOC, GPIO_Pin_2},
    {GPIOA, GPIO_Pin_0}
};

// MAPA DEL TECLADO

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

#define DEBOUNCE_DELAY 20

// INIT

void teclado_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    // FILAS (salidas)
    GPIO_InitStruct.GPIO_Pin =
        filas[0].pin | filas[1].pin | filas[2].pin | filas[3].pin;

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_Init(filas[0].puerto, &GPIO_InitStruct);

    // COLUMNAS (entradas)
    GPIO_InitStruct.GPIO_Pin =
        columnas[0].pin | columnas[1].pin | columnas[2].pin;

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;

    GPIO_Init(columnas[0].puerto, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = columnas[3].pin;
    GPIO_Init(columnas[3].puerto, &GPIO_InitStruct);

    tecla = 0;
}


void filas_high(void) {
    for (int i = 0; i < 4; i++) {
        GPIO_SetBits(filas[i].puerto, filas[i].pin);
    }
}

void activar_fila(int f) {
    filas_high();
    GPIO_ResetBits(filas[f].puerto, filas[f].pin);
}

int leer_columna(void) {
    for (int i = 0; i < 4; i++) {
        if (GPIO_ReadInputDataBit(columnas[i].puerto, columnas[i].pin) == 0) {
            return i;
        }
    }
    return -1;
}

// ESCANEO

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
