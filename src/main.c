/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include <teclado.h>
#include "stm32f4xx.h"
			

int main(void)
{
    teclado_init();

    while (1)
    {
        teclado_update();

        char t = teclado_getKey();

        if (t != 0) {

        }
    }
}
