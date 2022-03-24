#include "SPI.h"
#include <stdio.h>

// Note: When the data frame size is 8 bit, "SPIx->DR = byte_data;" works incorrectly. 
// It mistakenly send two bytes out because SPIx->DR has 16 bits. To solve the program,
// we should use "*((volatile uint8_t*)&SPIx->DR) = byte_data";

void SPI2_GPIO_Init(void) {
	// GPIO Port B Enabled
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	
	// GPIO Alternative Function
	GPIOB->MODER &= ~GPIO_MODER_MODE13;
	GPIOB->MODER &= ~GPIO_MODER_MODE14;
	GPIOB->MODER &= ~GPIO_MODER_MODE15;
	GPIOB->MODER |= (GPIO_MODER_MODE13_1 | GPIO_MODER_MODE14_1 | GPIO_MODER_MODE15_1);
	
	// GPIOB Alternative Function AF5
	GPIOB->AFR[1] &= ~GPIO_AFRH_AFSEL13;
	GPIOB->AFR[1] &= ~GPIO_AFRH_AFSEL14;
	GPIOB->AFR[1] &= ~GPIO_AFRH_AFSEL15;
	GPIOB->AFR[1] |= (GPIO_AFRH_AFSEL13_2 | GPIO_AFRH_AFSEL13_0);
	GPIOB->AFR[1] |= (GPIO_AFRH_AFSEL14_2 | GPIO_AFRH_AFSEL14_0);
	GPIOB->AFR[1] |= (GPIO_AFRH_AFSEL15_2 | GPIO_AFRH_AFSEL15_0);
	
	// GPIOB Output Type
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT13;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT14;
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT15;
	
	// GPIOB Output Speed
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR13;
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR14;
	GPIOB->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR15;
	GPIOB->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR13 | GPIO_OSPEEDER_OSPEEDR14 | GPIO_OSPEEDER_OSPEEDR15);
	
	// GPIOB PUPDR
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD13;
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD14;
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD15;
}

void SPI2_Init(void){
	// Enable the SPI clock.
	RCC->APB1ENR1 |= RCC_APB1ENR1_SPI2EN;
	
	// Set the RCC SPI reset bit, then clear it to reset the SPI2 peripheral.
	RCC->APB1RSTR1 |= RCC_APB1RSTR1_SPI2RST;
	RCC->APB1RSTR1 &= ~RCC_APB1RSTR1_SPI2RST;
	
	// Disable the SPI enable bit. The peripheral must be configured while it is disabled.
	SPI2->CR1 &= ~SPI_CR1_SPE;
	
	// Configure the peripheral for full-duplex communication.
	SPI2->CR1 &= ~SPI_CR1_RXONLY;
	
	// Configure the peripheral for 2-line unidirectional data mode.
	SPI2->CR1 &= ~SPI_CR1_BIDIMODE;
	
	// Disable output in bidirectional mode.
	SPI2->CR1 &= ~SPI_CR1_BIDIOE;
	
	// Configure the frame format as MSB first.
	SPI2->CR1 &= ~SPI_CR1_LSBFIRST;
	
	// Configure the frame format to 8-bit mode.
	SPI2->CR2 &= ~SPI_CR2_DS;
	SPI2->CR2 |= (SPI_CR2_DS_0 | SPI_CR2_DS_1 |SPI_CR2_DS_2);
	
	// Use Motorola SPI mode.
	SPI2->CR2 &= ~SPI_CR2_FRF;
	
	// Configure the clock to low polarity.
	SPI2->CR1 &= ~SPI_CR1_CPOL;
	
	// Configure the clock to first clock transition.
	SPI2->CR1 &= ~SPI_CR1_CPHA;
	
	// Set the baud rate prescaler to 16.
	SPI2->CR1 &= ~SPI_CR1_BR;
	SPI2->CR1 |= (SPI_CR1_BR_0 | SPI_CR1_BR_1);
	
	// Disable hardware CRC calculation.
	SPI2->CR1 &= ~SPI_CR1_CRCEN;
	
	// Set SPI2 to master mode.
	SPI2->CR1 |= SPI_CR1_MSTR;
	
	// Enable software SSM.
	SPI2->CR1 |= SPI_CR1_SSM;
	
	// Enable NSS pulse generation.
	SPI2->CR2 |= SPI_CR2_NSSP;
	
	// Set the FIFO threshold to 1/4 (required for 8-bit mode).
	SPI2->CR2 |= SPI_CR2_FRXTH;
	
	// Enable the SPI peripheral.
	SPI2->CR1 |= SPI_CR1_SPE;
	
}
 
void SPI_Transfer_Byte(SPI_TypeDef* SPIx, uint8_t write_data, uint8_t* read_data) {
	// Wait for the Transmit Buffer Empty flag to become set.
	while((SPIx->SR & SPI_SR_TXE ) != SPI_SR_TXE );
	
	/* Write data to the SPIx->DR register to begin transmission.
	   Note that the register memory address must be cast to (volatile uint8 t*)
	   and dereferenced when using the 8-bit format. */
	*((volatile uint8_t*)&SPIx->DR) = write_data;
	//write_data = *((volatile uint8_t*)&SPIx->DR);
	
	// Wait for the Busy to become unset for the transmission to complete.
	while( (SPIx->SR & SPI_SR_BSY) == SPI_SR_BSY );
	
	// Wait for the Receive Not Empty flag to set for the data to be received.
	while( (SPIx->SR & SPI_SR_RXNE ) != SPI_SR_RXNE );
	
	/* Read received data from the SPIx->DR register.
		 Again note that the register memory address must be cast to (volatile uint8 t*)
		 and dereferenced when using the 8-bit format. */
	*read_data = *((volatile uint8_t*)&SPIx->DR);

}