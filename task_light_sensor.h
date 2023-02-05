/*
 * task_light_sensor.h
 *
 *  Created on: Apr 26, 2022
 *      Author: Eliot Pickhardt
 */

#ifndef TASK_LIGHT_SENSOR_H_
#define TASK_LIGHT_SENSOR_H_

#include "main.h"

/**
 * Task in charge of reading data from the light sensor using I2C
 */
void Task_Light_Sensor(void *pvParameters);


#endif /* TASK_LIGHT_SENSOR_H_ */
