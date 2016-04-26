#include "inc/tm4c123gh6pm.h"

#include "driverlib/pin_map.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/adc.h"
#include "driverlib/interrupt.h"
// fr[n ping
#include "inc/hw_timer.h"
#include "driverlib/fpu.h"
#include <math.h>
#include <stdio.h>
#include "driverlib/timer.h"






static void init_ultrasonic(void){


SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
     	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
     	  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
     	  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
     	  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
     		  	  //Settings for timers
     			 SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
     			 SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
     			 SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
     				 TimerConfigure(TIMER0_BASE, TIMER_CFG_ONE_SHOT_UP);
     				 TimerConfigure(TIMER1_BASE, TIMER_CFG_ONE_SHOT_UP);
}
