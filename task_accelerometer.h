/*
 * task_accelerometer.h
 *
 *  Created on: Apr 26, 2022
 *      Author: Eliot Pickhardt
 */

#ifndef TASK_ACCELEROMETER_H_
#define TASK_ACCELEROMETER_H_

#include "main.h"

extern volatile uint32_t ACC_X_VAL;
extern volatile uint32_t ACC_Y_VAL;
extern volatile uint32_t ACC_Z_VAL;

/**
 * Task in charge of accelerometer values
 */
void Task_Accelerometer(void *pvParameters);

/*
 * Configures the Accelerometer to get values from x, y, and z directions
 */
void Accelerometer_XYZ(void);

/**
 * Whenever the ADC is finished, reads data from memory and determines if the board has moved in certain ways
 */
void ADC14_IRQHandler(void);

#endif /* TASK_ACCELEROMETER_H_ */
