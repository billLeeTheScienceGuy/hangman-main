/*
 * opt3001.c
 *
 *  Created on: Mar 14, 2021
 *      Author: younghyunkim, Eliot Pickhardt
 */

#include "opt3001.h"
#include "math.h"
#include "ece353.h"

/******************************************************************************
 * Initialize the opt3001 ambient light sensor on the MKII.  This function assumes
 * that the I2C interface has already been configured to operate at 100KHz.
 ******************************************************************************/

void opt3001_init(void)
{
    // Initialize OPT3001
    i2c_write_16(OPT3001_SLAVE_ADDRESS, CONFIG_REG, 0xC610);
}


/******************************************************************************
 * Returns the current ambient light in lux
 *  ******************************************************************************/
float opt3001_read_lux(void)
{
    uint16_t readVal;
    uint16_t exponent;
    float mantissa;


    // Read the Result register of OPT3001 and convert into Lux, then return.
    readVal = i2c_read_16(OPT3001_SLAVE_ADDRESS, RESULT_REG);
    exponent = (readVal >> 12);
    mantissa = (readVal &= 0x0FFF);

    return 0.01*pow(2, exponent)*mantissa;

}

/**
 * Reads the value in the register without doing operations on it
 */
uint16_t opt3001_read_sensor_value(void){
    return i2c_read_16(OPT3001_SLAVE_ADDRESS, RESULT_REG);
}

