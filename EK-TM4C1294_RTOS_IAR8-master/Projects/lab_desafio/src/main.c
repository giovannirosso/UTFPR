//GIOVANNI DE ROSSO UNRUH

#include "system_tm4c1294.h" // CMSIS-Core
#include "driverleds.h"      // device drivers
#include "driverbuttons.h"
#include "cmsis_os2.h" // CMSIS-RTOS
#include "stdbool.h"
#include <stdio.h>
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"

#define BUFFER_SIZE 8
#define BUTTON_1 0X00001
#define BUTTON_2 0X00002

osThreadId_t ledThread1, ledThread2, ledThread3, ledThread4, mainThread;
osMessageQueueId_t msgQueue;
uint32_t flagSelect, flags;

typedef struct msg_s
{
  float duty;
  uint8_t led;
} msg_t;

typedef struct
{
  uint8_t led_state;
} ledStruct;

void softwarePwm(uint8_t led, float dutyCycle)
{
  LEDOn(led);
  osDelay(10 * (dutyCycle / 100.f));

  LEDOff(led);
  osDelay(10 * (1.f - dutyCycle / 100.f));
}

int aux = 0;
void ledHandler(void *args)
{
  ledStruct *led = (ledStruct *)args;
  osStatus_t status;
  msg_t msg;
  while (1)
  {
    status = osMessageQueueGet(msgQueue, &msg, NULL, 0);

    if (status == osOK)
    {
      msg.led;
      msg.duty;
    }
    int cont = osKernelGetTickCount();
    if (cont - aux >= 1000)
    {
      softwarePwm(msg.led, 100);
      aux = cont;
    }
    else
    {
      softwarePwm(msg.led, 0);
    }
  }
}

int selectedLed = 16;
int selectedIntensity[4] = {50, 50, 50, 50};
void taskSelector(void *args)
{
  osStatus_t status;
  msg_t msg;

  while (1)
  {
    uint8_t flagSelect = osThreadFlagsWait(BUTTON_1 | BUTTON_2, osFlagsWaitAny, osWaitForever);

    if (flagSelect & BUTTON_1)
    {
      selectedLed = (selectedLed >> 1);
      if (selectedLed < 1)
        selectedLed = 8;

      if (selectedLed >= 1 && selectedLed <= 8)
      {
        msg_t msg = {
            .duty = selectedIntensity[selectedLed],
            .led = selectedLed,
        };
        osMessageQueuePut(msgQueue, &msg, NULL, osWaitForever);
        osThreadYield();
      }
    }

    else if (flagSelect & BUTTON_2)
    {
      selectedIntensity[selectedLed] += 10;
      if (selectedIntensity[selectedLed] > 100)
        selectedIntensity[selectedLed] = 0;
      if (selectedLed >= 1 && selectedLed <= 8)
      {
        msg_t msg = {
            .duty = selectedIntensity[selectedLed],
            .led = selectedLed,
        };
        osMessageQueuePut(msgQueue, &msg, NULL, osWaitForever);
        osThreadYield();
      }
    }
  }
}

const int debouncing_time = 250;
int last_Tick = 0;
int setPIN = 0;
void ButtonHandler()
{
  if (GPIOIntStatus(GPIO_PORTJ_BASE, true) & GPIO_PIN_0)
  {
    ButtonIntClear(USW1);
    setPIN = 1;
  }
  if (GPIOIntStatus(GPIO_PORTJ_BASE, true) & GPIO_PIN_1)
  {
    ButtonIntClear(USW2);
    setPIN = 2;
  }
  int getTick = osKernelGetTickCount();
  if (getTick - last_Tick >= debouncing_time)
  {
    if (setPIN == 1)
    {
      //BOTÃO 1
      osThreadFlagsSet(mainThread, BUTTON_1);
    }
    if (setPIN == 2)
    {
      //BOTÃO 2
      osThreadFlagsSet(mainThread, BUTTON_2);
    }
    last_Tick = getTick;
  }
}

void configButton()
{
  ButtonInit(USW1 | USW2);
  ButtonIntDisable(USW1 | USW2);
  GPIOIntRegister(GPIO_PORTJ_BASE, ButtonHandler); //HANDLE
  GPIOIntRegisterPin(GPIO_PORTJ_BASE, GPIO_PIN_0, ButtonHandler);
  ButtonIntEnable(USW1 | USW2);
}

void main(void)
{
  SystemInit();
  LEDInit(LED4 | LED3 | LED2 | LED1);

  ledStruct led_1;
  led_1.led_state = LED1;

  ledStruct led_2;
  led_2.led_state = LED2;

  ledStruct led_3;
  led_3.led_state = LED3;

  ledStruct led_4;
  led_4.led_state = LED4;

  configButton();

  osKernelInitialize();

  ledThread1 = osThreadNew(ledHandler, &led_1, NULL);
  ledThread2 = osThreadNew(ledHandler, &led_2, NULL);
  ledThread3 = osThreadNew(ledHandler, &led_3, NULL);
  ledThread4 = osThreadNew(ledHandler, &led_4, NULL);

  mainThread = osThreadNew(taskSelector, NULL, NULL);

  flags = osThreadFlagsSet(mainThread, BUTTON_1); /* A */
  flags = osThreadFlagsSet(mainThread, BUTTON_2); /* C */

  msgQueue = osMessageQueueNew(4, sizeof(msg_t), NULL); // Create message queue for up to 10 messages of type msg_t

  if (osKernelGetState() == osKernelReady)
    osKernelStart();

  while (1)
    ;
} // main
