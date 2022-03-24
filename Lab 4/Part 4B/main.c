#include "stm32l476xx.h"

#include "I2C.h"
#include "SysClock.h"
#include "UART.h"
#include <string.h>
#include <stdio.h>

/*PinOuts
 *NC = None
 *SDA = PB7
 *GND = GND
 *SCLK = PB6
 *VDD = 3.3V
 */

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
	System_Clock_Init(); // System Clock = 80 MHz
	
	// Initialize I2C
	I2C_GPIO_Init();
	I2C_Initialization();

	// Initialize UART -- change the argument depending on the part you are working on
	Init_USARTx(2);
	
	int i;
	uint8_t test = 0;
	uint8_t SlaveAddress;
	uint8_t Data_Receive;
	uint8_t Data_Send;
	
	SlaveAddress = 0b1001000 << 1;
	
	while(1) {	
		Data_Send = 0;
		// [TODO] - Get Temperature
		// First, send a command to the sensor for reading the temperature
		I2C_SendData(I2C1, SlaveAddress, &Data_Send, 1);
		// Next, get the measurement
		I2C_ReceiveData(I2C1, SlaveAddress, &Data_Receive, 1);
		// [TODO] - Print Temperature to Termite
		printf("Temperature: %dC\n",Data_Receive);
		// Some delay
		for(i = 0; i < 50000; ++i); 
	}
}
