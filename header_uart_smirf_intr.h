#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "string.h"
#include <stdio.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "driverlib/sysctl.h"
/*
 * https://e2e.ti.com/support/microcontrollers/stellaris_arm/f/471/t/203124
 */
void UART1IntHandler(void);
void init_uart(void);
void runPostMan(void);

//DEBUG
uint32_t globalINT;
char globalChar[4];
char listID[4];			// ID
char listDE[4];	  		// Degree
char listCM[4];			// CM
uint32_t id, de, cm;

int driveOrderz[3];

int iIt;
char globalBuffer[16];
char *cPtr;
bool globalReadFlag;

