#include "stm32l476xx.h"
 
void PWM_Init() {
	// Enable GPIO Port A Clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	
	// Enable TIM2 Clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	
	// Configure PA5
	GPIOA->MODER &= ~GPIO_MODER_MODE5;
	GPIOA->MODER |= GPIO_MODER_MODE5_1;
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5;
	
	// Configure PWM Output for TIM2 CH 1
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL5;
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL5_0;
	
	// TIM2_CH1 Direction (Upward)
	TIM2->CR1 &= ~TIM_CR1_DIR;
	
	// TIM2_CH1 PreScalar fout = fin /(PSC + 1)
	TIM2->PSC = 3; // 4Mhz/(3+1) = 1MHz
	
	// TIM2_CH1 Auto-Reload Reg
	TIM2->ARR = 999; 
	
	// TIM2_CH1 Output Compare Mode & Preload Enable
	TIM2->CCMR1 |= TIM_CCMR1_OC1M_1;
	TIM2->CCMR1 |= TIM_CCMR1_OC1M_2;
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE;
	
	// TIM2_CH1 Output Polarity active high & Enable
	TIM2->CCER |= TIM_CCER_CC1P;
	TIM2->CCER |= TIM_CCER_CC1E;
	
	// Capture/Compare Value
	TIM2->CCR1 |= 0;
	
	// Enable Counter
	TIM2->CR1 |= TIM_CR1_CEN;
}
 
int main() {
	// Initialization - We will use the default 4 MHz clock
	PWM_Init();
	
	// Periodic Dimming
	int i;
	int dir = 0; 			// 0 = Up, 1 = Down
	int p = 0;				// Counter
	int tresh = 999; // Treshold
	while(1) {
		// [TODO] (changing duty cycle, etc.)
		if (dir == 0) {
			if (p == tresh) {
				dir = 1;
			} else {
				p++;
			}
		}
		if (dir == 1) {
			if (p == 0) {
				dir = 0;
			} else {
				p--;
			}
		}
		TIM2->CCR1 &= ~TIM_CCR1_CCR1;
		TIM2->CCR1 |= p;
		for(i=0; i<1000; ++i); // Some Delay
	}
	
}
