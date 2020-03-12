#include <stdint.h>
#include <stdbool.h>
// includes da biblioteca driverlib
//#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"

uint8_t LED_D4 = 0;

void main(){
  SysTickPeriodSet(24000000); // f = 1Hz para clock = 24MHz
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // Habilita GPIO F (LED D4 = PF0)
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)); // Aguarda final da habilitação
    
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0); // LEDs D4 como saída
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0); // LEDs D4 apagados
  GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);
  
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK); // Habilita GPIO K
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK)); // Aguarda final da habilitação
    
  GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_4); // K4 como saída
  GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_4, 0); // K4 LOW
  GPIOPadConfigSet(GPIO_PORTK_BASE, GPIO_PIN_4, GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);
  
  
  while(1){
  
    for(int i=0;i<12000000;i++){} 
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, LED_D4);
    if(LED_D4==0){
      LED_D4=1;
    }
    else{
      LED_D4=0;
    }
    //for(int i=0;i<12000000;i++){} 
  }
}