/*
 * usart_console.h
 *
 * Created: 8/29/2018 9:08:58 PM
 *  Author: cellis
 */ 


#ifndef USART_CONSOLE_H_
#define USART_CONSOLE_H_


#include <asf.h>

#define USART_SERIAL			&USARTD1
#define USART_SERIAL_BAUDRATE	9600
#define USART_SERIAL_CHAR_LEN	USART_CHSIZE_8BIT_gc
#define USART_SERIAL_PARITY		USART_PMODE_DISABLED_gc
#define USART_SERIAL_STOP_BIT	true

#define USART_TX_PIN			PIN7_bm

void console_init(void);


#endif /* USART_CONSOLE_H_ */