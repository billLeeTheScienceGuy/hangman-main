/*
 * main.h
 *
 *  Created on: Apr 28, 2022
 *      Author: Eliot Pickhardt
 */

#ifndef MAIN_H_
#define MAIN_H_


#include "msp.h"
#include "msp432p401r.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <task_accelerometer.h>
#include <task_buzzer.h>
#include <task_light_sensor.h>
#include <task_mkII_s1.h>
#include <task_mkII_s2.h>
#include <i2c.h>
#include <ece353.h>
#include <opt3001.h>
#include <task_lcd.h>
#include <task_accelerometer_timer.h>
#include <word_list.h>
#include <hangman_image.h>


TaskHandle_t Task_Accelerometer_Handle;
TaskHandle_t Task_Buzzer_Handle;
TaskHandle_t Task_Light_Sensor_Handle;
TaskHandle_t Task_LCD_Handle;
TaskHandle_t Task_Accelerometer_Timer_Handle;
TaskHandle_t TaskHandle_MKII_S1;
TaskHandle_t TaskHandle_MKII_S2;

QueueHandle_t Queue_Task_Accelerometer;
QueueHandle_t Queue_Task_Light_Sensor;

SemaphoreHandle_t Sem_Light_Sensor;
SemaphoreHandle_t Sem_S1;
SemaphoreHandle_t Sem_S2;
SemaphoreHandle_t Sem_Buzzer;
SemaphoreHandle_t Sem_Buzzer2;


#endif /* MAIN_H_ */
