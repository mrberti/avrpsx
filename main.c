#include "global.h"
#include "psx.h"
#include "uart.h"

#include <avr/io.h>
#include <util/delay.h>

int main()
{
	initUART();
	UART_enable();
	UART_puts("\r\nSTARTING...\r\n");
	PSX_init();
	_delay_ms(1000);
	UART_puts("\r\nENTERING MAIN...\r\n");
	while(1)
	{
		_delay_ms(50);
		PSX_get_button_state();
		UART_puth(PSX_get_button_pressure(BUTTON_PRESSURE_R2));	
		UART_puts(" ");	
		if(PSX_button_pressed(BUTTON_SQUARE))
		{
			PSX_set_pressure_mode(CMD_PRESSURE_OFF);
		}
		if(PSX_button_pressed(BUTTON_TRIANGLE))
		{
			PSX_set_pressure_mode(CMD_PRESSURE_ON);
		}
		if(PSX_button_pressed(BUTTON_R1))
			PSX_set_input_mode(CMD_SET_ANALOG, CMD_LOCK_OFF);
		if(PSX_button_pressed(BUTTON_L1))
			PSX_set_input_mode(CMD_SET_DIGITAL, CMD_LOCK_OFF);
	}
	return 0;
}
