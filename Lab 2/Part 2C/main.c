#include "stm32l476xx.h"

#include "LED.h"
#include "RTC.h"
#include "SysClock.h"

char strTime[12] = {0};
char strDate[12] = {0};

#define __RTC_CONVERT_BIN2BCD(__VALUE__) (uint8_t)((((__VALUE__) / 10) << 4) | ((__VALUE__) % 10))
#define __RTC_CONVERT_BCD2BIN(__VALUE__) (uint8_t)(((uint8_t)((__VALUE__) & (uint8_t)0xF0) >> (uint8_t)0x4) * 10 + ((__VALUE__) & (uint8_t)0x0F))

void RTC_Set_Alarm(void) {
	// Disable Both Alarms
	RTC->CR &= ~RTC_CR_ALRAE;
	RTC->CR &= ~RTC_CR_ALRBE;
	
	// Disable RTC Write Protection
	RTC_Disable_Write_Protection();
	
	// Disable Alarms Interrupts
	RTC->CR &= ~RTC_CR_ALRAIE;
	RTC->CR &= ~RTC_CR_ALRBIE;
	
	// Wait for Alarms to be ready
	while((RTC->ISR & RTC_ISR_ALRAWF) == 0);
	while((RTC->ISR & RTC_ISR_ALRBWF) == 0);
	
	// Mask Trigger for Alarm A
	RTC->ALRMAR &= ~RTC_ALRMAR_MSK1;
	RTC->ALRMAR |= RTC_ALRMAR_MSK2;
	RTC->ALRMAR |= RTC_ALRMAR_MSK3;
	RTC->ALRMAR |= RTC_ALRMAR_MSK4;
	
	//RTC->ALRMAR |= RTC_ALRMBR_ST_0;
	RTC->ALRMAR |= RTC_ALRMBR_ST_1;
	
	// Mask Trigger for Alarm B
	RTC->ALRMBR |= RTC_ALRMBR_MSK1;
	RTC->ALRMBR |= RTC_ALRMBR_MSK2;
	RTC->ALRMBR |= RTC_ALRMBR_MSK3;
	RTC->ALRMBR |= RTC_ALRMBR_MSK4;
	
	// Enable Alarm and Interrupts
	RTC->CR |= RTC_CR_ALRAE;
	RTC->CR |= RTC_CR_ALRBE;
	RTC->CR |= RTC_CR_ALRAIE;
	RTC->CR |= RTC_CR_ALRBIE;
	
	// Enable RTC Write Protection
	RTC_Enable_Write_Protection();
}

void RTC_Alarm_Enable(void) {
	// Rising Edge Trigger
	EXTI->RTSR1 |= EXTI_RTSR1_RT18;
	
	// Interrupt & Mask Interrupt
	EXTI->IMR1 |= EXTI_IMR1_IM18;
	EXTI->EMR1 |= EXTI_EMR1_EM18;
	
	// Clearing pending interrupt bits
	EXTI->PR1 |= EXTI_PR1_PIF18;
	
	// Enable & Priority Interrupt
	NVIC_EnableIRQ(RTC_Alarm_IRQn);
	NVIC_SetPriority(RTC_Alarm_IRQn, 0);
}

void RTC_Alarm_IRQHandler(void) {
	// Clear interrupt pending flags
	EXTI->PR1 |= EXTI_PR1_PIF18;
	
	if(RTC->ISR & RTC_ISR_ALRAF)
	{
		// Clear event flag
		RTC->ISR &= ~RTC_ISR_ALRAF;
		
		// Toggle LED
		Green_LED_Toggle();
	}
	
	if(RTC->ISR & RTC_ISR_ALRBF)
	{
		// Clear event flag
		RTC->ISR &= ~RTC_ISR_ALRBF;
		
		// Toggle LED
		Green_LED_Toggle();
	}
}

int main(void) {	
	System_Clock_Init(); // Switch System Clock = 80 MHz
	
	LED_Init();
	
	RTC_Init();
	RTC_Alarm_Enable();
	RTC_Set_Alarm();
	
	while(1) {
		Get_RTC_Calendar(strTime, strDate);
	}
}
