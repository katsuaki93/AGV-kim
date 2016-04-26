#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"

#include "driverlib/debug.h"
#include "driverlib/adc.h"


static void init_rotation(void){

    //enable pins
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);


    GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_0);
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_1);
   SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
   // ADCSequenceDisable(ADC0_BASE, 1);
    ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH3 );
    ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH3 );
    ADCSequenceStepConfigure(ADC0_BASE, 1, 2,ADC_CTL_CH6 );
    ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_CH6 |ADC_CTL_IE|ADC_CTL_END);
    //SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ADCSequenceEnable(ADC0_BASE, 1);
    //enable interrupt, may need to enable processor interrupt:
    IntMasterEnable();
    ADCIntClear(ADC0_BASE,1);
    ADCIntEnable(ADC0_BASE,1);
    IntEnable(INT_ADC0SS1);
    //clear interrupt flag
       ADCIntClear(ADC0_BASE, 1);
       ADCProcessorTrigger(ADC0_BASE, 1);


}
