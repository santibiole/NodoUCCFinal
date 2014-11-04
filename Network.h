/**
 * @file Network.h
 * @brief servicios Network
 * @details
 * @author Agustin
 * @date 30/08/2013
 *
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include"typedef.h"

/**
 * @def definición de constantes.
 *
 */
#define MY_ADDRESS          1

#define MAX_CANT_REINTENTOS	3
#define MAX_CANT_RETRANS    3
#define TAM_DATO			10
#define TAM_TRAMA			TAM_DATO + 7
#define PREAMB				0xAA


/**
 * @typedef definición de constantes.
 *
 */
typedef enum {
	REQ_TEMP = 0, /**< Request temperatura */
	RESP_TEMP,    /**< Respuesta Temperatura */
	PING,         /**< Ping */
	PONG,         /**< Pong */
	REQ_LED,	  /**< Request On Off Led */
	RESP_LED	  /**< Respuesta On Off Led */
}type_Comando;


/**
 * @struct estructura para una trama
 *
 */
struct str_camposTrama{
	uint8_t 		Preambulo;      /**< Preámbulo */
	uint8_t			IdTrama;        /**< Id Trama */
	uint8_t			DirDestino;     /**< Dirección Destino */
	uint8_t			DirOrigen;      /**< Dirección Origen */
	type_Comando	Comando;        /**< Comando. Este parámetro puede ser un valor de @ref type_Comando*/			     //TODO: corroborar el tamaño de este tipo de variable
	uint8_t			Dato[TAM_DATO]; /**< Buffer de Datos de tamaño de @ref TAM_DATO */
	uint8_t			Chksum;         /**< Checksum */
};

/**
 * @typedef definición de estructura de banderas
 *
 */
typedef struct{
    uint8_t ack_wait    :1;     /**< Bandera que indica la espera de un acknowledge */
    uint8_t not_ack     :1;     /**< Bandera que indica que el que transmitió no recibió un ack */
    uint8_t ack_error   :1;     /**< Bandera que indica que no se recibió un acknowledge por algún error */

}str_netFlags;



/**
 * @union unión para manejar memoria como buffer o como estructura
 *
 */
union u_Trama{
	struct str_camposTrama 	campos;
	uint8_t					buffer[TAM_TRAMA];
};


/**
 * @brief prototipos de funciones
 */
extern void app_comando_respTemp(uint8_t *datos);
extern void app_comando_reqTemp(uint8_t *datos);

void net_Init(void);
uint8_t net_SendRequest(uint8_t dirDestino, type_Comando comando, uint8_t *datos);
uint8_t net_SendResponse(type_Comando comando, uint8_t *datos);
void net_prossesNetwork(void);

uint8_t chksum(uint8_t *buffer);
uint8_t net_sendtrama(void);
void net_analizarTrama(void);
void net_1ms_ISR(void);

#endif /* NETWORK_H_ */
