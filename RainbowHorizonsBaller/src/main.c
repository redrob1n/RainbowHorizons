/*
* 
*/

#include <asf.h>
#include "usart_console.h"
#include "drivers/pressure/MS56XX.h"
#include "drivers/pressure/SPI.h"
#include "drivers/spectrometer/spectrometer.h"

#define PRESSURE_SELECT_PIN		IOPORT_CREATE_PIN(PORTC, 4)

MS56XX_t pressure_sensor;
static void system_initialize(void);

int main (void)
{	
	sysclk_init();
	board_init();

	system_initialize();

	for (;;)
	{
		if (image_done)		
		{
			spectrometer_reset();
			readMS56XX(&pressure_sensor);
			printf("%li,", pressure_sensor.data.pressure);
			for (uint16_t i = 0; i < 2048; i++)
			{
				printf("%u,", image[i]);
			}
			printf("\n");
		}
		wdt_reset();		
	}
		
}

static void system_initialize(void)
{
	wdt_set_timeout_period(WDT_TIMEOUT_PERIOD_8CLK);
	wdt_enable();
	
	console_init(); //initialize usart for console
	
	//Only check for watchdog WARNING: only wdt reset cause will be reset
	if (reset_cause_is_watchdog())
	{
		printf("WARNING: watchdog timer caused the cpu to be reset\r\n");
		reset_cause_clear_causes(RESET_CAUSE_WDT);
	}
	
	//printf("Console USART initialized...\r\n");
	//printf("System initialized...\r\n");
	
	//spectrometer_init();
	pressure_sensor = define_new_MS56XX_default_OSR(MS5607, &SPIC, PRESSURE_SELECT_PIN);
	
	initializespi(&SPIC, &PORTC);
	enable_select_pin(pressure_sensor.select_pin);
	
	//Pressure sensor initialization routine, also reads calibration data from sensor
	calibratePressureSensor(&pressure_sensor);
	
	PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
	
	
	sei(); //set enable interrupts
	
	//printf("Global interrupts enabled...\r\n");
	
	printf("Application begin...\r\n");
}