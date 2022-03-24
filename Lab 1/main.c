#include "stm32l476xx.h"

void Init(){
    // Enable HSI
    RCC->CR |= ((uint32_t)RCC_CR_HSION);
    while ( (RCC->CR & (uint32_t) RCC_CR_HSIRDY) == 0 );

    // Select HSI as system clock source
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI;
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) == 0 );

    // Enable GPIO Clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
		RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    // Initialize Green LED
    GPIOA->MODER &= ~GPIO_MODER_MODE5_1;

		// Initialize User Button
		GPIOC->MODER &= ~GPIO_MODER_MODE13;
}

int main(void){
    // Initialization
    Init();
		uint16_t trigger = 1;
		uint32_t time = 0;
		uint32_t timeout = 100000;
		// Polling to Check for User Button Presses
		while(1) {
			time++;
			if (!(GPIOC->IDR & GPIO_IDR_ID13)) {
				if (timeout < 100000) {
					//Nothing
				} else {
					if (trigger == 1) {
						trigger = 0;
						timeout = 0;
					} else {
						trigger = 1;
						timeout = 0;
					}
				}
			}
			if (trigger == 1){
				if (time > 100000) {
					GPIOA->ODR &= ~GPIO_ODR_OD5;
				} else {
					GPIOA->ODR |= GPIO_ODR_OD5;
				}
			} else {
				GPIOA->ODR |= GPIO_ODR_OD5;
			}
			if (time >= 200000) {
				time = 0;
			}
			timeout++;
		}
}
