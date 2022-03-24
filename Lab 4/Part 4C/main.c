#include "stm32l476xx.h"
#include "LED.h"
#include "SPI.h"
#include "SysClock.h"
#include "SysTimer.h"

#include <stdio.h>

#define SPI_MASTER SPI2
#define SPI_SLAVE SPI3

uint8_t transmit = 0;
uint8_t receive = 0;

// PB14 to PB15

int main(void){
	System_Clock_Init();   // System Clock = 80 MHz
	SysTick_Init();
	
	LED_Init();
	
	SPI2_GPIO_Init();
	SPI2_Init();
	
	while(1) {
		for (transmit = 0; transmit <= 9; transmit++) {
			SPI_Transfer_Byte(SPI2, transmit, &receive);
			// delay between SPI transfers to observe LED behavior
			if (transmit == receive) {
				Green_LED_Toggle();
				delay(1000);
			}
		}
	}
}
