#include"typedef.h"
#include"bsp.h"
#include"Network.h"
#include <stdio.h>
#include <string.h>

/*
 * main.c
 */

#define TAM_URX_BUFFER 7
#define TAM_URX_TRAMA_ENCABEZADO 3

//union u_Trama rxTrama;
union u_rx {
	struct str_trama{
		uint8_t encabezado[TAM_URX_TRAMA_ENCABEZADO];
		uint8_t led_n;
		uint8_t dos_p;
		uint8_t estado;
		uint8_t fin_trama;
	}trama;

	uint8_t buffer[TAM_URX_BUFFER];
}rx;

static uint8_t uartrx_dato_disponible = 0;

void* Sw;
void *Led1, *Led2;
void *Sw1, *Sw2, *Sw3, *Sw4;

char datos[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/*
 * Servicios de tramas de Network. Prototipos
 */
void app_comando_respTemp(uint8_t *datos);
void app_comando_reqTemp(uint8_t *datos);
//void app_comando_reqLed(uint8_t *datos);
void borrar_uartrx_buffer (void);
int analizar_dato_uartrx(int estado_anterior);
int analizar_trama_uartrx (void);


void main(void) {

	typedef enum {
		mostrar_menu = 0,
		procesar_estado,
		onoff_led,
		} estados;
	estados	estado=0;

	int estado_anterior = 0;

	bsp_Init();

	while(1){

	    net_prossesNetwork();

	    switch (estado) {
	    		case mostrar_menu:
	    			// Se muestra el menú
	    			UART_Tx_string("\nMenú:\n1) On Off LED", 0);
	    			estado_anterior = estado;
	    			estado++;
	    			break;
	    		case procesar_estado:
	    			if(uartrx_dato_disponible == 1){
	    				estado = analizar_dato_uartrx(estado_anterior);
	    			}
	    			break;
	    		case onoff_led:
	    			// On Off LED
	    			estado_anterior = estado;
	    			if(uartrx_dato_disponible == 1){
	    				estado = analizar_dato_uartrx(estado_anterior);
	    	    		if(analizar_trama_uartrx()){
	    	    			led_On(Led2);
	    	    			borrar_uartrx_buffer();
	    				}
	    			}
	    }
	}
}

/**
 * @brief función para responder con el valor de la temperatura sensada por el nodo
 */

void app_comando_respTemp (uint8_t *datos){

}

/*
 * brief:función para pedir temperatura
 *
 */
void app_comando_reqTemp (uint8_t *datos){

}

/*
 * brief:función para solicitar el prendido/apagado de los leds
 *
 */
//void app_comando_reqLed (uint8_t *datos){
//	if (rxTrama.campos.Dato[0]=='1'){
//		if (rxTrama.campos.Dato[1]=='1'){
//			led_On(Led1);
//		} else
//			led_Off(Led1);
//	} else
//		if (rxTrama.campos.Dato[1] == '1') {
//		led_On(Led2);
//	} else
//		led_Off(Led2);
//}

int analizar_dato_uartrx (int estado_anterior) {
	int estado;
	uartrx_dato_disponible = 0;
	if(estado_anterior == 0){
		if(rx.buffer[6]=='1'){
			UART_Tx_string("\nOn Off LED: Indique, en formato LEDX:X,\nel led que desea prender o apagar\n1) Volver", 0);
			estado = 2;
			borrar_uartrx_buffer();
		}
	}
	if(estado_anterior == 2){
		if(rx.buffer[6]=='1' && rx.buffer[5]!='D' && rx.buffer[5]!=':'){
			estado = 0;
			borrar_uartrx_buffer();
		}
	}
	return estado;
}

int analizar_trama_uartrx (void) {

	if (strncmp("LED",(char *)&rx.trama.encabezado[0],3)==0) {
		if (strncmp(":",(char *)&rx.trama.dos_p,1)==0) {
			if (strncmp("1",(char *)&rx.trama.estado,1)==0){
				datos[1]=1;
				if (strncmp("1",(char *)&rx.trama.led_n,1)==0){
					datos[0]=1;
					return 1;
				} else if (strncmp("2",(char *)&rx.trama.led_n,1)==0){
					datos[0]=2;
					return 1;
				}
			} else if (strncmp("0",(char *)&rx.trama.estado,1)==0) {
				datos[1]=0;
				if (strncmp("1",(char *)&rx.trama.led_n,1)==0){
					datos[0]=1;
					return 1;
				} else if (strncmp("2",(char *)&rx.trama.led_n,1)==0){
					datos[0]=2;
					return 1;
				}
			}
		}
	}
	return 0;
}

void APP_ISR_uartrx (void){
	char data;
	unsigned int i;

	/*Levanto bandera de dato disponible en UART RX*/
	uartrx_dato_disponible = 1;
	/*Cargo dato disponible en "data"*/
	data = UART_Rx_char();

	for (i=0; i<TAM_URX_BUFFER-1; i++) {
			rx.buffer[i] = rx.buffer[i + 1];
		}
		rx.buffer[TAM_URX_BUFFER-1] = data;
}

void borrar_uartrx_buffer (void){
	unsigned int i;
	for (i=0; i<TAM_URX_BUFFER; i++)
		rx.buffer[i]=0;
}
