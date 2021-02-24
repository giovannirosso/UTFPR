//Giovanni de Rosso Unruh
#include <stdint.h>
#include <stdbool.h>
// includes da biblioteca driverlib
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"

uint8_t LED_D4 = 0;
#define _24MHZ 24000000
#define _120MHZ 120000000

void main()
{

  uint32_t clock = SysCtlClockFreqSet(SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480 | SYSCTL_XTAL_25MHZ, _24MHZ); //24MHz
  // uint32_t clock = SysCtlClockFreqSet(SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480 | SYSCTL_XTAL_25MHZ, _120MHZ); //120MHz

  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // Habilita GPIO F (LED D4 = PF0)
  while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    ; // Aguarda final da habilitação

  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0); // LEDs D4 como saída
  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0);       // LEDs D4 apagados
  GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_12MA, GPIO_PIN_TYPE_STD);

  while (1)
  {

    // for (int i = 0; i < 3000000; i++)        //feito no for
    // {
    // }
    SysCtlDelay(8000000); //SysCtlDelay:
                          //1/24MHz = 41,67nS, 41,67*3= 125nS. 125nS*8.000.000 = 1 seconds
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, LED_D4);
    if (LED_D4 == 0)
    {
      LED_D4 = 1;
    }
    else
    {
      LED_D4 = 0;
    }
    //for(int i=0;i<12000000;i++){}
  }
}