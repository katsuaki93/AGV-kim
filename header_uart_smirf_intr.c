#include "header_uart_smirf_intr.h"

// TIPS
// http://stackoverflow.com/questions/9210528/split-string-with-delimiters-in-c

void init_uart(void) {

	SysCtlClockSet(
			SYSCTL_SYSDIV_1 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN
					| SYSCTL_XTAL_16MHZ);
	// Är vaken när proccesorn är vaken
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinConfigure(GPIO_PB0_U1RX); // PB0 Recieve
	GPIOPinConfigure(GPIO_PB1_U1TX);

	//LED
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //enable GPIO port for LED
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_2); //enable pin for LED PF2

	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(), 9600,
			(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	UARTFIFOLevelSet(UART1_BASE, UART_FIFO_TX4_8, UART_FIFO_RX4_8);	// WHEN TO TRIGGER INTERRUPTS

	IntMasterEnable(); //enable processor interrupts
	UARTEnable(UART1_BASE);
	IntEnable(INT_UART1); //enable the UART interrupt
	UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT); //only enable RX and TX interrupts

}

void runPostMan(void) {

	// Tokenize globalBuffer
	const char s[2] = ",";
	char *token;

	int iterator = 0;

	//   get the first token * /
	token = strtok(globalBuffer, s);
	//char str[16];

	//   walk through other tokens * /
	while (token != NULL) {


		driveOrderz[iterator] = atoi(token);
		iterator++;
		token = strtok(NULL, s); // reset token
	}

	// SEND driveOrderz OVER UART
	// %id,dis,angle,0,!
	/*
	iterator = 0;
	for (iterator = 0; iterator < 3; iterator++) {
		sprintf(str, "%i,", driveOrderz[iterator]);
		cPtr = str;

		while (*cPtr) {
			UARTCharPut(UART1_BASE, *cPtr);
			++cPtr;
		}
	}
	UARTCharPut(UART1_BASE, '!');
	*/

	// Clear memoryaddresses

	/*
	 memset(&str[0], 0, sizeof(str));
	 str[0] = 0;

	 memset(&driveOrderz[0], 0, sizeof(driveOrderz));
	 driveOrderz[0] = 0;
	 */
	//UARTCharPut(UART1_BASE, '%');
	//UARTCharPut(UART1_BASE, 'G');

	drive_me();
	memset(&globalBuffer[0], 0, sizeof(globalBuffer));
	driveOrderz[0] = 0;
}

void UART1IntHandler(void) {

	uint32_t ui32Status;
	ui32Status = UARTIntStatus(UART1_BASE, true); //get interrupt status
	UARTIntClear(UART1_BASE, ui32Status); //clear the asserted interrupts

	char c;

	char startUp[] = "Messsage recieved \n";

	cPtr = startUp;
	bool flush =false;
	// READ DATA FROM % to ! INTO STRING
	while (UARTCharsAvail(UART1_BASE)) //loop while there are chars
	{
		c = UARTCharGetNonBlocking(UART1_BASE);
		//c = UARTCharGet(UART1_BASE); // gets stuck in this function until there's a byte in the fifo
		//UARTCharPutNonBlocking(UART1_BASE, c); //echo character

		if (c == '%') {
			globalReadFlag = true;
			flush = false;
			iIt = 0;
		} else if (c == '!') {
			/*
			while (*cPtr) {
				UARTCharPut(UART1_BASE, *cPtr);
				++cPtr;
			}
			*/

			globalReadFlag = false;
			if(!flush)
			runPostMan();
			// break;
		} else if (globalReadFlag == true) {
			globalBuffer[iIt] = c;
			iIt++;
			if (c == '#'){
				clearOrderz();
				memset(&globalBuffer[0], 0, sizeof(globalBuffer));
				globalReadFlag = false;
				flush= true;
			}




		}




	} // END WHILE

	// LED BLINK
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2); //blink LED
	SysCtlDelay(SysCtlClockGet() / (40)); //delay ~1 msec
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0); //turn off LED
	SysCtlDelay(SysCtlClockGet() / (40)); //delay ~1 msec

}
