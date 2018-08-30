/*
 * spectrometer.c
 *
 * Created: 8/29/2018 9:40:50 PM
 *  Author: Sean
 */ 

#include <util/atomic.h>
#include "spectrometer.h"

#define SPEC_PORT PORTE
#define FIFO_CS PIN0_bm
#define XRST PIN1_bm
#define FIFO_RST PIN2_bm
#define PIXEL_RDY PIN3_bm
#define SPI_CS PIN4_bm
#define MISO PIN5_bm
#define MOSI PIN6_bm
#define SPI_CLK PIN7_bm

/* PIXEL INTERRUPT */
ISR(PORTE_INT0_vect){
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
		spectrometer_read(); //	reads in the 16-bit pixel
		if(++pixel_count >= 2048){ // once we've read an entire image, flip the flag
			image_done = true;
		}
	}
}

/* Set up PIXEL_RDY interrupt and SPI things */
void spectrometer_init(void){
	/* Enable interrupt things */
	PORTE.PIN3CTRL = PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;
	PORTE.INT0MASK = PIXEL_RDY;
	PORTE.INTCTRL = PORT_INT0LVL_LO_gc | PORT_INT1LVL_LO_gc; //enable porte interrupts low level for interrupt0
	
	PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_LOLVLEN_bm; //Enable high level interrupts.
	
	/* Initialize SPI things */
	spec_spi_conf.id =  IOPORT_CREATE_PIN(PORTE, 3);
	
	ioport_configure_port_pin(&PORTE, FIFO_CS, IOPORT_INIT_HIGH | IOPORT_DIR_OUTPUT);
	ioport_configure_port_pin(&PORTE, SPI_CS, IOPORT_PULL_UP | IOPORT_DIR_INPUT);
	
	ioport_configure_port_pin(&PORTE, MOSI, IOPORT_INIT_HIGH| IOPORT_DIR_OUTPUT);
	ioport_configure_port_pin(&PORTE, MISO, IOPORT_DIR_INPUT);
	ioport_configure_port_pin(&PORTE, SPI_CLK, IOPORT_INIT_HIGH| IOPORT_DIR_OUTPUT);
	
	ioport_configure_port_pin(&PORTE, FIFO_RST, IOPORT_INIT_LOW | IOPORT_DIR_OUTPUT);
	
	spi_master_init(&SPIE);
	spi_master_setup_device(&SPIE, &spec_spi_conf, SPI_MODE_0, 1000000, 0);
	spi_enable(&SPIE);
	
	/* X_RESET strobe */
	delay_ms(100);
	ioport_configure_port_pin(&PORTE, XRST, IOPORT_INIT_LOW | IOPORT_DIR_OUTPUT); // Must wait 100ms, strobe low-high-low, then wait 100ms
	ioport_set_pin_low(IOPORT_CREATE_PIN(PORTE, XRST));
	ioport_set_pin_high(IOPORT_CREATE_PIN(PORTE, XRST));
	delay_ms(100);
	
	/* Configure the Spectrometer settings */
	uint8_t buffer[3] = {0x19, 0x3, 0xE8}; //integration time of 1s
	spi_select_device(&SPIE, &spec_spi_conf);
	spi_write_packet(&SPIE, buffer, 3);
	spi_deselect_device(&SPIE, &spec_spi_conf);
}

/* If FIFO_RST is brought high, the spectrometer will reset, allowing for a new image to be taken. */
void spectrometer_reset(void){
	ioport_set_pin_high(IOPORT_CREATE_PIN(PORTE, FIFO_RST));
	delay_ms(1);
	ioport_set_pin_low(IOPORT_CREATE_PIN(PORTE, FIFO_RST));
}

/* Bring FIFO_CS low, read in the 2 byte pixel, then put FIFO_CS back to high. */
void spectrometer_read(void){
	ioport_set_pin_low(IOPORT_CREATE_PIN(PORTE, FIFO_CS));
	
	spi_select_device(&SPIE, &spec_spi_conf);
	spi_read_packet(&SPIE, pixel, 2);
	spi_deselect_device(&SPIE, &spec_spi_conf);
	
	ioport_set_pin_high(IOPORT_CREATE_PIN(PORTE, FIFO_CS));
}