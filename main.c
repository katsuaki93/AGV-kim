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
#include "header_for_uart.h"
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











short int running = 0;
unsigned int pwmPeriod = 15000;
double distanceR =0;
double distanceL=0;
double distance = 0;
double target_distance = 0;

int newPulseR=0;
int newPulseL=0;
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
	       volatile uint8_t value=0;
	       void TrigUp(void);
	       void TrigDown(void);
	       void TrigUp2(void);
	       void TrigDown2(void);
	       void TrigUp3(void);
	       void TrigDown3(void);














void delayMS(int ms) {
    SysCtlDelay( (SysCtlClockGet()/(3*1000))*ms ) ;
}
void motor_v(int dir, int speed) // insert speed between 0 and 9 along with the direction, 1 - forward, 0 - stop, -1 - backwards
{
	unsigned int pwmNow = pwmPeriod/10 *speed;
	if (speed == 0)
		{
		pwmNow=1;
		}
	if (dir== -1)
	{
		PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7,1);
		PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3,pwmNow);
	}
	else if (dir==0)
	{
		PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7,pwmPeriod-5);
		PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3,pwmPeriod-5);
	}
	else if (dir == 1)
	{
		PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7,pwmNow);
		PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3,1);

	}
	  PWMGenEnable(PWM0_BASE, PWM_GEN_3);
	  PWMGenEnable(PWM0_BASE, PWM_GEN_1);
	  PWMGenEnable(PWM0_BASE, PWM_GEN_0);
}


void motor_h(int dir, int speed) // insert speed between 0 and 9 along with the direction
{
	unsigned int pwmNow = pwmPeriod/10 *speed;
	if (speed == 0)
		{
		pwmNow=1;
		}
	if (dir== -1)
	{
		PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2,pwmNow);
		PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0,1);
	}
	else if (dir==0)
	{
		PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2,pwmPeriod-5);
		PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0,pwmPeriod-5);
	}
	else if (dir == 1)
	{
		PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2,1);
		PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0,pwmNow);
	}


	PWMGenEnable(PWM0_BASE, PWM_GEN_1);

}


void move_forward(int speed) //sets the pwm:s for each side to forward
{
	motor_h(1,speed);
	motor_v(1,speed);

}
void move_backward(int speed) //sets the pwm:s for each side to forward
{
	motor_h(-1,speed);
	motor_v(-1,speed);

}
void move_stop() // Sets the pwm output to constantly one, i.e. the motors will stop.
{
	motor_h(0,0);
	motor_v(0,0);
}

void move_turn(int dir, int speed) // Fancy function for turning
{
	if (dir==1)
	{
		motor_h(-1,speed);
		motor_v(1,speed);
	}
	else if (dir==-1)
	{
		motor_h(1,speed);
		motor_v(-1,speed);
	}

}


void ADC0IntHandler(void)
{
     //clear interrupt flag
    ADCIntClear(ADC0_BASE, 1);

    ADCSequenceDataGet(ADC0_BASE,1,ui32ADC0Value);
    volatile uint32_t ADCR;
   	volatile uint32_t ADCL;


     //calculate average
     ADCL= ( ui32ADC0Value[1] +2);
     ADCR= ( ui32ADC0Value[3] +2);
//pwmValue = pwmValue + ADCR;
//testCounter = testCounter + 1;


 	if (ADCL > 1900)
     		{

     			if (newPulseL == 1){	}
     			else
     			{
     				newPulseL = 1;
     				distanceL = distanceL + 1.39;
     				testCounter = testCounter + 1;
     			}
     		}
 			else if (ADCL<1700)
     		{
     			if (newPulseL == 0) { }
     			else
     			{
     				newPulseL = 0;
     			}
     		} else
     		{

     		}



 	if (ADCR > 2550)
     		{
     			if (newPulseR == 1){	}
     			else
     			{
     				newPulseR = 1;
     				distanceR = distanceR + 1.39;
     				testCounter2 = testCounter2 + 1;
     			}
     		}
 			else if (ADCR < 2000)
     		{
     			if (newPulseR == 0) { }
     			else
     			{
     				newPulseR = 0;
     			}
     		}else
     		{

     		}
 	ADCProcessorTrigger(ADC0_BASE, 1);
}



//INTERRUPT FUNCTION TRIGGER UP (ECHO START PING)
void TrigUp(void)
{


	TimerEnable(TIMER0_BASE, TIMER_A);


		 GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_3);
		 GPIOIntTypeSet(GPIO_PORTB_BASE,GPIO_PIN_3,GPIO_FALLING_EDGE);
		 GPIOIntRegister(GPIO_PORTB_BASE, TrigDown);
		 GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_3);
  }


//INTERRUPT FUNCTION FALLING DOWN (ECHO END PING)
void TrigDown(void)
{

	ui32Timer=TimerValueGet(TIMER0_BASE,TIMER_A);
	ui32Distance = (((ui32Timer) * 170) / 120000 );

		if(ui32Distance < 15) {
			GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1, GPIO_PIN_1);
			move_stop();
		}




		//counterms = counterms + 100;
	 TimerDisable(TIMER0_BASE, TIMER_A);
	 GPIOIntDisable(GPIO_PORTB_BASE, GPIO_INT_PIN_3);


}


//INTERRUPT FUNCTION TRIGGER UP (ECHO START HCSR04-1)
void TrigUp2(void){

	TimerEnable(TIMER1_BASE, TIMER_A);


			 GPIOIntClear(GPIO_PORTC_BASE, GPIO_INT_PIN_6);
			 GPIOIntTypeSet(GPIO_PORTC_BASE,GPIO_PIN_6,GPIO_FALLING_EDGE);
			 GPIOIntRegister(GPIO_PORTC_BASE, TrigDown2);
			 GPIOIntEnable(GPIO_PORTC_BASE, GPIO_INT_PIN_6);

}


//INTERRUPT FUNCTION FALLING DOWN (ECHO END HCSR04-1)
void TrigDown2(void){

	ui32Timer2=TimerValueGet(TIMER1_BASE,TIMER_A);
	ui32Distance2 = (((ui32Timer2) * 170) / 120000 );

		if(ui32Distance2 < 15) {
			GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1, GPIO_PIN_1);
			running = 1;
    		    			move_stop();
    		    			delayMS(150);
    		    		    move_turn(-1, 8);
    		    			delayMS(400);
    		    		    target_distance=9;
    		    			running = 1;
    		        			move_stop();
    		        			delayMS(150);
    		        			move_forward(9);
    		        			delayMS(450);
    		        			//target_distance=100;
		}

	//SET TIMER TO 0;

	 TimerDisable(TIMER1_BASE, TIMER_A);
	 GPIOIntDisable(GPIO_PORTC_BASE, GPIO_INT_PIN_6);

}


//INTERRUPT FUNCTION TRIGGER UP (ECHO START HCSR04-2)
void TrigUp3(void){

	TimerEnable(TIMER2_BASE, TIMER_A);

			 GPIOIntClear(GPIO_PORTD_BASE, GPIO_INT_PIN_6);
			 GPIOIntTypeSet(GPIO_PORTD_BASE,GPIO_PIN_6,GPIO_FALLING_EDGE);
			 GPIOIntRegister(GPIO_PORTD_BASE, TrigDown3);
			 GPIOIntEnable(GPIO_PORTD_BASE, GPIO_INT_PIN_6);

}


//INTERRUPT FUNCTION FALLING DOWN (ECHO END PING HCSR04-2)
void TrigDown3(void){

	ui32Timer3=TimerValueGet(TIMER2_BASE,TIMER_A);
	ui32Distance3 = (((ui32Timer3) * 170)/120000 );

		if(ui32Distance3 < 15) {
			GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1, GPIO_PIN_1);
			running = 1;
    		    			move_stop();
    		    			delayMS(150);
    		    		    move_turn(1, 8);
    		    			delayMS(400);
    		    		    target_distance=9;
    		     			running = 1;
    		        		        			move_stop();
    		        		        			delayMS(150);
    		        		        			move_forward(9);
    		        		        			delayMS(450);
		}

	//SET TIMER TO 0;
	//counterms = counterms + 100;
	 TimerDisable(TIMER2_BASE, TIMER_A);
	 GPIOIntDisable(GPIO_PORTD_BASE, GPIO_INT_PIN_6);

}



int main(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC |   SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	//Config uart
	    init_uart();
    //configure pwm
	    init_pwm(pwmPeriod);

    // adc config :
init_rotation();

//for ping
init_ultrasonic();



     				 TimerConfigure(TIMER2_BASE, TIMER_CFG_ONE_SHOT_UP);
     				 ui32Period = ((SysCtlClockGet()*2.5) * 10)/2;





     				move_forward(0);
     			    			delayMS(2000);

     			    			move_stop();
    while(1)
    {

   	 counterms = 0;
   				 //Settings for timers
   			GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_3 | GPIO_PIN_2 | GPIO_PIN_1 , 0);
   			HWREG(TIMER0_BASE + TIMER_O_TAV) = 0;
   			HWREG(TIMER1_BASE + TIMER_O_TAV) = 0;
   			HWREG(TIMER2_BASE + TIMER_O_TAV) = 0;
   			TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period);
   			TimerLoadSet(TIMER1_BASE, TIMER_A, ui32Period);
   			TimerLoadSet(TIMER2_BASE, TIMER_A, ui32Period);

   		 //SEND
   			  //make pin 3 to output and low
   			  GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_2 |GPIO_PIN_3);
   			  GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2 | GPIO_PIN_3 , 0);
   			  //set pin 3 to high
   			  GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2, GPIO_PIN_2);
   			  GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_3, GPIO_PIN_3);
   			  //Delay for 5µs
   			  SysCtlDelay(((SysCtlClockGet()*2.5) * 0.000005)/ 3);
   			  GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3 , 0);
   			  //Delay extra 5µs for HCSR04
   			  SysCtlDelay(((SysCtlClockGet()*2.5) * 0.000005)/ 3);
   			  	  GPIOPinWrite(GPIO_PORTB_BASE,GPIO_PIN_2, 0);


   			  //RECIEVE (PING)
   			  GPIOIntDisable(GPIO_PORTB_BASE, GPIO_PIN_3);       //  Disable interrupt (in case it was enabled)
   			  GPIOIntClear(GPIO_PORTB_BASE, GPIO_PIN_3);
   			  GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_3);
   			  //Set pin 3 to interrupt on rising edge
   			  GPIOIntTypeSet(GPIO_PORTB_BASE,GPIO_PIN_3,GPIO_RISING_EDGE);
   			  //Interrupt function
   			  GPIOIntRegister(GPIO_PORTB_BASE, TrigUp);

   			  	  //RECIEVE (HCSR04-1)(HCSR04-2)
   			 	  GPIOIntDisable(GPIO_PORTC_BASE, GPIO_PIN_6);
   			 	  GPIOIntDisable(GPIO_PORTD_BASE, GPIO_PIN_6);  //Disable interrupt (in case it was enabled)

   			 	  GPIOIntClear(GPIO_PORTD_BASE, GPIO_PIN_6);
   			 	  GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_6);
   			 	  GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_6);
   			 	  GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_6);
   			 	  //Set pin 3 to interrupt on rising edge
   			 	  GPIOIntTypeSet(GPIO_PORTC_BASE,GPIO_PIN_6,GPIO_RISING_EDGE);
   			 	  GPIOIntTypeSet(GPIO_PORTD_BASE,GPIO_PIN_6,GPIO_RISING_EDGE);
   			 	  //Interrupt function
   			 	  GPIOIntRegister(GPIO_PORTC_BASE, TrigUp2);
   			 	  GPIOIntRegister(GPIO_PORTD_BASE, TrigUp3);
   			 	  //Enable interrupt
   			 	  GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_3);
   			 	  GPIOIntEnable(GPIO_PORTC_BASE, GPIO_INT_PIN_6);
   			 	  GPIOIntEnable(GPIO_PORTD_BASE, GPIO_INT_PIN_6);


distance = (distanceL+ distanceR)/2;

if (target_distance != 0)
{
	if (distance > target_distance)
	{
		move_stop();
		target_distance=0;
		distance = 0;
		distanceR = 0;
		distanceL = 0;
	}
}else
{

}




    	//small example that changes the output from stop to max/speed

       // delayMS(300);
      /* int i = 1;
       for( i =10;i>4;i--)
       {
        	move_backward(i);
        	delayMS(5000);
       }
       for( i =10;i>4;i--)
           {
            	move_forward(i);
            	delayMS(5000);
           }*/
     /*   move_turn(1,7);
        delayMS(5000);
        move_stop();
        delayMS(100);
        move_turn(-1,7);
        delayMS(5000);
        move_stop();
        delayMS(100);
*/

    	//körbar via blåtand


    	char c;
    	if(UARTCharsAvail(UART1_BASE))
    	{
    		c=UARTCharGet(UART1_BASE);
    		UARTCharPut(UART1_BASE, c);
    		if (c=='s')
    		{
    			running = 1;
    			move_stop();
    			delayMS(150);
    			move_forward(8);
    			//target_distance=100;
    			//UARTCharPut(UART1_BASE,('%'+'f'+'r'+'a'+'m'+'a'+'t'));

    		}
    		if (c=='q')
    		{
    			running = 0;
    			move_stop();
    			target_distance = 0;
    		}
    		if (c=='w')
    		{
    			running = 1;
    			move_stop();
    			delayMS(150);
    		    move_backward(8);
    		    //target_distance=100;
    		}
    		if (c=='a')
    		    		{
    		    			running = 1;
    		    			move_stop();
    		    			delayMS(150);
    		    		    move_turn(1, 8);
    		    		    target_distance=20;
    		    		}
    		if (c=='d')
    		    		{
    		    			running = 1;
    		    			move_stop();
    		    			delayMS(150);
    		    		    move_turn(-1,8);
    		    		    target_distance=20;
    		    		}
    	}
  	  SysCtlDelay(((SysCtlClockGet()*3) * 0.01) / 3);
    }
    //Delay to prevent mixing signals

}

