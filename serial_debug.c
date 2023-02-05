/*
 * serial_debug.c
 *
 *      Author: Eliot Pickhardt
 */

#include <stdint.h>
#include <stdbool.h>
#include "msp.h"
#include "serial_debug.h"
#include "circular_buffer.h"

volatile char Rx_String[80];
volatile uint16_t Rx_Char_Count = 0;
volatile bool ALERT_STRING = false;
Circular_Buffer *Tx_Buffer;

static void serial_debug_init_uart(void)
{
    P1->SEL0 |= BIT2 | BIT3;
    P1->SEL1 &= ~(BIT2 | BIT3);

    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST;
    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_SWRST | EUSCI_B_CTLW0_SSEL__SMCLK;

    EUSCI_A0->BRW = 13;

    EUSCI_A0->MCTLW = (0 << EUSCI_A_MCTLW_BRF_OFS) | EUSCI_A_MCTLW_OS16;

    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST;

    EUSCI_A0->IFG &= ~(EUSCI_A_IFG_RXIFG | EUSCI_A_IFG_TXIFG);

    EUSCI_A0->IE |= EUSCI_A_IE_RXIE;

    NVIC_EnableIRQ(EUSCIA0_IRQn);

    // Prime the pump -- ADD CODE
    EUSCI_A0->TXBUF = 0;
}

void serial_debug_init(void)
{
    serial_debug_init_uart();

    Tx_Buffer = circular_buffer_init(80);
}


void serial_debug_put_string(char * s)
{
    while(*s != 0){
        while(EUSCI_A0->STATW & EUSCI_A_STATW_BUSY)
        {

        }
        EUSCI_A0->TXBUF = *s;
        s++;
    }
}
//****************************************************************************
// This function is called from MicroLIB's stdio library.  By implementing
// this function, MicroLIB's putchar(), puts(), printf(), etc will now work.
// ****************************************************************************/
int fputc(int c, FILE* stream)
{
    // Busy wait while the circular buffer is full -- ADD CODE
    while(circular_buffer_full(Tx_Buffer)){
        //wait
    }

    // globally disable interrupts
    __disable_irq();
    // add the character to the circular buffer  -- ADD CODE
    circular_buffer_add(Tx_Buffer, c);

    // globally disable interrupts
      __enable_irq();

    // Enable Tx Empty Interrupts  -- ADD CODE
    EUSCI_A0->IE |= EUSCI_A_IE_TXIE;

    return 0;
}
//****************************************************************************
// UART interrupt service routine
// ****************************************************************************/
void EUSCIA0_IRQHandler(void)
{
    char c;
    // Check for Rx interrupts
    if(EUSCI_A0->IFG & EUSCI_A_IFG_RXIFG)
    {
        Rx_String[Rx_Char_Count] = EUSCI_A0->RXBUF;
        Rx_Char_Count++;
        if((EUSCI_A0->RXBUF == 0x0A) || (EUSCI_A0->RXBUF == 0x0D)){
            ALERT_STRING = true;
        }
    }
    // Check for Tx Interrupts
    if (EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG)
    {
        //Check to see if the Tx circular buffer is empty
        if(circular_buffer_empty(Tx_Buffer))
        {
            // Disable Tx Interrupts -- ADD CODE
            EUSCI_A0->IE &= ~EUSCI_A_IE_TXIE;
        }
        else
        {
            // Get the next char from the circular buffer -- ADD CODE
            c = circular_buffer_remove(Tx_Buffer);
            // Transmit the character -- ADD CODE
            EUSCI_A0->TXBUF = c;
        }
    }
}
