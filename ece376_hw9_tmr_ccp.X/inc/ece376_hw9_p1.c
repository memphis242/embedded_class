/*
 * File:   ece376_hw9_p1.c
 * Author: Abdullah Almosalami
 *
 * Created on November 7, 2021, 12:28 AM
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
#include "ccp.h"
#include "timer.h"


#define TOGGLE_RC0  (LATCbits.LATC0 ^= 0x1)


// as global variables...
static uint16_t ccp_compare_val_final = 2840u;
static volatile uint8_t compare_count = 0u;		// Volatile because this is changed in the ISR
static volatile uint8_t reset_comp_count_flag = 0x00;	// Volatile because this is changed in the ISR
// Just to conveniently represent which button was last pressed, I'll use an enum data type...
enum buttons_t { BUTTON0, BUTTON1, BUTTON2, BUTTON3, BUTTON4, BUTTON5, BUTTON6, BUTTON7 };
// Just to start, I'll make RB7 as the last button pressed...
enum buttons_t LAST_BUTTON_PRESSED = BUTTON7;


/******************************************************************************
 * Interrupt Routine
 * 
 * TODO: 
 */
void __interrupt() isr(void){
      
    /* ****************************************************
     * CCP1 INTERRUPT
     * ****************************************************
     */
    if(CCP1_IF_BIT && CCP1_INT_ENABLE_BIT){
        
        if(compare_count == 0) {	// If this is now the last interrupt cycle...
			reset_comp_count_flag = 0x01;
			if((LAST_BUTTON_PRESSED == BUTTON0) || (LAST_BUTTON_PRESSED == BUTTON1)){
                CCP1_SET_COMP_VAL(0xFFFF);	// These are for notes A0 and A1, which require chained interrupts
            }
            
			TOGGLE_RC0;	// Toggle RC0
			
		} else {
			compare_count--;
			if(compare_count == 0) {	// Next interrupt will be last interrupt cycle...
				CCP1_SET_COMP_VAL(ccp_compare_val_final);
			}
		}
		
		// Clear flag
		CLEAR_CCP1_IF;
        
    }
    
    return;
}


void main(void) {
    
    // Configure I/O
	TRISC &= 0xFE;	// RC0 as output, leave the rest as whatever they were before
	// And then for the buttons, PORTB...
	ADCON1 |= 0x0F;	// Just have all pins be digital
	TRISB = 0xFF;
    PORTB = 0x00;
	
	
	// **************************************************
	// whatever the LCD stuff you want...
	// **************************************************

	// Now Timer1 and CCP1...
	Timer1_Init_Default(0x7FFF);
	
	// Now turn on Timer1 and unmask the CCP1 interrupt flag, allow peripheral interrupts, and global interrupts...
	Timer1_Enable();
	ENABLE_CCP1_INTERRUPT;
	ENABLE_PERIPHERAL_INTERRUPTS;
	ei();

    
	reset_comp_count_flag = 0x00;
	while(1){
        
		if(reset_comp_count_flag) {		// Only update once reset_comp_count_flag has been set...
			
			// update LAST_BUTTON_PRESSED to whatever is the most recent button pressed...
			// NOTE this checks RB0 first then down to RB7. If more than one button is pressed, the highest button is chosen
			// e.g., if RB1 and RB4 are both pressed, then RB4 is chosen...
			if(PORTBbits.RB0) {
				LAST_BUTTON_PRESSED = BUTTON0;
			} else if(PORTBbits.RB1) {
				LAST_BUTTON_PRESSED = BUTTON1;
			} else if(PORTBbits.RB2) {
				LAST_BUTTON_PRESSED = BUTTON2;
			} else if(PORTBbits.RB3) {
				LAST_BUTTON_PRESSED = BUTTON3;
			} else if(PORTBbits.RB4) {
				LAST_BUTTON_PRESSED = BUTTON4;
			} else if(PORTBbits.RB5) {
				LAST_BUTTON_PRESSED = BUTTON5;
			} else if(PORTBbits.RB6) {
				LAST_BUTTON_PRESSED = BUTTON6;
			} else if(PORTBbits.RB7) {
				LAST_BUTTON_PRESSED = BUTTON7;
			} else {
				// do nothing, leave LAST_BUTTON_PRESSED as is...
			}
			
			// Now update ccp_compare_val_final and compare_count based on LAST_BUTTON_PRESSED
			switch(LAST_BUTTON_PRESSED) {
				
				// Since Timer1 and CCP1 are only 16-bit at most, they can only go as high as 65,536 counts
				// To do more than that, I will use multiple successive interrupts to toggle...
				case BUTTON0:
					// Note A0 is 27.5 Hz, or ~36,363,363 ns period, which is 363,636 100ns cycles
					// We toggle twice as fast, or at half the period, so 181,818 100ns cycles, or
					// 65,536 * 2 + 50,746
					ccp_compare_val_final = 50746u;	
					compare_count = 2u;
					CCP1_SET_COMP_VAL(0xFFFF);
					break;
					
				case BUTTON1:
					// Note A1 is 50 Hz, or ~18,181,818 ns period, which is 181,818 100ns cycles
					// For toggling at half periods, that is 90,909 100ns cycles, or
					// 65,536 * 1 + 25373
					ccp_compare_val_final = 25373u;
					compare_count = 1u;
					CCP1_SET_COMP_VAL(0xFFFF);
					break;
					
				case BUTTON2:
					// You get the idea now...
					ccp_compare_val_final = 45454u;
					compare_count = 0;
					CCP1_SET_COMP_VAL(ccp_compare_val_final);
					break;
					
				case BUTTON3:
					ccp_compare_val_final = 22727u;
					compare_count = 0u;
					CCP1_SET_COMP_VAL(ccp_compare_val_final);
					break;
					
				case BUTTON4:
					ccp_compare_val_final = 11363u;
					compare_count = 0u;
					CCP1_SET_COMP_VAL(ccp_compare_val_final);
					break;
				
				case BUTTON5:
					ccp_compare_val_final = 5681u;
					compare_count = 0u;
					CCP1_SET_COMP_VAL(ccp_compare_val_final);
					break;
					
				case BUTTON6:
					ccp_compare_val_final = 2840u;
					compare_count = 0u;
					CCP1_SET_COMP_VAL(ccp_compare_val_final);
					break;
					
				case BUTTON7:
					ccp_compare_val_final = 1420u;
					compare_count = 0u;
					CCP1_SET_COMP_VAL(ccp_compare_val_final);
					break;
			
			}
			
			reset_comp_count_flag = 0x00;	// Reset this flag
			
		}
		
	}
    
    return;
}
