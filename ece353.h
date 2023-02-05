/*
 * ece353.h
 *
 *  Created on: Jan 31, 2021
 *      Author: Eliot Pickhardt
 */


#ifndef __ECE353_H__
#define __ECE353_H__

#include <stdbool.h>
#include <stdint.h>
#include "msp.h"


/*
 * Initiates S1 on the MKII
 *
 */
void ece353_MKII_S1_Init(void);

/*
 * Initiates S1 on the MKII
 *
 */
void ece353_MKII_S2_Init(void);

/*
 * True if S1 is on, false otherwise
 *
 */
bool ece353_MKII_S1(void);

/*
 * True if S2 is on, false otherwise
 *
 */
bool ece353_MKII_S2(void);

#endif
