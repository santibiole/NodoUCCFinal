/**
 * @file Network.c
 * @brief servicios Network
 * @details
 * @author Agustin
 * @date 30/08/2013
 *
 */


#include"Network.h"
#include"bsp.h"

void* LedRojo;
void* LedVerde;

/**
 * @brief definición de uniones del tipo u_Trama
 *
 */
union u_Trama rxTrama, txTrama ;

/**
 * @var netFlags definición de variable global del tipo @ref str_netFlags
 *
 */
str_netFlags netFlags;




uint8_t idTrama;

/**
 * @var idTramaRx definición de variable global para almacenar el id de trama
 *
 */
uint8_t idTramaRx;


/**
 * @var myAddr definición de variable global
 *
 */
uint8_t myAddr = MY_ADDRESS;

/**
 * @var timeout_ack definición de variable global como contador para esperar ack
 *
 */
uint16_t timeout_ack;


/**
 * @var req_retrans definición de variable global para llevar cuenta de las retransmiciones
 *
 */
uint8_t req_retrans;


/**
 * @brief función para enviar trama. Se interntará hacerlo tres veces.
 * @return bandera flag:1 se envió paquete. Led rojo se prende
 *                 flag:0 se supero el máximo de reintentos (3) de enviar trama.
 */
uint8_t net_sendtrama(void){
    uint8_t reintentos, flag, send;

    flag = 1;
    send = 0;
    reintentos = 0;
    while(flag && (!send)){
        send = rf_sendPacket((char)txTrama.campos.DirDestino, (char*)&txTrama.buffer[0], TAM_TRAMA); //send:1 se envio paquete
        if(send){
            led_On(LedRojo);                                                                   //se indica con led rojo que el paquete fue enviado
            bsp_DelayMs(300);
            led_Off(LedRojo);
            break;                                                                            //sale de la función si se envió paquete y retorna flag:1
        }

        reintentos++;
        if(reintentos > MAX_CANT_REINTENTOS){
            flag = 0;                                               //si se superó la cantidad de reintentos resetear flag para informar que no se envió.
        }
        bsp_DelayMs(300);                                                                     //demora antes de reintentar un envío
    }
    return flag;
}


/**
 * @brief función para enviar solicitud de acuerdo al tipo de comando.
 *
 * Se envía trama solicitando, a través de la especificación de un Comando, la acción requerida (@ref type_Comando)
 * Para ello la función carga en la unión txTrama (@ref str_camposTrama)
 * la información necesaria en cada miembro.
 *
 * @param [in] dirDestino: dirección de destino donde enviar trama
 * @param [in] comando: para especificar el tipo de solicitud. (@ref type_Comando)
 * @param [in] datos: puntero a la dirección del buffer donde se almacenan los datos
 *
 * @return    se envió solicitud 1 y se espera un ack
 *         no se envió solicitud 0. Se intentó enviar 3 veces.
 */
uint8_t net_SendRequest(uint8_t dirDestino, type_Comando comando, uint8_t *datos){

    uint8_t i;

    //se carga estructura
    txTrama.campos.Preambulo = PREAMB;
    txTrama.campos.IdTrama = idTrama++;
    txTrama.campos.DirDestino = dirDestino;
    txTrama.campos.DirOrigen = myAddr;
    txTrama.campos.Comando = comando;
    for(i=0;i<TAM_DATO;i++){                               //se carga el campo Dato de la estructura con los datos apuntados
        txTrama.campos.Dato[i] = *(datos+i);
    }
    txTrama.campos.Chksum = chksum(&txTrama.buffer[0]);    //calculo checksum y lo cargo en el campo chksum de la estructura

    req_retrans = 0;
    if(net_sendtrama()){                                   //Se envía trama y se pregunta por si se envió
        netFlags.ack_wait = 1;                             //espero una ack
        netFlags.not_ack = 0;
        timeout_ack = 500;                                 //cargo contador con los milisegundos a esperar un ack
        return 1;
    } else{
        return 0;
    }
 }

/**
 * @brief función para enviar respuesta de acuerdo al tipo de comando.
 *
 * Se envía trama con un tipo de respuesta solicitada en el comando de la trama recibida.
 * Para ello la función carga en la unión txTrama (@ref str_camposTrama)
 * la información necesaria en cada miembro.
 *
 * @param comando: para especificar el tipo de respuesta. (@ref type_Comando)
 * @param datos: puntero a la dirección del buffer donde se almacenan los datos
 * @return    se envió trama 1
 *         no se envió trama 0. Se superó la cantidad de reintentos permitidos
 */
uint8_t net_SendResponse(type_Comando comando, uint8_t *datos){

    uint8_t i;
    //se carga la estructura del tipo txtrama
    txTrama.campos.Preambulo = PREAMB;
    txTrama.campos.IdTrama = rxTrama.campos.IdTrama;
    txTrama.campos.DirDestino = rxTrama.campos.DirOrigen;
    txTrama.campos.DirOrigen = myAddr;
    txTrama.campos.Comando = comando;

    for(i=0;i<TAM_DATO;i++){                                //se carga el campo Dato de la estructura con los datos apuntados
        txTrama.campos.Dato[i] = *(datos+i);
    }

    txTrama.campos.Chksum = chksum(&txTrama.buffer[0]);    //calculo checksum y lo cargo en el campo chksum de la estructura

    return net_sendtrama();                                //Se envía trama y se retorna
}




/**
 * @brief función para analizar trama.
 *
 *
 * Si lo recibido contiene el PREAMB adecuado, si el cálculo del checksum coincide con
 * el campo checksum recibido y el campo DirDestino coincide con myAdress pregunta
 *
 * -Si si está esperando un ack y de acuerdo al comando será que recibió el valor de temperatura o un PONG.
 * -Si no está esperando un ack significa que es un REQUEST. Pregunta si la id de trama almacenada es distinta
 * a la recibida. Si asi es guarda id de trama nueva y envía temperatura o un PING.
 * Si coincide significa que la trama recibida ya se recibió pero aparentemente no llegó lo solicitado por lo
 * que se retransmite.
 *
 *
 *
 */

void net_analizarTrama(void){

    //buffer de datos a enviar en la trama con comando PONG
    uint8_t buffer[TAM_DATO] = {0,0,0,0,0,0,0,0,0,0}; //declaro e inicializo con ceros un buffer con TAM_DATO para enviar un PONG y nada de info?? por qué con cero?

    /*
     * Se pregunta por si coincide PREAMB, el checksum y si la trama es para myAddr
     */
    if(rxTrama.campos.Preambulo == PREAMB){
        if(chksum(&rxTrama.buffer[0]) == rxTrama.campos.Chksum){
            if(rxTrama.campos.DirDestino == myAddr){
                // Pregunta por si estoy a la espera de una trama de respuesta
                if(netFlags.ack_wait){
                    //Acciones de acuerdo a lo que haya que hacer con la trama de acuerdo a lo que traiga como comando
                    switch(rxTrama.campos.Comando){
                    	//si la trama es una respuesta con el valor de temperatura
                        case RESP_TEMP:
                            app_comando_respTemp(&rxTrama.campos.Dato[0]); //el campo dato, donde se guarda la temperatura se procesa en la función app_comando_respTemp()
                            //se resetean flags
                            netFlags.ack_wait = 0;
                            netFlags.not_ack = 0;
                            timeout_ack = 0;

                        break;
                        //si es una respuesta a un PING reinicia banderas
                        case PONG:
                            netFlags.ack_wait = 0;
                            netFlags.not_ack = 0;
                            timeout_ack = 0;
                        break;
                        //si es una respuesta a un REQ_LED reinicia banderas
                        case RESP_LED:
                            netFlags.ack_wait = 0;
                            netFlags.not_ack = 0;
                            timeout_ack = 0;
                        default:

                        break;
                    }
                // Sino, es una trama nueva tipo REQUEST
                }else{
                    if(idTramaRx != rxTrama.campos.IdTrama){ //compara la id de trama con la recibida. Si son distintas ingresa
                        idTramaRx = rxTrama.campos.IdTrama; //almacena la nueva id de trama
                        switch(rxTrama.campos.Comando){
                        	//Enviar temperatura
                        	case REQ_TEMP:
                                app_comando_reqTemp(&rxTrama.campos.Dato[0]);
                                break;
                            //Mandar un PONG
                        	case PING:
                                net_SendResponse(PONG, buffer);
                                break;
                            //On Off Led
                        	case REQ_LED:
                        		app_comando_reqLed(&rxTrama.campos.Dato[0]);
                        		break;
                            default:

                            break;
                        }
                    } else {
                        net_sendtrama();            //si coincide el id de trama con el almacenado significa que el que transmitió la trama no recibió un ack y paso el tiempo de espera por lo que hay que enviarle una trama con lo que solicitó
                    }
                }
            }
        }
    }
}

/**
 * @brief:función para procesar trama si el dato recibido está listo. Se prende LedVerde
 * si está listo el dato, se lo levanta y analiza.
 *
 */
void net_prossesNetwork(void){
    if(rf_rxDataReady()){
        led_On(LedVerde);                                           //Se prende led verde si rf_rxDataReady()=1
        rf_getRxPacket(&rxTrama.buffer[0], TAM_TRAMA);              //Recogemos lo recibido en rxTrama
        bsp_DelayMs(50);
        led_Off(LedVerde);
        net_analizarTrama();                                        //acá se modifican las banderas ack, not_ack y timeout_ack

    }

    if(netFlags.ack_wait && netFlags.not_ack){
//si la cantidad de retransmiciones se superó, dejo de esperar un ack y aviso que es un error poniendo en 1 la flag ack_error
        if(req_retrans >= MAX_CANT_RETRANS){
            netFlags.ack_wait = 0;
            netFlags.ack_error = 1;
        } else {//sino se superó la cantidad de retransmiciones, envío trama. Si se envía,
            if (net_sendtrama()){

                netFlags.ack_wait = 1; //la flag de ack_wait se pone en 1 para avisar que va a esperar una respuesta
                netFlags.not_ack = 0;//y resetea la bandera de not_ack para avisar que no recibió uno
                timeout_ack = 500;  //contador para esperar por un ack por determinada cant de tiempo
                req_retrans++;
            }
        }
    }
}
/**
 * @brief función para calcular checksum
 * @param *buffer trama en forma de buffer
 * @return el resultado de la suma
 */
uint8_t chksum(uint8_t *buffer){
    uint8_t suma = 0;
    uint8_t i;

    for(i=0; i<(TAM_TRAMA-1); i++)
        suma +=  *(buffer+i);
    return suma;
}

void net_1ms_ISR(void){
    if(timeout_ack){
        timeout_ack--;
        if(!timeout_ack)
            netFlags.not_ack = 1;
    }
}



