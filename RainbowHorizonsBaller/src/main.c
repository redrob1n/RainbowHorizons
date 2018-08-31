/*
* 
*/

#include <asf.h>
#include "usart_console.h"
#include "drivers/pressure/MS56XX.h"
#include "drivers/pressure/SPI.h"
#include "drivers/spectrometer/spectrometer.h"

#define DEBUG

#define PRESSURE_SELECT_PIN		IOPORT_CREATE_PIN(PORTC, 4)

MS56XX_t pressure_sensor;
uint8_t x1, x2;

static void system_initialize(void);

int main (void)
{	
	sysclk_init();
	board_init();

	system_initialize();
	
	uint8_t counter = 0;
	for (;;)
	{
		test_spec();
		if(!image_done && counter++ > 40){
			spectrometer_reset();		
		}
		
		if (image_done)		
		{
			delay_ms(100);
			readMS56XX(&pressure_sensor);
			printf("%li,", pressure_sensor.data.pressure);
			spectrometer_reset();
			
			for (uint16_t i = 0; i < 2048; i++)
			{
				printf("%u,", image[i]);
			}
			printf("\n");
		}
		wdt_reset();
		delay_ms(50);
	}
		
}

static void system_initialize(void)
{
	wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_8KCLK);
	wdt_enable();
	
	console_init(); //initialize usart for console
	
	//Only check for watchdog WARNING: only wdt reset cause will be reset
	if (reset_cause_is_watchdog())
	{
		printf("WARNING: watchdog timer caused the cpu to be reset\r\n");
		reset_cause_clear_causes(RESET_CAUSE_WDT);
	}
	
	printf("Console USART initialized...\r\n");
	printf("System initialized...\r\n");
	
	pressure_sensor = define_new_MS56XX_default_OSR(MS5607, &SPIC, PRESSURE_SELECT_PIN);
	
	initializespi(&SPIC, &PORTC);
	enable_select_pin(pressure_sensor.select_pin);
	
#ifdef DEBUG
	//TEST DEBUG TAKE THIS OUT
	initializespi(&SPIE, &PORTE);
	
	#define EMBED_CS IOPORT_CREATE_PIN(PORTE, 4)
	enable_select_pin(EMBED_CS);
	spideselect(EMBED_CS);
	PORTE.DIRSET = 0b10;
	PORTE.OUTCLR = 0b10;
	delay_ms(100);
	PORTE.OUTSET = 0b10;
	delay_us(5);
	PORTE.OUTCLR = 0b10;
	delay_ms(100);
	while (1)
	{
		spiselect(EMBED_CS);
		spiwrite(&SPIE, 0x04 << 2);
		
		x1 = spiread(&SPIE);
		x2 = spiread(&SPIE);
		spideselect(EMBED_CS);
		delay_ms(50);
	}
#endif
	
	//END TEST DEBUG
	
	//Pressure sensor initialization routine, also reads calibration data from sensor
	calibratePressureSensor(&pressure_sensor);
	
	spectrometer_init();
	
	PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
	
	
	sei(); //set enable interrupts
	
	printf("Global interrupts enabled...\r\n");
	
	printf("Application begin...\r\n");
}