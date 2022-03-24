#include "stm32l476xx.h"

static void DAC_Pin_Init(void) {
	// Enable GPIO Port A clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	// set PA4 to analog, no pull-up/no pull-down
	GPIOA->MODER &= ~GPIO_MODER_MODE4;
	GPIOA->MODER |= GPIO_MODER_MODE4;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD4;
}

void DAC_Write_Value(uint32_t value) {
	
	// DAC1Ch1 right-aligned 12-bit value
	if (value > 4095) {
		DAC->DHR12R1 = 0x0FFF;
	} else if (value < 1){
		DAC->DHR12R1 = 1;
	} else {
		DAC->DHR12R1 = value; 
	}
}

void DAC_Init(void) {
	DAC_Pin_Init();

	// Enable DAC clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_DAC1EN;
	DAC->CR  &= ~DAC_CR_EN1;
	
	// Enable software trigger mode
	DAC->CR |= DAC_CR_TEN1;
	DAC->CR &= ~DAC_CR_TSEL1;
	DAC->CR |= DAC_CR_TSEL1;
	
	// Disable trigger
	DAC->CR &= ~DAC_CR_TEN1;

	// DAC1 connected in normal mode to external pin only with buffer enabled
	DAC->MCR &= ~DAC_MCR_MODE1;
	
	// Enable DAC channel 1
	DAC->CR |= DAC_CR_EN1;
	
	DAC_Write_Value(0);
}
