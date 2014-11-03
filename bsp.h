/**
 * @file bsp.h
 * @brief
 * @details
 * @author Agustin
 * @date 30/08/2013
 *
 */

#ifndef BSP_H_
#define BSP_H_

#include"typedef.h"


extern void* LedRojo;
extern void* LedVerde;
extern void* Sw;
extern void *Led1, *Led2;
extern void *Sw1, *Sw2, *Sw3, *Sw4;

extern void net_1ms_ISR(void);

// BSP services-----------------------------------------------------
void bsp_Init(void);
void bsp_DelayMs(uint16_t cm);

// Led services-----------------------------------------------------
void led_On(void* led);
void led_Off(void* led);

// Sw services-----------------------------------------------------
uint8_t sw_getState(void* sw);

// RF services-----------------------------------------------------
uint8_t rf_sendPacket(char addr, char *txBuffer, char size);
uint8_t rf_rxDataReady(void);
void rf_getRxPacket(char *buffer, uint8_t size);

#endif /* BSP_H_ */
