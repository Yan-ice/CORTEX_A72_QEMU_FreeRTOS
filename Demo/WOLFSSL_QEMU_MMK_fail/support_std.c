#include "FreeRTOS.h"

void _std_putchar(char c){
	uartputc_sync(c);
}

void _std_enter_critical(){
	portENTER_CRITICAL();
}

void _std_exit_critical(){
	portEXIT_CRITICAL();
}
