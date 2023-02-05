/*
 * task_light_sensor.c
 *
 *  Created on: Apr 26, 2022
 *      Author: Eliot Pickhardt
 */


#include "task_light_sensor.h"

/**
 * Task in charge of reading data from the light sensor using I2C
 */
void Task_Light_Sensor(void *pvParameters){
    float lux = 0.0;
    uint16_t value = 0;
    BaseType_t status;

    //initialize the i2c process and the light sensor
    i2c_init();
    opt3001_init();

    while(1){
        //Take semaphore
        status = xSemaphoreTake(Sem_Light_Sensor, portMAX_DELAY);

        lux = opt3001_read_lux(); //read light sensor value (in lux)
        value = opt3001_read_sensor_value(); //read light sensor value directly from sensor (for RNG)

        status = xQueueSend(Queue_Task_Light_Sensor, &lux, portMAX_DELAY); //Send data to game
        if(status != pdTRUE)
        {
            // should never make it here
            printf("\n\r *** Unknown error sending to Queue_Task_Light_Sensor ***\n\r");
            while(1){};
        }

        status = xQueueSend(Queue_Task_Light_Sensor, &value, portMAX_DELAY);
        if(status != pdTRUE)
        {
            // should never make it here
            printf("\n\r *** Unknown error sending to Queue_Task_Light_Sensor ***\n\r");
            while(1){};
        }

        //Give back semaphore
        status = xSemaphoreGive(Sem_Light_Sensor);

        vTaskDelay(100);
    }
}


