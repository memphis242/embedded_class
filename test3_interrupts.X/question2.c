/*
 * File:   ece376_hw9_p1.c
 * Author: Abdullah Almosalami
 *
 * Created on November 7, 2021, 12:28 AM
 * 
 * ****************************************************************************
 * The Assigned Problem
 * ****************************************************************************
 * Using Timer1 and CCP1 in Compare mode, have the PIC
 * generate an accurate set of frequencies (to the closest 100ns) based on
 * button presses. For example, RB0 pressed --> RC0 outputs note A0; RB1
 * pressed --> RC0 outputs note A1; and so on...
 * 
 * ****************************************************************************
 * My Solution
 * ****************************************************************************
 * I use the enumerated variable LAST_BUTTON_PRESSED to indicate which button
 * was pressed. This acts as the "state" of this machine. This variable is
 * updated in the while(1) loop. The actual toggling of RC0 is done in the
 * interrupt routine servicing the CCP1 interrupt. The trick was setting the
 * CCPR1 register with the right value to get the required frequency output.
 * Two of the frequencies, A0 and A1, had periods longer than what Timer1 could
 * even count to. So the compare_count and reset_comp_count_flag variables
 * were utilized to account for this (i.e., to help chain successive compare
 * interrupts). If you go through the code, it should make sense if you follow
 * how these state variables change through code execution.
 * If it doesn't make sense, contact me! I'd be happy to explain :D.
 * 
 */


// <editor-fold defaultstate="collapsed" desc="CONFIGURATION REGISTERS">

// CONFIG1H
#pragma config OSC = HSPLL      // Oscillator Selection bits (HS oscillator, PLL enabled (Clock Frequency = 4 x FOSC1))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (00C000-00FFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

// </editor-fold>


#include <xc.h>
#include <stdint.h>


/* Set up compare match to produce 687Hz, 770Hz, 1209Hz, and 1336Hz
	 * 		- 687 Hz amounts to toggling about every 7278 100ns cycles
	 *		- 770 Hz : 6494 100ns cycles
	 *		- 1209 Hz : 4136 100ns cycles
	 *		- 1336 Hz : 3743 100ns cycles
	 */
#define COMPARE_VAL_687HZ		7278u
#define COMPARE_VAL_770HZ		6494u
#define COMPARE_VAL_1209HZ		4136u
#define COMPARE_VAL_1336HZ		3743u

#define NOTE_200MS_COUNT_MAX	2000000u	// 200ms is equal to 2,000,000 100ns cycles


// Global and/or static variables
volatile static uint32_t note_200ms_timer_count = 0x00000000u;

volatile static uint8_t note_done = 0x01u;
static uint8_t RB1_pressed = 0x00u;
static uint8_t RB5_pressed = 0x00u;


void __interrupt() isr(void) {
	
	// CCP1 interrupt
	if(PIR1bits.CCP1IF && PIE1bits.CCP1IE) {
		if(!note_done) {	// If the 200ms hasn't passed up already...
		
			if(RB1_pressed) {	// RB1 pressed so we're playing 687Hz on RC0
				note_200ms_timer_count += COMPARE_VAL_687HZ;
			} else {	// RB5 must have been pressed instead and we're playing 770Hz on RC1
				note_200ms_timer_count += COMPARE_VAL_770HZ;
			}
			
			if(note_200ms_timer_count >= NOTE_200MS_COUNT_MAX){
				note_done = 0x01;	// 200ms have passed
				LATCbits.LATC0 = 0u;
				LATCbits.LATC1 = 0u;
			}
			
			LATCbits.LATC0 ^= 1u;
			
		}
		
		// Clear flag
		PIR1bits.CCP1IF = 0u;
	}

	// CCP2 interrupt
	if(PIR2bits.CCP2IF && PIE2bits.CCP2IE) {
		if(!note_done) {	// If the 200ms hasn't passed up already...
			
			LATCbits.LATC1 ^= 1u;
			
		}
		
		// Clear flag
		PIR2bits.CCP2IF = 0u;
	}
	
}


void main(void) {
	
	// Initialize I/O
	TRISC &= ~(0x03u);		// RC0, RC1 outputs
	TRISB |= 0x12u;			// RB1, RB5 inputs
	ADCON1 |= 0x0F;			// No ADC use here
	// Clear ports just in case...
	PORTB = 0x00u;
	PORTC = 0x00u;
	
	// Initialize Timer1 and Timer3 which will be used with CCP1 and CCP2 respectively
	T1CON = 0x80;		// 16-bit, prescaler of 1, tmr1 osc off, internal clock, tmr1 off
	T3CON = 0x88;		// Like-wise; also, set CCP1 for Timer1, CCP2 for Timer3
	
	// Initialize CCP1 and CCP2
	CCP1CON = 0xB;	// 1011 for Compare mode /w Special Trigger Event --> i.e., interrupt + reset timer
	CCP2CON = 0xB;	// Like-wise
	
	
	 // Will start off at default of 0xFFFFu for both CCP compare registers
	 CCPR1 = 0xFFFFu; 
	 CCPR2 = 0xFFFFu;
	
	
	// Interrupts --> I am just using CCP interrupts...
	// First clear interrupt flags just in case
	PIR1bits.CCP1IF = 0u;
	PIR2bits.CCP2IF = 0u;
	// Then enable interrupts
	PIE1bits.CCP1IE = 1u;
	PIE2bits.CCP2IE = 1u;
	// Now enable all unmasked peripheral interrupts
	INTCONbits.PEIE = 1u;
	// Now enable all globally unmasked interrupts
	INTCONbits.GIE = 1u;
	
	
	// Event handling
	while(1) {
		
		if(PORTBbits.RB1) {
			RB1_pressed = 0x01u;
			note_done = 0x00u;
			note_200ms_timer_count = 0u;
			
			CCPR1 = COMPARE_VAL_687HZ;
			CCPR2 = COMPARE_VAL_1209HZ;		
			
		} else if(PORTBbits.RB5) {
			RB5_pressed = 0x01u;
			note_done = 0x00u;
			note_200ms_timer_count = 0u;
			
			CCPR1 = COMPARE_VAL_770HZ;
			CCPR2 = COMPARE_VAL_1336HZ;	
			
		}
		
	}
	
	
}