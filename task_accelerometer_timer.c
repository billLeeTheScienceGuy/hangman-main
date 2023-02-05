/*
 * task_accelerometer_timer.c
 *
 *  Created on: Oct 21, 2020
 *      Author: Eliot Pickhardt, Joe Krachey
 */
#include <main.h>

/**
 * Starts ADC Conversion every 50 ms
 */
void Task_Accelerometer_Timer(void *pvParameters)
{
    while(1)
         {
             /*
              * Start the ADC conversion
              */
             ADC14->CTL0 |= ADC14_CTL0_SC | ADC14_CTL0_ENC;

             /*
              * Delay 50mS
              */
             vTaskDelay(pdMS_TO_TICKS(50));

         }
}




