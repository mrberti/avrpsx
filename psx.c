#include "psx.h"

// GLOBALE VARIABLE ///////////////////////////////////////////////////////

uint8_t pad_state = 0;
uint16_t button_state = 0;
uint8_t stick_state_right_x = 0;
uint8_t stick_state_right_y = 0;
uint8_t stick_state_left_x = 0;
uint8_t stick_state_left_y = 0;

uint8_t button_states[MAX_BYTES];
uint8_t button_pressure[MAX_BYTES_BUTTON_PRESSURE];


// IMPLEMENTATION //////////////////////////////////////////////////////////

void PSX_init()
{
	// PSX_DDR, PSX_PORT und PSX_PIN löschen
	PSX_DDR = 0;
	PSX_PORT = 0;
	PSX_PIN = 0;
	// ATT, CLOCK und CMD als Ausgang setzen
	PSX_DDR = (1<<ATT_PIN) | (1<<CLK_PIN) | (1<<CMD_PIN);
	// Pullups aktivieren
	PSX_PORT = (1<<DATA_PIN) | (1<<ACK_PIN);
	// ATT initial auf HIGH setzen
	PSX_PORT |= (1<<ATT_PIN);
	// CLK initial auf HIGH setzen
	PSX_PORT |= (1<<CLK_PIN);
	
	for(uint8_t i = 0; i < MAX_BYTES; i++)
		button_states[i] = 0;
	for(uint8_t i = 0; i < MAX_BYTES_BUTTON_PRESSURE; i++)
		button_pressure[i] = 0;
}

uint8_t PSX_get_header(uint8_t cmd)
{
	uint8_t data = 0;
	
	PSX_receive_data(CMD_START);
	WAIT_FOR_ACK;
	data = PSX_receive_data(cmd);
	WAIT_FOR_ACK;
	PSX_receive_data(CMD_IDLE);
	WAIT_FOR_ACK;
	
	pad_state = data;
	
	return data;
}

void PSX_set_config_mode(uint8_t state)
{
	_delay_ms(DELAY_TIME);
	// ATT auf LOW setzen
	PSX_PORT &= ~(1<<ATT_PIN);
	
	PSX_get_header(CMD_SET_CONFIG);
	
	PSX_receive_data(state);
	WAIT_FOR_ACK;
	if(state == CMD_CONFIG_OFF)
	{
		PSX_receive_data(CMD_CONFIG_EXIT);
		WAIT_FOR_ACK;
		PSX_receive_data(CMD_CONFIG_EXIT);
		WAIT_FOR_ACK;
		PSX_receive_data(CMD_CONFIG_EXIT);
		WAIT_FOR_ACK;
		PSX_receive_data(CMD_CONFIG_EXIT);
		WAIT_FOR_ACK;
		PSX_receive_data(CMD_CONFIG_EXIT);
	}
	else
	{
		PSX_receive_data(CMD_IDLE);
	}
	
	// ATT wieder auf High setzen, Ende der Übertragung
	_delay_us(TIME_CLK);
	PSX_PORT |= (1<<ATT_PIN);
}

void PSX_set_input_mode(uint8_t mode, uint8_t lock)
{
	PSX_set_config_mode(CMD_CONFIG_ON);
	_delay_ms(DELAY_TIME);
	// ATT auf LOW setzen
	PSX_PORT &= ~(1<<ATT_PIN);
	
	PSX_get_header(CMD_SWITCH_MODE);
	if(pad_state == DATA_CONFIG_ON)
	{
		PSX_receive_data(mode);
		WAIT_FOR_ACK;
		PSX_receive_data(lock);
		WAIT_FOR_ACK;
		PSX_receive_data(CMD_IDLE);
		WAIT_FOR_ACK;
		PSX_receive_data(CMD_IDLE);
		WAIT_FOR_ACK;
		PSX_receive_data(CMD_IDLE);
		WAIT_FOR_ACK;
		PSX_receive_data(CMD_IDLE);
	}
		
	// ATT wieder auf High setzen, Ende der Übertragung
	_delay_us(TIME_CLK);
	PSX_PORT |= (1<<ATT_PIN);
	PSX_set_config_mode(CMD_CONFIG_OFF);
}

void PSX_set_pressure_mode(uint8_t state)
{
	PSX_set_config_mode(CMD_CONFIG_ON);
	_delay_ms(DELAY_TIME);
	// ATT auf LOW setzen
	PSX_PORT &= ~(1<<ATT_PIN);
	
	PSX_get_header(CMD_PRESSURE_MODE);
	if(pad_state == DATA_CONFIG_ON)
	{
		PSX_receive_data(CMD_PRESSURE_1 & state);//(state && CMD_PRESSURE_1);
		WAIT_FOR_ACK;
		PSX_receive_data(CMD_PRESSURE_2 & state);//(state && CMD_PRESSURE_2);
		WAIT_FOR_ACK;
		PSX_receive_data(CMD_PRESSURE_3 & state);//(state && CMD_PRESSURE_3);
		WAIT_FOR_ACK;
		PSX_receive_data(CMD_IDLE);
		WAIT_FOR_ACK;
		PSX_receive_data(CMD_IDLE);
		WAIT_FOR_ACK;
		PSX_receive_data(CMD_IDLE);
	}

	// ATT wieder auf High setzen, Ende der Übertragung
	_delay_us(TIME_CLK);
	PSX_PORT |= (1<<ATT_PIN);
	PSX_set_config_mode(CMD_CONFIG_OFF);
}

uint8_t PSX_get_button_state()
{
	// ATT auf LOW setzen
	PSX_PORT &= ~(1<<ATT_PIN);

	PSX_get_header(CMD_REQUEST_DATA);
	
	if(pad_state == DATA_CONFIG_ON) // im Config Modus können keine Tasten abgefragt werden
	{
		// ATT wieder auf High setzen, Ende der Übertragung
		_delay_us(TIME_CLK);
		PSX_PORT |= (1<<ATT_PIN);
		return 0;
	}
	
	uint8_t cycles = 2*(pad_state & 0x0F);
	for(uint8_t i = 0; i < cycles; i++)
	{
		button_states[i] = PSX_receive_data(CMD_IDLE);
		if(i != cycles - 1)
			WAIT_FOR_ACK;	// letztes Byte bekommt kein ACK mehr
	}

	// ATT wieder auf High setzen, Ende der Übertragung
	_delay_us(TIME_CLK);
	PSX_PORT |= (1<<ATT_PIN);
	
	button_state = button_states[0]<<8;
	button_state += button_states[1];
	stick_state_right_x = button_states[2];
	stick_state_right_y = button_states[3];
	stick_state_left_x = button_states[4];
	stick_state_left_y = button_states[5];
	for(uint8_t i = 0; i < MAX_BYTES_BUTTON_PRESSURE; i++)
		button_pressure[i] = button_states[6+i];

	return 1;
}

uint8_t PSX_receive_data(uint8_t cmd)
{
	uint8_t data = 0;
	for (uint8_t n = 0; n < 8; n++) {
		// CMD setzen, CLK ist auf HIGH
		if(cmd & (1<<n))
			PSX_PORT |= (1<<CMD_PIN);
		else
			PSX_PORT &= ~(1<<CMD_PIN);
		_delay_us(TIME_CLK);
		// CLK auf LOW ziehen
		PSX_PORT &= ~(1<<CLK_PIN);
		_delay_us(TIME_CLK);
		// DATA auslesen
		if( PSX_PIN & (1<<DATA_PIN) )
			data |= (1<<n);
		// CLK wieder auf HIGH setzen
		PSX_PORT |= (1<<CLK_PIN);
	}
	return data;
}

uint8_t PSX_button_pressed(uint16_t button)
{
	return !(button_state & button);
}

uint8_t PSX_get_analog_value(uint8_t stick)
{
	if( (pad_state & 0xF0) == PAD_ANALOG)
	{
		switch (stick) {
			case STICK_LEFT_X:
				return stick_state_left_x;
			case STICK_LEFT_Y:
				return stick_state_left_y;
			case STICK_RIGHT_X:
				return stick_state_right_x;
			case STICK_RIGHT_Y:
				return stick_state_right_y;
			default:
				return 0x7F; // 0xFF / 2
				break;
		}
	}
	else 
		return 0x7F;
}

uint8_t PSX_get_button_pressure(uint8_t button)
{
	return button_pressure[button];
}
