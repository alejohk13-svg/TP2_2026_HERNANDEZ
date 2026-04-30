#include <teclado.h>

static char tecla = 0;

void teclado_init(void) {
    tecla = 0;
}

void teclado_update(void) {

}

char teclado_getKey(void) {
    return tecla;
}
