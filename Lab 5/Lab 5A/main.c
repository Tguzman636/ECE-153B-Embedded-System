#include "ADC.h"
#include "PWM.h"
#include "SysClock.h"
#include "stm32l476xx.h"

#include <stdio.h>

int data;
volatile int pmw_data;
volatile float voltage;

void LED_Init() {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	GPIOA->MODER &= ~GPIO_MODER_MODE5_1;
	
}

int main(void) {
    // Initialization
    System_Clock_Init(); // Switch System Clock = 16 MHz

    ADC_Init();

		PWM_Init();
		//LED_Init();

		data = 0;
		voltage = 0;
			
    while (1) {
			// Start Regular Conversion
			ADC1->CR |= ADC_CR_ADSTART;
			
			// Wait for ADC conversion
			while((ADC1->ISR & ADC_ISR_EOC) == 0);
			
			// Ready ADC data register
			data = ADC1->DR;
			
			voltage = 3.3*((4096-((float)data))/4096);
			
			if (data > 4000) {
				TIM2->CCR1 |= 4096;
			} else {
				TIM2->CCR1 &= ~TIM_CCR1_CCR1;
				TIM2->CCR1 |= data;
			}
			
			
			for(int i=0; i<1000; ++i); // Some Delay
    }
}
