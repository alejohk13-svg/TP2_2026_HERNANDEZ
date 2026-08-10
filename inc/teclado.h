#ifndef TECLADO_H
#define TECLADO_H

#include "stm32f4xx.h"

void teclado_init(void);
void teclado_update(void);
char teclado_getKey(void);

#endif
