#ifndef PSX_H_
#define PSX_H_

/**
 * \defgroup sbertling_psx PSX
 * \code #include "psx.h" \endcode
 * \brief Eine Bibliothek um mit dem AVR einen Playstation Controller anzusteuern
 * 
 * \code
 * DATA -|---__________--__|--
 *          0 1 2 3 4 5 6 7
 * CLK  -|-_-_-_-_-_-_-_-_-|--
 *        0 1 2 3 4 5 6 7
 * CMD  -|__--________--__-|--
 * \endcode
 * 
 * \author Simon Bertling
 */

/*@{*/

#include "global.h"

#include <avr/io.h>
#include <util/delay.h>

// Port defines 
#define PSX_PORT	PORTB	//!< Port an dem das Pad angeschlossen wird
#define PSX_DDR		DDRB	//!< DDR von Port, an dem das Pad angeschlossen ist
#define PSX_PIN		PINB	//!< PIN register von dem Port, an dem das Pad angeschlossen ist

#define CLK_PIN		PB2		//!< CLK Pin
#define ATT_PIN		PB3		//!< ATT Pin
#define CMD_PIN		PB4		//!< CMD Pin
#define ACK_PIN		PB1		//!< ACK Pin
#define DATA_PIN	PB5		//!< DATA Pin

// Zeitkonstanten
#define TIME_CLK	5	//!< Zeit in µs, die CLK auf einem Pegel bleibt
#define DELAY_TIME	5

#define MAX_BYTES_BUTTON_PRESSURE	12
#define MAX_BYTES					21

// Makros
#define WAIT_FOR_ACK	while((PSX_PIN & (1<<ACK_PIN))) //!< Wartet bis ACK auf LOW ist

// Befehlskonstanten
#define CMD_START			0x01	//!< Startkommando, der Controller sendet daraufhin seine Daten
#define CMD_REQUEST_DATA	0x42	//!< Dieses Kommando wird direkt nach CMD_START gesendet
#define CMD_SET_CONFIG		0x43	//!< Der Controller startet/endet den Config Modus
#define CMD_CONFIG_ON		0x01
#define CMD_CONFIG_OFF		0x00
#define CMD_CONFIG_EXIT		0x5A
#define CMD_SWITCH_MODE		0x44	//!< Wechselt zwischen Analog und Digital Modus. Controller muss in Config Modus sein
#define CMD_SET_ANALOG		0x01
#define CMD_SET_DIGITAL		0x00
#define CMD_LOCK_ON			0x03
#define CMD_LOCK_OFF		0x00
#define CMD_MORE_INFO		0x45	//!< Fragt mehr Informationen ab. Controller muss in Config Modus sein
#define CMD_PRESSURE_MODE	0x4F	//!< Setzt den Druck Modus. Controller muss in Config Modus sein
#define CMD_PRESSURE_1		0xFF
#define CMD_PRESSURE_2		0xFF
#define CMD_PRESSURE_3		0x03
#define CMD_PRESSURE_ON		0xFF
#define CMD_PRESSURE_OFF	0x00
#define CMD_IDLE			0x00	//!< Ruhelage

// Datenkonstanten
#define DATA_REQUEST_RESPONSE	0x5A	//!< Der Controller sendet dies, nachdem CMD_REQUEST_DATA empfangen wurde. Ende des Headers.
#define DATA_CONFIG_ON			0xF3	//!< Headerinformation. Controller sendet dies, wenn der Config Modus an ist.
#define DATA_DIGITAL			0x41	//!< Standard Digital Pad
#define DATA_NEGCON				0x23	//!< NegCon Controller
#define DATA_ANALOG				0x73	//!< Analog Controller mit roter LED an
#define DATA_PSX_MOUSE			0x12	//!< PSX Maus

// Controllerkonstanten

#define PAD_DIGITAL		0x40
#define PAD_ANALOG		0x70
#define PAD_CONFIG		0xF0

// Standard Defs
#define BUTTON_SLCT		0x0100	//!< Select Taste
#define BUTTON_JOYL		0x0200	//!< L3
#define BUTTON_JOYR		0x0400	//!< R3
#define BUTTON_START	0x0800	//!< Start Taste
#define BUTTON_UP		0x1000	//!< Steuerkreuz oben
#define BUTTON_RIGHT	0x2000	//!< Steuerkreuz rechts
#define BUTTON_DOWN		0x4000	//!< Steuerkreuz unten
#define BUTTON_LEFT		0x8000	//!< Steuerkreuz links

#define BUTTON_L2			0x01	//!< L1 Taste
#define BUTTON_R2			0x02	//!< R1 Taste
#define BUTTON_L1			0x04	//!< L2 Taste
#define BUTTON_R1			0x08	//!< R2 Taste
#define BUTTON_TRIANGLE		0x10	//!< Dreieck Taste
#define BUTTON_CIRCLE		0x20	//!< O Taste
#define BUTTON_CROSS		0x40	//!< X Taste
#define BUTTON_SQUARE		0x80	//!< Quadrat Taste

#define STICK_RIGHT_X		0x01	//!< Rechter Analogstick in X Richtung
#define STICK_RIGHT_Y		0x02	//!< Rechter Analogstick in Y Richtung
#define STICK_LEFT_X		0x04	//!< Linker Analogstick in X Richtung
#define STICK_LEFT_Y		0x08	//!< Linker Analogstick in Y Richtung

#define BUTTON_PRESSURE_R			0
#define BUTTON_PRESSURE_L			1
#define BUTTON_PRESSURE_U			2
#define BUTTON_PRESSURE_D			3
#define BUTTON_PRESSURE_TRIANGLE	4
#define BUTTON_PRESSURE_CIRCLE		5
#define BUTTON_PRESSURE_CROSS		6
#define BUTTON_PRESSURE_SQUARE		7
#define BUTTON_PRESSURE_L1			8
#define BUTTON_PRESSURE_R1			9
#define BUTTON_PRESSURE_L2			10
#define BUTTON_PRESSURE_R2			11

// NegCon, Maus, etc implementation später

/**
 * \brief Initialisiert die Ports des AVR
 */
void PSX_init(void);

/**
 * \brief Liest den Header aus
 * \param[in] cmd Zu sendender Befehl
 * \returns Controller Status
 */
uint8_t PSX_get_header(uint8_t cmd);

/**
 * \brief Versetzt den Controller in den Config Mode
 * \param[in] state CMD_CONFIG_MODE_ON oder CMD_CONFIG_MODE_OFF
 */
void PSX_set_config_mode(uint8_t state);

/**
 * \brief Setzt die Eingabe Methode (Analog oder Digital)
 * \param[in] mode Modus, Analog oder Digital
 * \param[in] lock Lock Modus an oder aus
 */
void PSX_set_input_mode(uint8_t mode, uint8_t lock);

/**
 * \brief Configuriert den Controller Druckdaten der Tasten zu senden
 * \param[in] state An oder Aus
 */
void PSX_set_pressure_mode(uint8_t state);

/**
 * \brief Fragt den aktuellen Status des Controllers ab
 * \return 0: Fehler, 1: Erfolg
 */
uint8_t PSX_get_button_state(void);

/**
 * \brief Empfängt das Datenbyte des Controllers während cmd auf dem CMD Pin gesendet wird
 * \param[in] cmd Zu sendender Befehl
 * \return Datenbyte des Controllers
 */
uint8_t PSX_receive_data(uint8_t cmd);

/**
 * \brief Überprüft, ob eine bestimmte Taste gedrückt ist
 * \param[in] button Taste, kann auch mit ODER verknüpft werden
 * \returns 0: Taste nicht gedrück, sonst1: Taste gedrückt
 */
uint8_t PSX_button_pressed(uint16_t button);

/**
 * \brief Gibt den Wert des als Parameter übergebenen Analogsticks zurück
 * \param[in] stick zB STICK_LEFT_X
 * \returns Analog Wert des Sticks
 */
uint8_t PSX_get_analog_value(uint8_t stick);

/**
 * \brief Gibt den aktuellen Druckwert einer Taste zurück
 * \param[in] button Taste, die abgefragt werden soll
 * \returns aktueller Druckwert einer Taste
 */
uint8_t PSX_get_button_pressure(uint8_t button);

/*@}*/

#endif /*PSX_H_*/
