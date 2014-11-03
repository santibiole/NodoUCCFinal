#include"typedef.h"
#include"bsp.h"
#include"Network.h"

/*
 * main.c
 */

void* Sw;
void *Led1, *Led2;
void *Sw1, *Sw2, *Sw3, *Sw4;

char datos[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/*
 * Servicios de tramas de Network. Prototipos
 */
void app_comando_respTemp(uint8_t *datos);
void app_comando_reqTemp(uint8_t *datos);


void main(void) {

	bsp_Init();
	
	led_On(Led1);

	while(1){
	    net_prossesNetwork();

	    if(!sw_getState(Sw1)){
	    	UART_Tx_string("HOLA",0);
	        net_SendRequest(0x01, PING, datos);
	    }
	}
}

/**
 * @brief función para responder con el valor de la temperatura sensada por el nodo
 */

void app_comando_respTemp(uint8_t *datos){

}

/*
 * brief:función para pedir temperatura
 *
 */
void app_comando_reqTemp(uint8_t *datos){

}
