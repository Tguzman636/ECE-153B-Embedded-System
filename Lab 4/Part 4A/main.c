#include "stm32l476xx.h"

#include "LED.h"
#include "SysClock.h"
#include "UART.h"

#include <string.h>
#include <stdio.h>

// Initializes USARTx
// USART2: UART Communication with Termite
// USART1: Bluetooth Communication with Phone
void Init_USARTx(int x) {
	if(x == 1) {
		UART1_Init();
		UART1_GPIO_Init();
		USART_Init(USART1);
	} else if(x == 2) {
		UART2_Init();
		UART2_GPIO_Init();
		USART_Init(USART2);
	} else {
		// Do nothing...
	}
}

int main(void) {
	System_Clock_Init(); // Switch System Clock = 80 MHz
	LED_Init();
	
	// Initialize UART -- change the argument depending on the part you are working on
	Init_USARTx(2);
	
	char rxByte;
	printf("Welcome to UART\n");
	printf("Please enter a letter\n");
	printf("Y/y = LED ON, N/n = LED OFF\n");
	while(1) {
		//printf("Start\n");
		scanf("%c", &rxByte);
		if(rxByte == 'y' || rxByte == 'Y')
		{
			printf("LED ON\n");
			Green_LED_On();
		}
		else if(rxByte == 'n' || rxByte == 'N')
		{
			printf("LED OFF\n");
			Green_LED_Off();
		}
		else
		{
			printf("Invalid - Try another letter\n");
		}
	}
}
