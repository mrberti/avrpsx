#include "uart.h"
#include "global.h"
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdlib.h>

void initUART()
{
	// setze Nutzdatenlänge auf 8 bit
	UCSRC |= (1<<URSEL)|(3<<UCSZ0);
		
	// setze Baud Rate
	UBRRH = UBRR_VAL >> 8;
	UBRRL = UBRR_VAL & 0xFF;
}

void UART_disable()
{
	UCSRB &= ~(1<<TXEN);
}

void UART_enable()
{
	UCSRB |= (1<<TXEN);
}

void UART_putc(char c)
{
	// Warte auf Sendebereitschaft
	while(!(UCSRA & (1<<UDRE)));
	UDR = c;
}

void UART_puts(char *s)
{
	while(*s)
	{
		UART_putc(*s);
		s++;
	}
}

void UART_putd(int16_t d)
{
	char s[7];
	itoa(d, s, 10);
	UART_puts(s);
}

void UART_puth(int16_t d)
{
	char s[7];
	itoa(d, s, 16);
	UART_puts(s);
}
