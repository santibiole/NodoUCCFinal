/**
 * @file bsp.c
 * @brief servicios bsp
 * @details
 * @author Agustin
 * @date 30/08/2013
 *
 */



#include "system.h"
#include "bsp.h"
#include "hal_pin.h"
#include "hal_cc2500.h"
#include "TI_MSP_UART.h"

extern void APP_ISR_uartrx (void);

/**
 * @var
 * @brief Declaración de buffer global de 10 elementos char donde se almacenarán
 * los datos recibidos en el registro RX del CC2500
 */
char rxBuffer[17];

/**
 * @var rf_rxBufferLen Variable Global donde se almacenará el tamaño de lo recibido //todo: por que la inicializamos con 10? no es necesario me parece porque se carga a través de la función hal_cc2500_receivePacket()
 * a través de hal_cc2500_receivePacket()
 *
 */
uint8_t rf_rxBufferLen = 17;

/**
 * @var rxAddr Variable Global donde se almacenará el Address del origen de lo recibido
 *
 */
uint8_t rxAddr;

/**
 * @var bsp_contMs Variable Global como contador
 *
 */
uint16_t bsp_contMs;


/**
 * @struct
 * @brief estructura con banderas de estado
 *
 */
struct str_bspFlags{
	uint8_t rf_rxDataReady		:1;/**< Datos listos. */

}bspFlags;


/**
 * @brief declaración de variables del tipo estructura pinConfig
 *
 */
const pinConfig pinLedRojo = {HAL_PORT1, 0, HAL_PIN_DIG_OUT, 0, BIT0};
const pinConfig pinLedVerde = {HAL_PORT1, 1, HAL_PIN_DIG_OUT, 0, BIT1};
const pinConfig pinLed1 = {HAL_PORT4, 5, HAL_PIN_DIG_OUT, 0, BIT5};
const pinConfig pinLed2 = {HAL_PORT4, 6, HAL_PIN_DIG_OUT, 0, BIT6};

const pinConfig pinSw = {HAL_PORT1, 2, HAL_PIN_DIG_IN_R, 1, BIT2};
const pinConfig pinSw1 = {HAL_PORT2, 0, HAL_PIN_DIG_IN_R, 1, BIT0};
const pinConfig pinSw2 = {HAL_PORT2, 1, HAL_PIN_DIG_IN_R, 1, BIT1};
const pinConfig pinSw3 = {HAL_PORT2, 2, HAL_PIN_DIG_IN_R, 1, BIT2};
const pinConfig pinSw4 = {HAL_PORT2, 3, HAL_PIN_DIG_IN_R, 1, BIT3};

const pinConfig pinRele1 = {HAL_PORT4, 4, HAL_PIN_DIG_OUT, 0, BIT4};
const pinConfig pinRele2 = {HAL_PORT4, 3, HAL_PIN_DIG_OUT, 0, BIT3};

const pinConfig cc_gdo0 = {HAL_PORT2, 6, HAL_PIN_INT_HILO, 0, BIT6};
const pinConfig cc_gdo2 = {HAL_PORT2, 7, HAL_PIN_DIG_IN, 0, BIT7};
const pinConfig cc_spiSOMI = {HAL_PORT3, 2, HAL_PIN_SEC_FUNC_OUT, 0, BIT2};
const pinConfig cc_spiSIMO = {HAL_PORT3, 1, HAL_PIN_SEC_FUNC_OUT, 0, BIT1};
const pinConfig cc_spiCLK = {HAL_PORT3, 3, HAL_PIN_SEC_FUNC_OUT, 0, BIT3};
const pinConfig cc_spinCS = {HAL_PORT3, 0, HAL_PIN_DIG_OUT, 1, BIT0};



/**
 * @brief función de inicialización de hardware. Inicializa pines y Timmer
 *
 */
void bsp_Init(void){
	WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    DCOCTL = CALDCO_1MHZ;       //seteo clock a 1MHZ
    BCSCTL1 = CALBC1_1MHZ;

    //inicialización pines
    LedRojo = (void*)hal_pin_init(&pinLedRojo); //hal_pin_init() carga en registros del micro lo establecido en la estructura
    LedVerde = (void*)hal_pin_init(&pinLedVerde);
	Sw = (void*)hal_pin_init(&pinSw);
	Led1 = (void*)hal_pin_init(&pinLed1);
	Led2 = (void*)hal_pin_init(&pinLed2);

	Sw1 = (void*)hal_pin_init(&pinSw1);
	Sw2 = (void*)hal_pin_init(&pinSw2);
	Sw3 = (void*)hal_pin_init(&pinSw3);
	Sw4 = (void*)hal_pin_init(&pinSw4);

	Rele1 = (void*)hal_pin_init(&pinRele1);
	Rele2 = (void*)hal_pin_init(&pinRele2);

	hal_pin_init(&cc_gdo0);
	hal_pin_init(&cc_gdo2);
	hal_pin_init(&cc_spiSOMI);
	hal_pin_init(&cc_spiSIMO);
	hal_pin_init(&cc_spiCLK);
	hal_pin_init(&cc_spinCS);

	//inicialización SPI
	spi1_setup((uint8_t*)&UCB0CTL0);

	//inicialización CC2500
	hal_cc2500_powerupReset();
	hal_cc2500_WriteRFSettings();

	//inicialización Timmer
    TACTL = (TASSEL_2 + ID_0 + MC_2);
    TACCTL1 = (CM_0 + CCIS_2);
    TACCR1 = TAR + 1000;
    TACCTL1 &= ~(CCIFG);
    TACCTL1 |= CCIE;

    Init_UART();                                // Inicialize USCIA (UART Mode)

	__enable_interrupt();

}

/**
 * @brief:función para delay de milisegundos
 *
 * @param [in] cm cantidad de milisegundos
 *
 * @retval None
 *
 */
void bsp_DelayMs(uint16_t cm){
	bsp_contMs = cm;
	while(bsp_contMs);
}

/**
 * @brief: función para prender led
 *
 * @param [in] led puntero al led a encender
 *
 * @retval None
 *
 * @see hal_pin_out()
 */
void led_On(void* led){
	hal_pin_out((const pinConfig*)led, 1);
}


/**
 * @brief: función para apagar led
 *
 * @param [in] led puntero al led a apagar
 *
 * @retval None
 *
 * @see hal_pin_out()
 */
void led_Off(void* led){
	hal_pin_out((const pinConfig*)led, 0);
}

/**
 * @brief: función para obtener el estado de un switch
 *
 * @param [in] sw switch a consultar
 *
 * @return estado del switch 1 presionado
 *                           0 no presionado
 *
 * @see hal_pin_in()
 */
uint8_t sw_getState(void* sw){
	return((uint8_t)hal_pin_in((void*)sw));
}

/**
 * @brief: función para prender rele
 *
 * @param [in] rele puntero al rele a encender
 *
 * @retval None
 *
 * @see hal_pin_out()
 */
void rele_On(void* rele){
	hal_pin_out((const pinConfig*)rele, 1);
}

/**
 * @brief: función para apagar rele
 *
 * @param [in] rele puntero al rele a apagar
 *
 * @retval None
 *
 * @see hal_pin_out()
 */
void rele_Off(void* rele){
	hal_pin_out((const pinConfig*)rele, 0);
}

/**
 * Se usa sevicio de la librería del CC2500:
 * This function transmits a packet with length up to 63 bytes.  To use this
 *  function, GD00 must be configured to be asserted when sync word is sent and
 *  de-asserted at the end of the packet, which is accomplished by setting the
 *  IOC FG0 register to 0x06,per the CCxxxx datasheet.  GDO0 goes high at
 *  packet start and returns low when complete.  The function polls GDO0 to
 *  ensure packet completion before returning.
 *
 * @param addr Variable containing the destination address to the transmitted packet
 * @param txBuffer Pointer to a buffer containing the data to be transmitted
 * @param size The size of the txBuffer
 * @return 1 Packet Sended
 *           0 Not Sended (channel busy)
 */
uint8_t rf_sendPacket(char addr, char *txBuffer, char size){
	return(hal_cc2500_sendPacket(addr, txBuffer, size));
}


/**
 * @brief:función para retornar el estado de la flag rf_rxDataReady
 *
 * @return 1 dato listo
 *         0 dato no listo
 */
uint8_t rf_rxDataReady(void){
	if(bspFlags.rf_rxDataReady)
		return 1;
	else
		return 0;
}


/*
 * @brief:función para levantar el paquete recibido si el dato está listo.
 *
 * Esta función emplea la función hal_cc2500_receivePacket() para recoger dato en el registro RX
 * del CC2500. Además recoge el Address de origen de lo recibido, la longitud de lo recibido y
 * que debería tener el buffer donde se almacenarán los datos, y RSSI y LQI
 *
 * @param buffer: puntero a la dirección del buffer donde se almacenará la trama guardada en rxBuffer. Ver @ref hal_cc2500_receivePacket
 * @param size: tamaño del buffer.
 *
 * @retval None
 *
 */
void rf_getRxPacket(char *buffer, uint8_t size){
	uint8_t i;

	if(bspFlags.rf_rxDataReady){
		bspFlags.rf_rxDataReady = 0;
		if(size >= rf_rxBufferLen){
			for(i=0;i<rf_rxBufferLen;i++){
				*(buffer + i) = rxBuffer[i];
			}
		}
	}
}

/**
 * @brief Vector interrupción.
 *
 *
 * @see hal_cc2500_receivePacket()
 */
#pragma vector=PORT2_VECTOR
__interrupt void Port2_ISR(void){

	uint8_t rssi, lqi; //variables que almacenarán los valores que recogerá la función al leer el registro del CC2500

	if(hal_pin_intGetFlag(&cc_gdo0)){
		hal_pin_intClearFlag(&cc_gdo0);
		rf_rxBufferLen = 17;
		if(hal_cc2500_receivePacket(&rxAddr, rxBuffer, (char*)&rf_rxBufferLen, &rssi, &lqi)){ //todo: la función solicita un puntero y le pasamos un array. Esta bien? no habría que ponerle &
			bspFlags.rf_rxDataReady = 1;
		}
	}
}

/**
 * Vector interrupción Timmer contador de milisegundos
 */
#pragma vector = TIMERA1_VECTOR
__interrupt void Ta1_ISR(void){

	if(TACCTL1 & CCIFG){
		TACCR1 += 1000;
		TACCTL1 &= ~CCIFG;

		if(bsp_contMs)
			bsp_contMs--;

		net_1ms_ISR();

	}
}

char UART_Rx_char (void) {
	char data;
	data = UART_UCAxRXBUF;
	UART_Tx_char(data);
    return data;
}

#pragma vector=UART_ISR_RX_VECTOR
__interrupt void ISR_UartRx(void){
    if(UART_IFG & UART_UCAxRXIFG){
      APP_ISR_uartrx();
    }
}

