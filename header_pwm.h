#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"


#include "driverlib/pwm.h"

 void init_pwm(unsigned int pwmPeriod){



	 // SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC |   SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);


	   //Config pwm
	   //Configure PWM Clock to match system
	   SysCtlPWMClockSet(SYSCTL_PWMDIV_1);

	   // Enable the peripherals used by this program.
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	   // SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

	    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);  //The Tiva Launchpad has two modules (0 and 1).

	    //Configure PB6,PB7,PE4 and PE5 Pins as PWM
	    GPIOPinConfigure(GPIO_PC5_M0PWM7);
	    GPIOPinConfigure(GPIO_PB5_M0PWM3);



	    GPIOPinConfigure(GPIO_PB4_M0PWM2);
	    GPIOPinConfigure(GPIO_PB6_M0PWM0);


	    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_5);
	    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_4);
	    GPIOPinTypePWM(GPIO_PORTC_BASE, GPIO_PIN_5);
	    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6);
	    //Configure PWM Options
	    //PWM_GEN_0 Covers M0PWM0 and M0PWM1
	    //PWM_GEN_2 Covers M0PWM4 ABD M0PWM5 See page 207 4/11/13 DriverLib doc
	    PWMGenConfigure(PWM0_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

	    PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
	    PWMGenConfigure(PWM0_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

	    //Set the Period (expressed in clock ticks)
	    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, pwmPeriod);

	    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, pwmPeriod);
	    PWMGenPeriodSet(PWM0_BASE, PWM_GEN_0, pwmPeriod);

	    //Set PWM duty-50% (Period /2) TOTALT ONODIG KOD
	    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0,pwmPeriod/2);
	    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2,pwmPeriod/2);
	    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3,pwmPeriod/2);
	    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7,pwmPeriod/2);

	    // Enable the PWM generator
	    PWMGenEnable(PWM0_BASE, PWM_GEN_3);
	    PWMGenEnable(PWM0_BASE, PWM_GEN_1);
	    PWMGenEnable(PWM0_BASE, PWM_GEN_0);
	    // Turn on the Output pins
	    PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT | PWM_OUT_3_BIT | PWM_OUT_2_BIT| PWM_OUT_0_BIT, true);



}
