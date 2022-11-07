/* 
 * File:   UART.h
 * Author: Compu Fire
 *
 * Created on 4 de septiembre de 2022, 19:30
 */

#ifndef UART_H
#define	UART_H

#pragma config FOSC = INTRC_NOCLKOUT // Oscillator Selection bits

#include <xc.h> // include processor files - each processor file is guarded.
#include <stdint.h>

void init_UART ();  // Prototipos de funciones


#endif	/* UART_H */

