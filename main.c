/*
 * Copyright (c) 2016-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ======== main_freertos.c ========
 */
#include "main.h"

TaskHandle_t Task_Accelerometer_Handle;
TaskHandle_t Task_Buzzer_Handle;
TaskHandle_t Task_Light_Sensor_Handle;
TaskHandle_t Task_LCD_Handle;
TaskHandle_t TaskHandle_MKII_S1;
TaskHandle_t TaskHandle_MKII_S2;

QueueHandle_t Queue_Task_Accelerometer;
QueueHandle_t Queue_Task_Light_Sensor;

SemaphoreHandle_t Sem_Light_Sensor;
SemaphoreHandle_t Sem_S1;
SemaphoreHandle_t Sem_S2;
SemaphoreHandle_t Sem_Buzzer;
/******************************************************************************
* main
******************************************************************************/
int main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    //initialize accelerometer and enable interrupts
    Accelerometer_XYZ();
    __enable_irq();

    //create queues
    Queue_Task_Light_Sensor = xQueueCreate(2,sizeof(float));
    Queue_Task_Accelerometer = xQueueCreate(1,sizeof(int));

    //create semaphores
    Sem_Light_Sensor = xSemaphoreCreateBinary();
    Sem_S1 = xSemaphoreCreateBinary();
    Sem_S2 = xSemaphoreCreateBinary();
    Sem_Buzzer = xSemaphoreCreateBinary();

    //create all tasks
    xTaskCreate
    (   Task_Accelerometer,
        "Task_Accelerometer",
        configMINIMAL_STACK_SIZE,
        NULL,
        2,
        &Task_Accelerometer_Handle
    );

    xTaskCreate
    (   Task_Buzzer,
        "Task_Buzzer",
        configMINIMAL_STACK_SIZE,
        NULL,
        5,
        &Task_Buzzer_Handle
    );

    xTaskCreate
    (   Task_Light_Sensor,
        "Task_Light_Sensor",
        configMINIMAL_STACK_SIZE,
        NULL,
        3,
        &Task_Light_Sensor_Handle
    );

    xTaskCreate
    (   Task_LCD,
        "Task_LCD",
        configMINIMAL_STACK_SIZE,
        NULL,
        1,
        &Task_LCD_Handle
    );

    xTaskCreate
    (   Task_MKII_S1,
        "Task_MKII_S1",
        configMINIMAL_STACK_SIZE,
        NULL,
        4,
        &TaskHandle_MKII_S1
    );

    xTaskCreate
    (   Task_MKII_S2,
        "Task_MKII_S2",
        configMINIMAL_STACK_SIZE,
        NULL,
        7,
        &TaskHandle_MKII_S2
    );
    xTaskCreate
    (   Task_Accelerometer_Timer,
        "Task_Accelerometer_Timer",
        configMINIMAL_STACK_SIZE,
        NULL,
        6,
        &Task_Accelerometer_Timer_Handle
    );

    /* Start the FreeRTOS scheduler */
    vTaskStartScheduler();

    while(1){};
    return (0);
}
//*****************************************************************************
//
//! \brief Application defined malloc failed hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationMallocFailedHook()
{
    /* Handle Memory Allocation Errors */
    while(1)
    {
    }
}

//*****************************************************************************
//
//! \brief Application defined stack overflow hook
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
    //Handle FreeRTOS Stack Overflow
    while(1)
    {
    }
}
