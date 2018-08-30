/*
 * usart_console.c
 *
 * Created: 8/29/2018 9:09:11 PM
 *  Author: cellis
 */ 

#include "usart_console.h"

void console_init()
{
	//initialize a configuration struct with USART settings
	static usart_serial_options_t usart_config = {
		.baudrate	=	USART_SERIAL_BAUDRATE,
		.charlength =	USART_SERIAL_CHAR_LEN,
		.paritytype =	USART_SERIAL_PARITY,
		.stopbits	=	USART_SERIAL_STOP_BIT
	};
	
	PORTC.DIR |= USART_TX_PIN;	//set the USART transmit pin to output
	
	stdio_serial_init(USART_SERIAL, &usart_config); //function maps the serial output to printf, not nessecary to know how it works
	
	sysclk_enable_peripheral_clock(USART_SERIAL);	//enable the USART's clock
	delay_us(1); //apparently need to slow down before print @32MHz
}