/*
 * ECE 153B - Winter 2022
 *
 * Name(s):
 * Section:
 * Lab: 5B
 */


#include "ADC.h"
#include "DAC.h"
#include "EXTI.h"
#include "PWM.h"
#include "SysClock.h"
#include "stm32l476xx.h"

#include <stdio.h>

volatile uint32_t data = 0;
volatile float voltage = 0;

int main(void) {
    // Initialization
    System_Clock_Init(); // Switch System Clock = 16 MHz

    ADC_Init();
    DAC_Init();
    EXTI_Init();

    PWM_Init();
	
    while (1) {
			// Start Regular Conversion
			ADC1->CR |= ADC_CR_ADSTART;
			
			// Wait for ADC conversion
			while((ADC1->ISR & ADC_ISR_EOC) == 0);
			
			// Ready ADC data register
			data = ADC1->DR;
			
			voltage = 3.3*((4096-((float)data))/4096);
			
			if (data > 3985) {
				TIM2->CCR1 = 4096;
			} else {
				TIM2->CCR1 &= ~TIM_CCR1_CCR1;
				TIM2->CCR1 |= data;
			}
			
			
			for(int i=0; i<1000; ++i); // Some Delay
    }
}
