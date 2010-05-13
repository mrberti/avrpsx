#ifndef UART_H_
#define UART_H_

/**
 * \brief Einfache routinen um auf dem UART zu senden
 * 		  Bisher nur für ATmega8 getestet
 * \author	Simon Bertling
 */

#include "global.h"
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>

#define BAUD 115200UL	//!< Baudrate

#define UBRR_VAL ((F_CPU+BAUD*8)/(BAUD*16)-1)   //!< clever runden
#define BAUD_REAL (F_CPU/(16*(UBRR_VAL+1)))     //!< Reale Baudrate
#define BAUD_ERROR ((BAUD_REAL*1000)/BAUD) 		//!< Fehler in Promille, 1000 = kein Fehler.
 
#if ((BAUD_ERROR<990) || (BAUD_ERROR>1010))
  #error Systematischer Fehler der Baudrate grösser 1% und damit zu hoch! 
#endif

/**
 * \brief Initialisiert den UART
 * \param Keine Parameter
 * \returns Kein Rückgabewert
 */
void initUART();

/**
 * \brief Deaktiviert den UART
 * \param Keine Parameter
 * \returns Kein Rückgabewert
 */
void UART_disable();

/**
 * \brief Aktiviert den UART
 * \param Keine Parameter
 * \returns Kein Rückgabewert
 */
void UART_enable();

/**
 * \brief Gibt ein Zeichen auf dem UART aus
 * \param unsigned char c: Das zu sendende Zeichen
 * \returns Kein Rückgabewert
 */
void UART_putc(char c);

/**
 * \brief Gibt einen String auf dem UART aus
 * \param unsigned char *s: Pointer auf zu sendenden String
 * \returns Kein Rückgabewert
 */
void UART_puts(char *s);

/**
 * \brief Gibt einen 16 bit vorzeichenbehafteten Integerwert auf dem UART aus
 * \param uint16_t d: Wert der zu sendenden Zahl
 * \returns Kein Rückgabewert
 */
void UART_putd(int16_t d);

void UART_puth(int16_t d);


#endif /*UART_H_*/
