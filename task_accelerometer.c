/*
 * task_accelerometer.c
 *
 *  Created on: Apr 26, 2022
 *      Author: Eliot Pickhardt
 */

#include "task_accelerometer.h"


volatile uint32_t ACC_X_VAL = 0;
volatile uint32_t ACC_Y_VAL = 0;
volatile uint32_t ACC_Z_VAL = 0;

volatile int direction;
volatile bool cycle;

/**
 * Task in charge of accelerometer values
 */
void Task_Accelerometer(void *pvParameters){
    BaseType_t status;

    int acc_dir = 0;

    __enable_irq();

    while(1)
    {
        // Wait for ISR to let us know that the adc conversion has processed
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        //if the ISR tells us that a cycle of motion (tipping the board one way and tipping it back)
        if(cycle){
            acc_dir = direction;
            status = xQueueSend(Queue_Task_Accelerometer, &acc_dir, portMAX_DELAY); //send the direction of motion to game
            cycle = false;
            direction = 0;
        }
    }
}

/*
 * Configures the Accelerometer to get values from x, y, and z directions
 */
void Accelerometer_XYZ(void){
    // Configure the X direction as an analog input pin.
         //Configure P6.0 (A15) the X direction as an analog input pin
         //Find Jx.x in MKII, and P6.0 and A15 in Launchpad schematics
       P6->SEL0 |= BIT1;
       P6->SEL1 |= BIT1;

       // Configure the Y direction as an analog input pin.
       P4->SEL0 |= BIT0;
       P4->SEL1 |= BIT0;

       // Configure the Z direction as an analog input pin.
       P4->SEL0 |= BIT2;
       P4->SEL1 |= BIT2;

       // Configure CTL0 to sample 16-times in pulsed sample mode.
       ADC14->CTL0 = ADC14_CTL0_SHP | ADC14_CTL0_SHT02;

       //Indicate that this is a sequence-of-channels.
       ADC14->CTL0 |= ADC14_CTL0_CONSEQ_1;

       // Configure ADC to return 12-bit values
       ADC14->CTL1 = ADC14_CTL1_RES_2;

       // Associate the X direction analog signal with MEM[0]
       ADC14->MCTL[0] = ADC14_MCTLN_INCH_14; //set the first memory address to the 14th channel

       // Associate the Y direction analog signal with MEM[1]
       ADC14->MCTL[1] = ADC14_MCTLN_INCH_13;

       // Associate the Z direction analog signal with MEM[2]
       ADC14->MCTL[2] = ADC14_MCTLN_INCH_11;

       //end of a sequence.
       ADC14->MCTL[2] |= ADC14_MCTLN_EOS;

       // Enable interrupts in the ADC AFTER a value is written into MEM[2].
       ADC14->IER0 = ADC14_IER0_IE1; //generate interrupt as soon as conversion completes

       //Enable ADC interrupt
       NVIC_EnableIRQ(ADC14_IRQn);
       NVIC_SetPriority(ADC14_IRQn, 2);

       //turn ADC on
       ADC14->CTL0 |= ADC14_CTL0_ON;
}

/**
 * Whenever the ADC is finished, reads data from memory and determines if the board has moved in certain ways
 */
void ADC14_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken;

    //detecting changes in values
    if(ACC_X_VAL < (ADC14->MEM[0] - 150)){ //if tipped right
        if(direction == -1){ //if previously tipped left
            cycle = true; //cycle complete
        }
        else direction = 1; //otherwise, note initial motion
    }
    if(ACC_X_VAL > (ADC14->MEM[0] + 150)){ //if tipped left
        if(direction == 1){ //if previously tipped right
            cycle = true; //cycle complete
        }
        else direction = -1; //otherwise, note initial condition
    }
    ACC_X_VAL = ADC14->MEM[0]; //store x,y,z variables from memory
    ACC_Y_VAL = ADC14->MEM[1];
    ACC_Z_VAL = ADC14->MEM[2];

    //give task notification to lower half task
    vTaskNotifyGiveFromISR(
            Task_Accelerometer_Handle,
            &xHigherPriorityTaskWoken
    );

    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

