/*
 * task_mkII_s1.h
 *
 *  Created on: Oct 14, 2020
 *      Author: Joe Krachey
 */

#ifndef TASK_MKII_S1_H_
#define TASK_MKII_S1_H_

#include <main.h>

/******************************************************************************
 * De-bounce switch S1.  If is has been pressed, give back semaphore
 *****************************************************************************/
void Task_MKII_S1(void *pvParameters);

#endif /* TASK_MKII_S1_H_ */
