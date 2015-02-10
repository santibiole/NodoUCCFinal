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
#define CANTIDAD_NODOS 3

#define AP 0
#define N1 1
#define N2 2
#define N3 3

union u_Trama rxTrama;
//union u_rx {
//	struct str_trama{
//		uint8_t encabezado[TAM_URX_TRAMA_ENCABEZADO];
//		uint8_t led_rele_n;
//		uint8_t dos_p;
//		uint8_t estado;
//		uint8_t fin_trama;
//	}trama;
//
//	uint8_t buffer[TAM_URX_BUFFER];
//}rx;
//
//static uint8_t uartrx_dato_disponible = 0;

void* Sw;
void *Led1, *Led2;
void *Sw1, *Sw2, *Sw3, *Sw4;
void *Rele1, *Rele2;

extern void UART_Tx_string(char *dato, char cant);

uint8_t rssi;
uint8_t nodo=N1;
uint8_t nodo_actual;

char datos[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
char intensidad[CANTIDAD_NODOS];

/*
 * Servicios de tramas de Network. Prototipos
 */
void app_comando_respRSSI(uint8_t *datos);
void app_comando_reqRSSI(uint8_t *datos);
void app_comando_reqLed(uint8_t *datos);
void app_comando_reqRele(uint8_t *datos);
//void borrar_uartrx_buffer (void);
//int analizar_dato_uartrx(int estado_anterior);
//int analizar_trama_uartrx_led (void);
//int analizar_trama_uartrx_rele (void);


void main(void) {

	bsp_Init();

	while(1){

		net_prossesNetwork();

		if (!sw_getState(Sw1)) {
			nodo_actual = nodo;
			net_SendRequest(nodo, REQ_RSSI, datos);
		}
		if (nodo > CANTIDAD_NODOS) {
			nodo = N1;
		}
	}
}

/**
 * @brief función para responder con el valor de la temperatura sensada por el nodo
 */

void app_comando_respRSSI (uint8_t *datos){
	intensidad[nodo_actual-1]=datos[0];
	nodo++;
}

/*
 * brief:función para pedir temperatura
 *
 */
void app_comando_reqRSSI (uint8_t *datos){
	datos[0]=rssi;
	net_SendResponse(RESP_RSSI, datos);
}

/*
 * brief:función para solicitar el prendido/apagado de los leds
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
 * brief:función para solicitar el prendido/apagado de los reles
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
 * brief:función para analizar el dato que ingresa por la uart
 *
 */
//int analizar_dato_uartrx (int estado_anterior) {
//	int estado;
//	uartrx_dato_disponible = 0;
//	if(estado_anterior == 0){
//		if(rx.buffer[6]=='1'){
//			UART_Tx_string("\n\nOn Off LED: Indique, en formato LEDX:X, el led que desea prender o apagar\n1) Volver\n", 0);
//			estado = 2;
//			borrar_uartrx_buffer();
//		}
//		if(rx.buffer[6]=='2'){
//			UART_Tx_string("\n\nOn Off RELE: Indique, en formato RELX:X, el rele que desea prender o apagar\n1) Volver\n", 0);
//			estado = 3;
//			borrar_uartrx_buffer();
//		}
//	}
//	if(estado_anterior == 2){
//		if(rx.buffer[6]=='1' && rx.buffer[5]!='D' && rx.buffer[5]!=':'){
//			estado = 0;
//			borrar_uartrx_buffer();
//		}
//	}
//	if(estado_anterior == 3){
//		if(rx.buffer[6]=='1' && rx.buffer[5]!='L' && rx.buffer[5]!=':'){
//			estado = 0;
//			borrar_uartrx_buffer();
//		}
//	}
//	return estado;
//}

/*
 * brief:función para analizar la trama de datos que ingresa por la uart
 *
 */
//int analizar_trama_uartrx_led (void) {
//
//	if (strncmp("LED",(char *)&rx.trama.encabezado[0],3)==0) {
//		if (strncmp(":",(char *)&rx.trama.dos_p,1)==0) {
//			if (strncmp("1",(char *)&rx.trama.estado,1)==0){
//				datos[1]='1';
//				if (strncmp("1",(char *)&rx.trama.led_rele_n,1)==0){
//					datos[0]='1';
//					return 1;
//				} else if (strncmp("2",(char *)&rx.trama.led_rele_n,1)==0){
//					datos[0]='2';
//					return 1;
//				}
//			} else if (strncmp("0",(char *)&rx.trama.estado,1)==0) {
//				datos[1]='0';
//				if (strncmp("1",(char *)&rx.trama.led_rele_n,1)==0){
//					datos[0]='1';
//					return 1;
//				} else if (strncmp("2",(char *)&rx.trama.led_rele_n,1)==0){
//					datos[0]='2';
//					return 1;
//				}
//			}
//		}
//	}
//	return 0;
//}

/*
 * brief:función para analizar la trama de datos que ingresa por la uart
 *
 */
//int analizar_trama_uartrx_rele (void) {
//
//	if (strncmp("REL",(char *)&rx.trama.encabezado[0],3)==0) {
//		if (strncmp(":",(char *)&rx.trama.dos_p,1)==0) {
//			if (strncmp("1",(char *)&rx.trama.estado,1)==0){
//				datos[1]='1';
//				if (strncmp("1",(char *)&rx.trama.led_rele_n,1)==0){
//					datos[0]='1';
//					return 1;
//				} else if (strncmp("2",(char *)&rx.trama.led_rele_n,1)==0){
//					datos[0]='2';
//					return 1;
//				}
//			} else if (strncmp("0",(char *)&rx.trama.estado,1)==0) {
//				datos[1]='0';
//				if (strncmp("1",(char *)&rx.trama.led_rele_n,1)==0){
//					datos[0]='1';
//					return 1;
//				} else if (strncmp("2",(char *)&rx.trama.led_rele_n,1)==0){
//					datos[0]='2';
//					return 1;
//				}
//			}
//		}
//	}
//	return 0;
//}

//void APP_ISR_uartrx (void){
//	char data;
//	unsigned int i;
//
//	/*Levanto bandera de dato disponible en UART RX*/
//	uartrx_dato_disponible = 1;
//	/*Cargo dato disponible en "data"*/
//	data = UART_Rx_char();
//
//	for (i=0; i<TAM_URX_BUFFER-1; i++) {
//			rx.buffer[i] = rx.buffer[i + 1];
//		}
//		rx.buffer[TAM_URX_BUFFER-1] = data;
//}

//void borrar_uartrx_buffer (void){
//	unsigned int i;
//	for (i=0; i<TAM_URX_BUFFER; i++)
//		rx.buffer[i]=0;
//}
