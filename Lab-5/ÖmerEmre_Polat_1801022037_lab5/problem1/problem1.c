#include "bsp.h"
#include "stdlib.h"

int main(void) {

	BSP_system_init();
	BSP_UART_init(9600);

    while(1)
    {
    	uart_tx(uart_rx());
    }

    return 0;
}
