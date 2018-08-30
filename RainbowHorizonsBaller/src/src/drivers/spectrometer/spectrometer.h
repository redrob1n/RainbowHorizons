/*
 * spectrometer.h
 *
 * Created: 8/29/2018 9:40:57 PM
 *  Author: Sean
 */ 

#include <asf.h>

#ifndef SPECTROMETER_H_
#define SPECTROMETER_H_

void spectrometer_init(void);
void spectrometer_reset(void);
void spectrometer_read(void);

struct spi_device spec_spi_conf;

uint16_t pixel_arr[2048];
uint8_t pixel[2];
uint16_t pixel_count = 0;

int image_done = false;


#endif /* SPECTROMETER_H_ */