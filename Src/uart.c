#include "stm32f446xx.h"

#define UART2EN			(1U<<17)
#define CR1_TE			(1U<<3)
#define CR1_RE 			(1U<<2)
#define CR1_UE			(1U<<13)
#define	SR_TXE			(1U<<7)
#define	SR_RXNE			(1U<<5)
#define GPIOAEN			(1U<<0)
#define SYS_FREQ 		16000000
#define APB1_CLK		SYS_FREQ
#define UART_BAUDRATE	115200

static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk,uint32_t Baudrate);
static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t Baudrate );
void uart2_write(int ch);
char uart2_read(void);
void uart2_rxtx_init(void);

int __io_putchar(int ch)
{
	uart2_write(ch);
	return ch;
}

void uart2_rxtx_init(void)
{
	/************Configure uart gpio_pin******************/
	/*Enable clock access to gpioa*/
	RCC->AHB1ENR |=GPIOAEN;

	/*set PA2 to alternate function mode*/
	GPIOA->MODER &=~ (1U<<4);
	GPIOA->MODER |= (1U<<5);

	/*set PA2 alternate function type to UART_TX(AF7)*/
	GPIOA->AFR[0] |=(1U<<8);
	GPIOA->AFR[0] |=(1U<<9);
	GPIOA->AFR[0] |=(1U<<10);
	GPIOA->AFR[0] &=~(1U<<11);

	/*set PA3 to alternate function mode*/
	GPIOA->MODER &=~ (1U<<6);
	GPIOA->MODER |= (1U<<7);
	/*set PA3 alternate function type to UART_RX(AF7)*/
	GPIOA->AFR[0] |=(1U<<12);
	GPIOA->AFR[0] |=(1U<<13);
	GPIOA->AFR[0] |=(1U<<14);
	GPIOA->AFR[0] &=~(1U<<15);


	/************Configure UART MODULE******************/
	/*Enable clock access to uart2*/
	RCC->APB1ENR |= UART2EN;

	/*Configure baudrate*/
	uart_set_baudrate(USART2,APB1_CLK, UART_BAUDRATE);
	/*Configure transfer direction*/
	USART2->CR1 =(CR1_TE | CR1_RE); /*	Don't need operator because i want to clean everything in the uart*/
	/*Enable uart module*/
	USART2->CR1 |=CR1_UE;

}
void uart2_tx_init(void)
{
	/************Configure uart gpio_pin******************/
	/*Enable clock access to gpioa*/
	RCC->AHB1ENR |=GPIOAEN;

	/*set PA2 to alternate function mode*/
	GPIOA->MODER &=~ (1U<<4);
	GPIOA->MODER |= (1U<<5);

	/*set PA2 alternate function type to UART_TX(AF7)*/
	GPIOA->AFR[0] |=(1U<<8);
	GPIOA->AFR[0] |=(1U<<9);
	GPIOA->AFR[0] |=(1U<<10);
	GPIOA->AFR[0] &=~(1U<<11);

	/************Configure UART MODULE******************/
	/*Enable clock access to uart2*/
	RCC->APB1ENR |= UART2EN;

	/*Configure baudrate*/
	uart_set_baudrate(USART2,APB1_CLK, UART_BAUDRATE);
	/*Configure transfer direction*/
	USART2->CR1 =CR1_TE; /*	Don't need operator because i want to clean everything in the uart*/
	/*Enable uart module*/
	USART2->CR1 |=CR1_UE;

}

char uart2_read(void)
{
	/*****Make sure the receive data register is NOT empty before put data into it*/
	while(!(USART2->SR & SR_RXNE)) {}
	/****Ready data to be read*///
	return USART2->DR;
}


void uart2_write(int ch)
{
	/*****Make sure the transmit data register is empty before put data into it*/
	while(!(USART2->SR & SR_TXE)) {}
	/*****Write to transmit data register*/
	USART2->DR = (ch & 0xFF);
}



/*static is value readable only in this file*/
static void uart_set_baudrate(USART_TypeDef *USARTx, uint32_t PeriphClk,uint32_t Baudrate)
{
	USARTx->BRR = compute_uart_bd(PeriphClk, Baudrate);
}

static uint16_t compute_uart_bd(uint32_t PeriphClk, uint32_t Baudrate )
{
	//return((PeriphClk + (Baudrate/2U))/Baudrate);
	return (PeriphClk/Baudrate);
}
