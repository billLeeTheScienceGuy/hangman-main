/*
 * ece353.c
 *
 *  Created on: Jan 31, 2021
 *      Author: Eliot Pickhardt
 */

#include "ece353.h"
#include <stdbool.h>

/*
 * Initiates S1 on the MKII
 *
 */
void ece353_MKII_S1_Init(void)
{
    P5->DIR &= ~BIT1;
}

/*
 * Initiates S1 on the MKII
 *
 */
void ece353_MKII_S2_Init(void)
{
    P5->DIR &= ~BIT5;
}

/*
 * True if S1 is on, false otherwise
 *
 */
bool ece353_MKII_S1(void){
    if((P5->IN & BIT1) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*
 * True if S2 is on, false otherwise
 *
 */
bool ece353_MKII_S2(void)
{
    if((P3->IN & BIT5) == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
}
