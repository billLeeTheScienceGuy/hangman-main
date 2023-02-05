/*
 * task_buzzer.c
 *
 *  Created on: Apr 26, 2022
 *      Author: Eliot Pickhardt
 */

#include "task_buzzer.h"

#define PWM     3000

/**
 * Task in charge of turning on the buzzer for .5s
 */
void Task_Buzzer(void *pvParameters){
    BaseType_t status;
    Buzzer_init();

    while(1){
        //when given the semaphore
        status = xSemaphoreTake(Sem_Buzzer, portMAX_DELAY);
        buzzer_on(); //turn buzzer on
        vTaskDelay(pdMS_TO_TICKS(500)); //wait .5s
        buzzer_off(); //turn buzzer off
    }
}

/**
 * Initializes the Buzzer for a certain duty cycle (tone)
 */
void Buzzer_init(void){
    //set P2.7 to OUTPUT
    P2->DIR |= BIT7;

    //P2.7 <--> TA0.4
    P2->SEL0 |= BIT7;
    P2->SEL1 &= ~BIT7; //selects primary module function

    //turn off timer
    TIMER_A0->CTL = 0;

    //set period of the timer (CCR0)
    TIMER_A0->CCR[0] = SystemCoreClock/PWM; //zero indexed

    //configure buzzer duty cycle
    TIMER_A0->CCR[4] = 0.5*(SystemCoreClock/PWM) - 1;

    //configure TA0.4 for RESET/SET mode
    TIMER_A0->CCTL[4] = TIMER_A_CCTLN_OUTMOD_7;

    //set clock source
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK;
}

/**
 * Turns the Buzzer On
 */
void buzzer_on(void){
    TIMER_A0->CTL &= ~TIMER_A_CTL_MC_MASK; //Stop mode

    TIMER_A0->CTL |= TIMER_A_CTL_MC__UP | TIMER_A_CTL_CLR;

}

/**
 * Turns the Buzzer Off
 */
void buzzer_off(void){

    TIMER_A0->CTL &= ~TIMER_A_CTL_MC_MASK;
}


