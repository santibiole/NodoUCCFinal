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

union u_Trama rxTrama;
union u_rx {
	struct str_trama{
		uint8_t encabezado[TAM_URX_TRAMA_ENCABEZADO];
		uint8_t led_rele_n;
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
void *Rele1, *Rele2;

extern void UART_Tx_string(char *dato, char cant);

char datos[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/*
 * Servicios de tramas de Network. Prototipos
 */
void app_comando_respTemp(uint8_t *datos);
void app_comando_reqTemp(uint8_t *datos);
void app_comando_reqLed(uint8_t *datos);
void app_comando_reqRele(uint8_t *datos);
void borrar_uartrx_buffer (void);
int analizar_dato_uartrx(int estado_anterior);
int analizar_trama_uartrx_led (void);
int analizar_trama_uartrx_rele (void);


void main(void) {

	typedef enum {
		mostrar_menu = 0,
		procesar_estado,
		onoff_led,
		onoff_rele
		} estados;
	estados	estado=0;

	int estado_anterior = 0;

	bsp_Init();

	while(1){

	    net_prossesNetwork();

	    switch (estado) {
	    		case mostrar_menu:
	    			// Se muestra el men�
	    			UART_Tx_string("\nMen�:\n1) On Off LED\n2) On Off RELE", 0);
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
	    	    		if(analizar_trama_uartrx_led()){
	    	    			borrar_uartrx_buffer();
	    	    			net_SendRequest(0x04, REQ_LED, datos);
	    				}
	    			}
	    			break;
	    		case onoff_rele:
	    			// On Off Rele
	    			estado_anterior = estado;
	    			if(uartrx_dato_disponible == 1){
	    				estado = analizar_dato_uartrx(estado_anterior);
	    	    		if(analizar_trama_uartrx_rele()){
	    	    			borrar_uartrx_buffer();
	    	    			net_SendRequest(0x04, REQ_RELE, datos);
	    				}
	    			}
	    			break;
	    }
	}
}

/**
 * @brief funci�n para responder con el valor de la temperatura sensada por el nodo
 */

void app_comando_respTemp (uint8_t *datos){

}

/*
 * brief:funci�n para pedir temperatura
 *
 */
void app_comando_reqTemp (uint8_t *datos){

}

/*
 * brief:funci�n para solicitar el prendido/apagado de los leds
 *
 */
void app_comando_reqLed (uint8_t *datos){
	if (datos[0]=='1'){
		if (datos[1]=='1'){
			led_On(Led1);
		} else
			led_Off(Led1);
	} else
		if (datos[1] == '1') {
		led_On(Led2);
	} else
		led_Off(Led2);
}

/*
 * brief:funci�n para solicitar el prendido/apagado de los reles
 *
 */
void app_comando_reqRele (uint8_t *datos){
	if (datos[0]=='1'){
		if (datos[1]=='1'){
			rele_On(Rele1);
		} else
			rele_Off(Rele1);
	} else
		if (datos[1] == '1') {
		rele_On(Rele2);
	} else
		rele_Off(Rele2);
}

/*
 * brief:funci�n para analizar el dato que ingresa por la uart
 *
 */
int analizar_dato_uartrx (int estado_anterior) {
	int estado;
	uartrx_dato_disponible = 0;
	if(estado_anterior == 0){
		if(rx.buffer[6]=='1'){
			UART_Tx_string("\n\nOn Off LED: Indique, en formato LEDX:X, el led que desea prender o apagar\n1) Volver\n", 0);
			estado = 2;
			borrar_uartrx_buffer();
		}
		if(rx.buffer[6]=='2'){
			UART_Tx_string("\n\nOn Off RELE: Indique, en formato RELX:X, el rele que desea prender o apagar\n1) Volver\n", 0);
			estado = 3;
			borrar_uartrx_buffer();
		}
	}
	if(estado_anterior == 2){
		if(rx.buffer[6]=='1' && rx.buffer[5]!='D' && rx.buffer[5]!=':'){
			estado = 0;
			borrar_uartrx_buffer();
		}
	}
	if(estado_anterior == 3){
		if(rx.buffer[6]=='1' && rx.buffer[5]!='L' && rx.buffer[5]!=':'){
			estado = 0;
			borrar_uartrx_buffer();
		}
	}
	return estado;
}

/*
 * brief:funci�n para analizar la trama de datos que ingresa por la uart
 *
 */
int analizar_trama_uartrx_led (void) {

	if (strncmp("LED",(char *)&rx.trama.encabezado[0],3)==0) {
		if (strncmp(":",(char *)&rx.trama.dos_p,1)==0) {
			if (strncmp("1",(char *)&rx.trama.estado,1)==0){
				datos[1]='1';
				if (strncmp("1",(char *)&rx.trama.led_rele_n,1)==0){
					datos[0]='1';
					return 1;
				} else if (strncmp("2",(char *)&rx.trama.led_rele_n,1)==0){
					datos[0]='2';
					return 1;
				}
			} else if (strncmp("0",(char *)&rx.trama.estado,1)==0) {
				datos[1]='0';
				if (strncmp("1",(char *)&rx.trama.led_rele_n,1)==0){
					datos[0]='1';
					return 1;
				} else if (strncmp("2",(char *)&rx.trama.led_rele_n,1)==0){
					datos[0]='2';
					return 1;
				}
			}
		}
	}
	return 0;
}

/*
 * brief:funci�n para analizar la trama de datos que ingresa por la uart
 *
 */
int analizar_trama_uartrx_rele (void) {

	if (strncmp("REL",(char *)&rx.trama.encabezado[0],3)==0) {
		if (strncmp(":",(char *)&rx.trama.dos_p,1)==0) {
			if (strncmp("1",(char *)&rx.trama.estado,1)==0){
				datos[1]='1';
				if (strncmp("1",(char *)&rx.trama.led_rele_n,1)==0){
					datos[0]='1';
					return 1;
				} else if (strncmp("2",(char *)&rx.trama.led_rele_n,1)==0){
					datos[0]='2';
					return 1;
				}
			} else if (strncmp("0",(char *)&rx.trama.estado,1)==0) {
				datos[1]='0';
				if (strncmp("1",(char *)&rx.trama.led_rele_n,1)==0){
					datos[0]='1';
					return 1;
				} else if (strncmp("2",(char *)&rx.trama.led_rele_n,1)==0){
					datos[0]='2';
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
