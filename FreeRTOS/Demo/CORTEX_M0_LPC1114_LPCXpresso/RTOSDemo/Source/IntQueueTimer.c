/*
 * FreeRTOS Kernel V10.0.1
 * Copyright (C) 2017 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo includes. */
#include "IntQueueTimer.h"
#include "IntQueue.h"

/* Hardware includes. */
#include "lpc11xx.h"

/* The two timer frequencies. */
#define tmrTIMER_2_FREQUENCY	( 2000UL )
#define tmrTIMER_3_FREQUENCY	( 2001UL )

/* The priorities for the two timers.  Note that a priority of 0 is the highest
possible on Cortex-M devices. */
#define tmrMAX_PRIORITY				( 0UL )
#define trmSECOND_HIGHEST_PRIORITY ( tmrMAX_PRIORITY + 1 )

void vInitialiseTimerForIntQueueTest( void )
{
	/* Enable AHB clock for GPIO and 16-bit timers. */
	LPC_SYSCON->SYSAHBCLKCTRL |= ( 7 << 6 );

	/* Interrupt and reset on MR0 match. */
	LPC_TMR16B0->MCR = 0x03;
	LPC_TMR16B1->MCR = 0x03;

	/* Configure the frequency of the interrupt generated by MR0 matches. */
	LPC_TMR16B0->MR0 = SystemCoreClock / tmrTIMER_2_FREQUENCY;
	LPC_TMR16B1->MR0 = SystemCoreClock / tmrTIMER_3_FREQUENCY;

	/* Don't generate interrupts until the scheduler has been started.
	Interrupts will be automatically enabled when the first task starts
	running. */
	taskDISABLE_INTERRUPTS();

	/* Set the timer interrupts to be above the kernel.  The interrupts are
	assigned different priorities so they nest with each other. */
	NVIC_SetPriority( TIMER_16_0_IRQn, trmSECOND_HIGHEST_PRIORITY );
	NVIC_SetPriority( TIMER_16_1_IRQn, tmrMAX_PRIORITY );

	/* Enable the timer interrupts. */
	NVIC_EnableIRQ( TIMER_16_0_IRQn );
	NVIC_EnableIRQ( TIMER_16_1_IRQn );

	/* Start the timers. */
	LPC_TMR16B0->TCR = 0x01;
	LPC_TMR16B1->TCR = 0x01;
}
/*-----------------------------------------------------------*/

void TIMER16_0_IRQHandler(void)
{
	/* Clear the interrupt. */
	LPC_TMR16B0->IR = LPC_TMR16B0->IR;

	/* Call the  standard demo int queue timer function for this first timer. */
	portEND_SWITCHING_ISR( xFirstTimerHandler() );
}
/*-----------------------------------------------------------*/

void TIMER16_1_IRQHandler(void)
{
	/* Clear the interrupt. */
	LPC_TMR16B1->IR = LPC_TMR16B1->IR;

	/* Call the standard demo int queue timer function for this second timer. */
	portEND_SWITCHING_ISR( xSecondTimerHandler() );
}
/*-----------------------------------------------------------*/
