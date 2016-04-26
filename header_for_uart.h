#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"




void init_uart(void){

	// SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	//  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC |   SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	 // Är vaken när proccesorn är vaken
	 SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	 SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	 SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	 SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	 GPIOPinConfigure(GPIO_PA0_U0RX);
	 GPIOPinConfigure(GPIO_PA1_U0TX);
	 GPIOPinConfigure(GPIO_PB0_U1RX);
	 GPIOPinConfigure(GPIO_PB1_U1TX);

	 GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	 GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	 UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600,
	 (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	 UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 9600,
	 (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

}



