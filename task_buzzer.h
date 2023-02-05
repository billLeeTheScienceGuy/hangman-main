/*
 * task_buzzer.h
 *
 *  Created on: Apr 26, 2022
 *      Author: Eliot Pickhardt
 */

#ifndef TASK_BUZZER_H_
#define TASK_BUZZER_H_

#include "main.h"

/**
 * Task in charge of turning on the buzzer for .5s
 */
void Task_Buzzer(void *pvParameters);

/**
 * Initializes the Buzzer for a certain duty cycle (tone)
 */
void Buzzer_init(void);

/**
 * Turns the Buzzer Off
 */
void buzzer_off(void);

/**
 * Turns the Buzzer On
 */
void buzzer_on(void);



#endif /* TASK_BUZZER_H_ */
