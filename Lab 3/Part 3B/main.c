#include <stdio.h> 
 
#include "stm32l476xx.h"

uint32_t volatile currentValue = 0;
uint32_t volatile lastValue = 0;
uint32_t volatile overflowCount = 0;
uint32_t volatile timeInterval = 0;
uint32_t volatile test = 0;
uint32_t volatile distance = 0;

void Input_Capture_Setup() {
	// Enable the clock for GPIO Port B
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	
	// Configure PB6 to be used as alternative function TIM4 CH1
	GPIOB->MODER &= ~GPIO_MODER_MODE6;
	GPIOB->MODER |= GPIO_MODER_MODE6_1;
	
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFSEL6;
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_1;
	
	// Set PB6 to no pull-up, no pull-down
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD6;
	
	// Enable Timer 4 in RCC APB1ENRx
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM4EN;
	
	// Set the prescaler to 15
	TIM4->PSC &= ~TIM_PSC_PSC;
	TIM4->PSC = 15;
	
	// Enable auto reload preload in the control register and set the auto reload value to its maximum value
	TIM4->CR1 |= TIM_CR1_ARPE;
	TIM4->ARR &= ~TIM_ARR_ARR;
	TIM4->ARR |= TIM_ARR_ARR;
	
	// In the capture/compare mode register, set the input capture mode bits such that the input capture is mapped to timer input 1
	TIM4->CCMR1 &= ~TIM_CCMR1_CC1S;
	TIM4->CCMR1 |= TIM_CCMR1_CC1S_0;
	
	// In the capture/compare enable register, set bits to capture both rising/falling edges and enable capturing
	TIM4->CCER |= TIM_CCER_CC1NP;
	TIM4->CCER |= TIM_CCER_CC1P;
	TIM4->CCER |= TIM_CCER_CC1NE;
	TIM4->CCER |= TIM_CCER_CC1E;
	
	// In the DMA/Interrupt enable register, enable both interrupt and DMA requests. In addition, enable the update interrupt
	TIM4->DIER |= TIM_DIER_CC1IE;
	TIM4->DIER |= TIM_DIER_CC1DE;
	TIM4->DIER |= TIM_DIER_UIE;
	
	// Enable update generation in the event generation register
	TIM4->EGR |= TIM_EGR_UG;
	
	// Clear the update interrupt flag
	TIM4->SR &= ~TIM_SR_UIF;
	
	// Set the direction of the counter and enable the counter in the control register
	TIM4->CR1 &= ~TIM_CR1_DIR;
	TIM4->CR1 |= TIM_CR1_CEN;
	
	// Enable the interrupt (TIM4 IRQn) in the NVIC and set its priority to 2
	NVIC_EnableIRQ(TIM4_IRQn);
	NVIC_SetPriority(TIM4_IRQn, 2);
	
}


void TIM4_IRQHandler(void) {
	if((TIM4 -> SR) & TIM_SR_CC1IF) {
		test++;
		if((GPIOB -> IDR) & GPIO_IDR_ID6) {
			lastValue = (TIM4 -> CCR1) & TIM_CCR1_CCR1;
			overflowCount = 0;
		}
		else {
			currentValue = (TIM4 -> CCR1) & TIM_CCR1_CCR1;
			timeInterval = currentValue + (65535*overflowCount) - lastValue;
		}
	}
	if((TIM4 -> SR) & TIM_SR_UIF) {
		// A counter overflow triggered this interrupt
		overflowCount++;
		TIM4 -> SR &= ~TIM_SR_UIF;
	}

}

void Trigger_Setup() {
	// GPIOA Enable Clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	// GPIOA MODER
	GPIOA->MODER &= ~GPIO_MODER_MODE9;
	GPIOA->MODER |= GPIO_MODER_MODE9_1;
	
	// Configuration for TIM1 CH2
	GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL9;
	GPIOA->AFR[1] |= GPIO_AFRH_AFSEL9_0;
	
	// No pull up/ no pull down
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD9;
	
	// Output to Push Pull
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT9;
	
	// GPIOA High Output Speed
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9;
	
	// Enable TIM1 CH2
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	
	// Prescaler 16MHz/(1+15) = 1MHz
	TIM1->PSC &= ~TIM_PSC_PSC;
	TIM1->PSC = 15;
	
	// Enable Autoreload
	TIM1->CR1 |= TIM_CR1_ARPE;
	
	// Autoreload to max value
	TIM1->ARR &= ~TIM_ARR_ARR;
	TIM1->ARR |= TIM_ARR_ARR;
	
	// CCR to activate sensor (10us)
	// PW = (CCR + 1)*CLK = 10*1MHz = 10us
	TIM1->CCR2 &= ~TIM_CCR2_CCR2;
	TIM1->CCR2 = 10;
	
	// PWM Mode 1 & Enable Output compare preload
	TIM1->CCMR1 &= ~TIM_CCMR1_OC2M;
	TIM1->CCMR1 |= (TIM_CCMR1_OC2M_1) | (TIM_CCMR1_OC2M_2);
	TIM1->CCMR1 |= TIM_CCMR1_OC2PE;
	
	// Enable output for TIM1_CH2
	TIM1->CCER |= TIM_CCER_CC2E;
	
	// BDTR Enable and Off-State Selecton (Run Mode)
	TIM1->BDTR |= (TIM_BDTR_MOE) | (TIM_BDTR_OSSR);
	
	// Enable update generation in the event generation register
	TIM1->EGR |= TIM_EGR_UG;
	
	// Enable update interrupt and clear the update interrupt flag.
	TIM1->DIER |= TIM_DIER_UIE;
	TIM1->SR &= ~TIM_SR_UIF;
	
	// Set the direction of the counter and enable the counter in the control register
	TIM1->CR1 &= ~TIM_CR1_DIR;
	TIM1->CR1 |= TIM_CR1_CEN;
}


int main(void) {	
	// Enable High Speed Internal Clock (HSI = 16 MHz)
	RCC->CR |= RCC_CR_HSION;
	while ((RCC->CR & RCC_CR_HSIRDY) == 0); // Wait until HSI is ready
	
	// Select HSI as system clock source 
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_HSI;
	while ((RCC->CFGR & RCC_CFGR_SWS) == 0); // Wait until HSI is system clock source
  
	// Input Capture Setup
	Input_Capture_Setup();
	
	// Trigger Setup
	Trigger_Setup();

	
	while(1) {
		if (timeInterval / 58 > 400) { // Out of Range
			distance = 0;
		} else {
			distance = timeInterval / 58;
		}
	}
}
