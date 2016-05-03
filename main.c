/*################################################
 # Hardware PWM proof of concept using
 # the Tiva C Launchpad
 #
 # Started with example code by
 # lawrence_jeff found here:
 # http://forum.stellarisiti.com/topic/707-using-hardware-pwm-on-tiva-launchpad/
 #
 # Altered to use code found on section
 # 22.3 of the TivaWare Peripheral Driver
 # Library User's Guide found here:
 # http://www.ti.com/lit/ug/spmu298a/spmu298a.pdf
 #
 #
 # This example pulses three on-board LEDs
 #
 #################################################*/
#include "inc/tm4c123gh6pm.h"
#include "header_uart_smirf_intr.h"
//#include "header_for_uart.h"
#include "header_pwm.h"
#include "header_rotationsencoder.h"
#include "header_ultrasonic.h"

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

#include "driverlib/interrupt.h"

// fr[n ping
#include "inc/hw_timer.h"
#include "driverlib/fpu.h"
#include <math.h>
#include <stdio.h>
#include "driverlib/timer.h"


 void clearOrderz(void);
int degreeMe(int degree);

int latestOrderId;
uint32_t ui32Distance3;
char * DistSR;
char * DistSL;
char * DistSF;

int currentHeading = 0;

int distanceToDrive = 0;
bool running = false;

double distanceR = 0;
double distanceL = 0;
double distance = 0;
double target_distance = 0;
double target_degree = 0;

bool newOrderReady = false;
bool turning = false;
bool driving = false;

int newPulseR = 0;
int newPulseL = 0;
volatile int testCounter = 0;
volatile int testCounter2 = 0;
volatile unsigned int pwmValue = 0;

uint32_t ui32ADC0Value[4];

unsigned int heej = 0;

//pings sensors
//ping sensor config
//To compensate interrupt delay
int counterms;
int ui32Timer;
int ui32Timer2;
int ui32Timer3;
uint32_t ui32Period;
uint32_t ui32Distance;
uint32_t ui32Distance2;
uint32_t ui32Distance3;
volatile uint8_t value = 0;
void TrigUp(void);
void TrigDown(void);
void TrigUp2(void);
void TrigDown2(void);
void TrigUp3(void);
void TrigDown3(void);

int degree = 0;

//Send new sensor data to posistionsystem

void UpdateIsr(void) {

	TimerIntClear(TIMER3_BASE, TIMER_TIMA_TIMEOUT); // Clear the timer interrupt
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
	//SysCtlDelay(((SysCtlClockGet()))/ 5);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_0);

	//char *cPtr;
	char str[16];

	//HC2 (RIGHT)
	sprintf(str, "%i", ui32Distance3);
	cPtr = str;

	UARTCharPut(UART1_BASE, '$');
	UARTCharPut(UART1_BASE, ',');
	//distance3
	while (*cPtr) {
		UARTCharPut(UART1_BASE, (*cPtr));
		cPtr++;
	}

	sprintf(str, "%i", ui32Distance2);
	cPtr = str;

	UARTCharPut(UART1_BASE, ',');
	//distance2
	while (*cPtr) {
		UARTCharPut(UART1_BASE, (*cPtr));
		cPtr++;
	}
	sprintf(str, "%i", ui32Distance);
	cPtr = str;
	UARTCharPut(UART1_BASE, ',');
	//distance1
	while (*cPtr) {
		UARTCharPut(UART1_BASE, (*cPtr));
		cPtr++;
	}

	sprintf(str, "%i", currentHeading);
	cPtr = str;

	UARTCharPut(UART1_BASE, ',');

	while (*cPtr) {
		UARTCharPut(UART1_BASE, (*cPtr));
		cPtr++;
	}

	UARTCharPut(UART1_BASE, ',');
	UARTCharPut(UART1_BASE, '!');
	UARTCharPut(UART1_BASE, '\r');
	UARTCharPut(UART1_BASE, '\n');

}

void delayMS(int ms) {
	SysCtlDelay((SysCtlClockGet() / (3 * 1000)) * ms);
}

void ADC0IntHandler(void) {
	//clear interrupt flag
	ADCIntClear(ADC0_BASE, 1);

	ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
	volatile uint32_t ADCR;
	volatile uint32_t ADCL;

	//calculate average
	ADCL = (ui32ADC0Value[1] + 2);
	ADCR = (ui32ADC0Value[3] + 2);
//pwmValue = pwmValue + ADCR;
//testCounter = testCounter + 1;

	if (ADCL > 1900) {

		if (newPulseL == 1) {
		} else {
			newPulseL = 1;
			distanceL = distanceL + 1.39;
			testCounter = testCounter + 1;
		}
	} else if (ADCL < 1700) {
		if (newPulseL == 0) {
		} else {
			newPulseL = 0;
		}
	} else {

	}

	if (ADCR > 2550) {
		if (newPulseR == 1) {
		} else {
			newPulseR = 1;
			distanceR = distanceR + 1.39;
			testCounter2 = testCounter2 + 1;
		}
	} else if (ADCR < 2000) {
		if (newPulseR == 0) {
		} else {
			newPulseR = 0;
		}
	} else {

	}

	ADCProcessorTrigger(ADC0_BASE, 1);
}

void reset_distance() {
	distance = 0;
	distanceL = 0;
	distanceR = 0;
}

int degreeMe(int degree){

	// Är vi mer än
	if( (currentHeading+degree) < -180 )
		return currentHeading+degree+360;
	else if( (currentHeading+degree) > 180 )
		return currentHeading+degree-360;


	return currentHeading+degree;
}

//quick initialization of next drive order
void drive_me() {
//	if (newOrderReady == true) {

	//} else {
		//driveOrderz[0] = orderId
		//driveOrderz[2] = distance to run
		//driveOrderz[1] = degreees the agv should turn
		latestOrderId = driveOrderz[0];
		distanceToDrive = driveOrderz[2];
		degree = driveOrderz[1];
		running = false;
		//currentHeading = degreeMe(degree);
		currentHeading = degreeMe(degree);
		//skicka echo here

		UARTCharPut(UART1_BASE, '%');
		UARTCharPut(UART1_BASE, 'G');
		UARTCharPut(UART1_BASE, '\r');
		UARTCharPut(UART1_BASE, '\n');
		if (degree != 0) {
			turning = true;

			if (degree > 0) {
				move_stop();
				delayMS(150);

				target_degree = degree;
				move_turn(1, 7);
				reset_distance();

			} else {
				move_stop();
				delayMS(150);
				target_degree = degree * (-1);
				move_turn(-1, 7);
				reset_distance();
			}
		} else {
			driving = true;
			running = false;
		}

		newOrderReady = true;
	//}

}

void pollDriveOrderCheck() {
	int temp_distance = 0;

	if (turning == true) {
		if (target_degree > 0) {
			temp_distance = target_degree / 4.8;
		}
		distance = distanceR;

		if (temp_distance != 0) {
			if (distance > temp_distance) {

				target_distance = 0;

				turning = false;
				running = false;
				driving = true;
				move_stop();
				delayMS(150);
				reset_distance();
			}
		} else {
			driving = true;
		}
	} else if (driving == true) {
		if (running == false) {
			if (distanceToDrive >= 0) {
				move_forward(4);
				target_distance = distanceToDrive;
			} else if (distanceToDrive <= 0) {
				move_backward(4);
				target_distance = distanceToDrive * (-1);
			}
			running = true;
		} else {

			distance = (distanceL + distanceR) / 2;
			if (distance > 20) {
				if (distanceToDrive > 0) {
					move_forward(10);

				} else if (distanceToDrive < 0) {
					move_backward(10);

				}

			}
			if (distance >= target_distance) {
				move_stop();
				running = false;
				driving = false;
				reset_distance();
			}
		}

	} else {
		if (newOrderReady == true) {
			// %id,dis,angle,0,!
			running = false;

			char str[16];
			sprintf(str, "%i", latestOrderId);
			cPtr = str;
			UARTCharPut(UART1_BASE, '%');
			while (*cPtr) {
				UARTCharPut(UART1_BASE, (*cPtr));
				cPtr++;
			}
			UARTCharPut(UART1_BASE, (','));
			sprintf(str, "%i",degree );
			cPtr = str;
			//UARTCharPut(UART1_BASE, '%');
			while (*cPtr) {
				UARTCharPut(UART1_BASE, (*cPtr));
				cPtr++;
			}
			UARTCharPut(UART1_BASE, (','));
			sprintf(str, "%i", distanceToDrive);
			cPtr = str;
			//UARTCharPut(UART1_BASE, '%');
			while (*cPtr) {
				UARTCharPut(UART1_BASE, (*cPtr));
				cPtr++;
			}

			UARTCharPut(UART1_BASE, (','));
			UARTCharPut(UART1_BASE, ('0'));
			UARTCharPut(UART1_BASE, (','));


			UARTCharPut(UART1_BASE, ('!'));
			UARTCharPut(UART1_BASE, '\r');
			UARTCharPut(UART1_BASE, '\n');
			newOrderReady = false;
		}
	}
}

//INTERRUPT FUNCTION TRIGGER UP (ECHO START PING)
void TrigUp(void) {

	TimerEnable(TIMER0_BASE, TIMER_A);

	GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_3);
	GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_FALLING_EDGE);
	GPIOIntRegister(GPIO_PORTB_BASE, TrigDown);
	GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_3);
}

//INTERRUPT FUNCTION FALLING DOWN (ECHO END PING)
void TrigDown(void) {

	ui32Timer = TimerValueGet(TIMER0_BASE, TIMER_A);
	ui32Distance = (((ui32Timer) * 170) / 120000);

	if (ui32Distance < 15) {
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
		//move_stop();
	}

	//counterms = counterms + 100;
	TimerDisable(TIMER0_BASE, TIMER_A);
	GPIOIntDisable(GPIO_PORTB_BASE, GPIO_INT_PIN_3);

}

//INTERRUPT FUNCTION TRIGGER UP (ECHO START HCSR04-1)
void TrigUp2(void) {

	TimerEnable(TIMER1_BASE, TIMER_A);

	GPIOIntClear(GPIO_PORTC_BASE, GPIO_INT_PIN_6);
	GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_FALLING_EDGE);
	GPIOIntRegister(GPIO_PORTC_BASE, TrigDown2);
	GPIOIntEnable(GPIO_PORTC_BASE, GPIO_INT_PIN_6);

}

//INTERRUPT FUNCTION FALLING DOWN (ECHO END HCSR04-1)
void TrigDown2(void) {

	ui32Timer2 = TimerValueGet(TIMER1_BASE, TIMER_A);
	ui32Distance2 = (((ui32Timer2) * 170) / 120000);

	if (ui32Distance2 < 15) {
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);

	}

	//SET TIMER TO 0;

	TimerDisable(TIMER1_BASE, TIMER_A);
	GPIOIntDisable(GPIO_PORTC_BASE, GPIO_INT_PIN_6);

}

//INTERRUPT FUNCTION TRIGGER UP (ECHO START HCSR04-2)
void TrigUp3(void) {

	TimerEnable(TIMER2_BASE, TIMER_A);

	GPIOIntClear(GPIO_PORTD_BASE, GPIO_INT_PIN_6);
	GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_6, GPIO_FALLING_EDGE);
	GPIOIntRegister(GPIO_PORTD_BASE, TrigDown3);
	GPIOIntEnable(GPIO_PORTD_BASE, GPIO_INT_PIN_6);

}

//INTERRUPT FUNCTION FALLING DOWN (ECHO END PING HCSR04-2)
void TrigDown3(void) {

	ui32Timer3 = TimerValueGet(TIMER2_BASE, TIMER_A);
	ui32Distance3 = (((ui32Timer3) * 170) / 120000);

	if (ui32Distance3 < 15) {
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);

	}

	//SET TIMER TO 0;
	//counterms = counterms + 100;
	TimerDisable(TIMER2_BASE, TIMER_A);
	GPIOIntDisable(GPIO_PORTD_BASE, GPIO_INT_PIN_6);

}
void clearOrderz()
{

	UARTCharPut(UART1_BASE, ('%'));
	UARTCharPut(UART1_BASE, ('G'));
	UARTCharPut(UART1_BASE, '\r');
	UARTCharPut(UART1_BASE, '\n');

 	move_stop();
 	target_distance = 0;
 	target_degree = 0;
					running = false;
					driving = false;
					turning = false;
					newOrderReady = false;
					reset_distance();
					// %id,dis,angle,0,!
								running = false;

								char str[16] = "%0,0,0,#,! \r\n";

								cPtr = str;
								while (*cPtr) {
									UARTCharPut(UART1_BASE, (*cPtr));
									cPtr++;
								}

								newOrderReady = false;



}



int main(void) {
	SysCtlClockSet(
	SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	//Config uart
	init_uart();
	//configure pwm
	init_pwm(pwmPeriod);

	// adc_config :
	init_rotation();

//for ping
	init_ultrasonic();
//SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC |   SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
// sensor push
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE,
	GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER3);
	TimerConfigure(TIMER3_BASE, TIMER_CFG_PERIODIC);   // 32 bits Timer
	TimerIntRegister(TIMER3_BASE, TIMER_A, UpdateIsr);    // Registering  isr
	TimerEnable(TIMER3_BASE, TIMER_A);
	IntEnable(INT_TIMER3A);
	TimerIntEnable(TIMER3_BASE, TIMER_TIMA_TIMEOUT);

	unsigned long ulPeriod;
	//Set periodic timer to count to clock period (1 sec)
	ulPeriod = (SysCtlClockGet());
	TimerLoadSet(TIMER3_BASE, TIMER_A, ulPeriod);

	TimerConfigure(TIMER2_BASE, TIMER_CFG_ONE_SHOT_UP);
	ui32Period = ((SysCtlClockGet() * 2.5) * 10) / 2;

	move_forward(0);

	while (1) {

		counterms = 0;
		//Settings for timers
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1, 0);
		HWREG(TIMER0_BASE + TIMER_O_TAV) = 0;
		HWREG(TIMER1_BASE + TIMER_O_TAV) = 0;
		HWREG(TIMER2_BASE + TIMER_O_TAV) = 0;
		TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period);
		TimerLoadSet(TIMER1_BASE, TIMER_A, ui32Period);
		TimerLoadSet(TIMER2_BASE, TIMER_A, ui32Period);

		//SEND
		//make pin 3 to output and low
		GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3);
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3, 0);
		//set pin 3 to high
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, GPIO_PIN_2);
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_PIN_3);
		//Delay for 5µs
		SysCtlDelay(((SysCtlClockGet() * 2.5) * 0.000005) / 3);
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, 0);
		//Delay extra 5µs for HCSR04
		SysCtlDelay(((SysCtlClockGet() * 2.5) * 0.000005) / 3);
		GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, 0);

		//RECIEVE (PING)
		GPIOIntDisable(GPIO_PORTB_BASE, GPIO_PIN_3); //  Disable interrupt (in case it was enabled)
		GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_3);
		GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_3);
		//Set pin 3 to interrupt on rising edge
		GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_3, GPIO_RISING_EDGE);
		//Interrupt function
		GPIOIntRegister(GPIO_PORTB_BASE, TrigUp);

		//RECIEVE (HCSR04-1)(HCSR04-2)
		GPIOIntDisable(GPIO_PORTC_BASE, GPIO_PIN_6);
		GPIOIntDisable(GPIO_PORTD_BASE, GPIO_PIN_6); //Disable interrupt (in case it was enabled)

		GPIOIntClear(GPIO_PORTD_BASE, GPIO_PIN_6);
		GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_6);
		GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_6);
		GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_6);
		//Set pin 3 to interrupt on rising edge
		GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_RISING_EDGE);
		GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_6, GPIO_RISING_EDGE);
		//Interrupt function
		GPIOIntRegister(GPIO_PORTC_BASE, TrigUp2);
		GPIOIntRegister(GPIO_PORTD_BASE, TrigUp3);
		//Enable interrupt
		GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_3);
		GPIOIntEnable(GPIO_PORTC_BASE, GPIO_INT_PIN_6);
		GPIOIntEnable(GPIO_PORTD_BASE, GPIO_INT_PIN_6);

		//Delay to prevent mixing signals
		SysCtlDelay(((SysCtlClockGet() * 3) * 0.01) / 3);

		pollDriveOrderCheck();
	}

}

