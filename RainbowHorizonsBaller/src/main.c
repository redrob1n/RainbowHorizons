/*
* 
*/

#include <asf.h>
#include "usart_console.h"

static void system_initialize(void);

int main (void)
{	
	sysclk_init();
	board_init();

	system_initialize();

	for (;;)
	{
		printf("Hello\r\n");
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
	
	printf("Console USART initialized...\r\n");
	printf("System initialized...\r\n");
	
	PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;
	
	
	sei(); //set enable interrupts
	
	printf("Global interrupts enabled...\r\n");
	
	printf("Application begin...\r\n");
}