//----------------------------------------------------------------------------
//  Description:  Header file for TI_MSP_UART.c
//
//  MSP430 Pheriperal Interface Code Library v1.0
//
//  Agustin Laprovitta
//  Communication Lab. - UCC
//  March 2011
//  IAR Embedded Workbench v5.10
//----------------------------------------------------------------------------

// You MUST set the value of MSP430_UART_SMCLK_FREQ and MSP430_UART_BAUD_RATE
// in you app definition section
// and call Init_UART() function in the app configuration section.
// Example:
// #define MSP430_UART_SMCLK_FREQ SMCLK_1MHZ
// #define MSP430_UART_BAUD_RATE  BAUD_RATE_9600BPS

// Ensure to add UART Register Definition in you BSP App
// UART Register Definition

#include "msp430f2274.h"

#define UART_UCAxCTL0       UCA0CTL0
#define UART_UCAxCTL1       UCA0CTL1
#define UART_UCAxBR1        UCA0BR1 	
#define UART_UCAxBR0        UCA0BR0 
#define UART_UCAxMCTL       UCA0MCTL
#define UART_UCAxTXBUF      UCA0TXBUF
#define UART_UCAxRXBUF      UCA0RXBUF
#define UART_IE             IE2
#define UART_IFG            IFG2
#define UART_UCAxRXIE       UCA0RXIE
#define UART_UCAxTXIE       UCA0TXIE
#define UART_UCAxRXIFG      UCA0RXIFG
#define UART_UCAxTXIFG      UCA0TXIFG

#define UART_ISR_RX_VECTOR  USCIAB0RX_VECTOR
#define UART_ISR_TX_VECTOR  USCIAB0TX_VECTOR

#define UART_PORTSEL        P3SEL
#define UART_PORTDIR        P3DIR
#define UART_PIN_RX         BIT5
#define UART_PIN_TX         BIT4


#define SMCLK_1MHZ  1               // 1MHz
#define SMCLK_4MHZ  4               // 4MHz
#define SMCLK_8MHZ  8               // 8MHz
#define SMCLK_12MHZ 12               // 12MHz
#define SMCLK_16MHZ 16               // 16MHz

#define MSP430_UART_SMCLK_FREQ   SMCLK_1MHZ // BRCLK assigned to SMCLK

#define BAUD_RATE_9600BPS   9600       // 9600bps  
#define BAUD_RATE_19200BPS  19200       // 19200bps
#define BAUD_RATE_56000BPS  56000       // 5600bps
#define BAUD_RATE_115200BPS 115200       // 115200bps

#define MSP430_UART_BAUD_RATE   BAUD_RATE_9600BPS  // Baud Rate in [BPS]
          

//******************************************************************************
// Funcion: Init_UART
// Configuracion Inicial del USCI A como UART
//******************************************************************************
void Init_UART(void);
//*****************************************************************
// Funcion: UART_Tx_char();
//                escrbe un char por el UART
//******************************************************************
void UART_Tx_char(char dato);     

//*****************************************************************
// Funcion: UART_Tx_string(char *dato, char cant);
// Descr.:    escribe un string por el UART
// Paramatros: cant = 0 (escribe hasta encontrar un null (0))
//             cant != 0 (imprime el numero de chars asignado)
//*****************************************************************
// Puede usarse como:
//   sprintf((char *)&buffer[0], "variable: %d", var);
//   UART_Tx_string(buffer);
//*****************************************************************
void UART_Tx_string(char *dato, char cant);





