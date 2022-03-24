#include "PWM.h"

void PWM_Init(void) 
{
    // Enable GPIO Port A Clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    
    // Enable TIM2 Clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    
    // Configure PA5
    GPIOA->MODER &= ~GPIO_MODER_MODE5_0;
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5;
    
    // Configure PWM Output for TIM2 CH 1
    GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL5;
    GPIOA->AFR[0] |= GPIO_AFRL_AFSEL5_0;
    
    // TIM2_CH1 Direction (Upward)
    TIM2->CR1 &= ~TIM_CR1_DIR;
    
    // TIM2_CH1 PreScalar fout = fin /(PSC + 1)
    TIM2->PSC = 7;
    
    // TIM2_CH1 Auto-Reload Reg
    TIM2->ARR = 4095;
    
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
