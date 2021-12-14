/*
 * File:   question4.c
 * Author: Abdullah Almosalami
 *
 * Created on December 14, 2021, 12:45 AM
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
#pragma config CCP2MX = PORTBE  // CCP2 MUX bit (CCP2 input/output is multiplexed with RB3)
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
#include <string.h>
#include <stdio.h>  // For sprintf()
#include "ccp.h"
#include "timer.h"


// DEFINITIONS & MACROS

// I/O
#define START_GAME_BUTTON           PORTBbits.RB0
#define GAME_ON_LED                 PORTBbits.RB7 = 1u
#define TRIS_GAME                   TRISB = 0x7F
#define CLEAR_GAME_PORT             PORTB = 0u

// Interrupt-related
#define INT0_IF                     INTCONbits.INT0IF
#define INT0_IE                     INTCONbits.INT0IE
#define INT0_RISING_EDGE            INTCON2bits.INTEDG0 = 1u


// Global variables...
volatile static uint8_t game_done_flag = 0u;

volatile static uint32_t elapsed_time = 0u;
volatile static uint32_t time1 = 0u;
volatile static uint32_t time2 = 0u;
volatile static uint32_t reaction_time = 0u;
static uint16_t num_of_seconds = 0u;
static uint16_t num_of_ms = 0u;    // Take remainder and then that / 10,000 is ms
static uint16_t num_of_us = 0u;


/******************************************************************************
 * Interrupt Routine
 * 
 * TODO: 
 */
void __interrupt() isr(void){
    
    /* ****************************************************
     * INT0 Interrupt
     * ****************************************************
     */
    if(  )
    
      
    /* ****************************************************
     * Timer1 Interrupt
     * ****************************************************
     */
    if(TMR1_IF && TMR1_ENABLE_BIT) {
        elapsed_time += 0x10000u;
        // Clear flag
        CLEAR_TMR1_IF;
    }
    
    
    /* ****************************************************
     * CCP1 INTERRUPT (Capture) --> For time2
     * ****************************************************
     */
    if(CCP1_IF_BIT && CCP1_INT_ENABLE_BIT){
        time2 = elapsed_time + (uint32_t) CCPR1;
        reaction_time = time2 - time1;
        
		
        TMR1_OFF;
        
		// Clear flag
		CLEAR_CCP1_IF;
    }
    
    /* ****************************************************
     * CCP2 INTERRUPT (Capture) --> For time1
     * ****************************************************
     */
    if(CCP2_IF_BIT && CCP2_INT_ENABLE_BIT){
        time1 = elapsed_time + (uint32_t) CCPR2;
        
		// Clear flag
		CLEAR_CCP2_IF;
    }
    
    return;
}


void main(void) {
    
    // Initialization...
    // I/O
    ADCON1bits.PCFG = 0xFu;
    TRIS_GAME;
    CLEAR_GAME_PORT;

	// Now Timer1, CCP1, and CCP2...
	Timer1_Init_Default();
    CCP1_Capture_Init_Default();
    CCP2_Capture_Init_Default();
  
    
    // Now while(1) loop...
	while(1){
        
		if(START_GAME_BUTTON) {
            elapsed_time = 0u;  // Reset elapsed_time
            TMR1_ON;
            ENABLE_PERIPHERAL_INTERRUPTS;
            ei();
            
            // Indicate game has begun
            GAME_ON_LED;
            
            
            // Await user's reaction...
            while(!game_done_flag);
            
            // Now display reaction_time!
            /* First convert reaction_time into s::ms::us --> warning: division ahead...
             * Note, at 100ns steps, it's 10,000,000 steps per second, 10,000 steps per ms, and 10 steps per us
             * Also note that
             *      - num_of_seconds will be within 0 to 429s,
             *      - num_of_ms will be within 0 and 999
             *      - num_of_us will be within 0 and 999
             */
            num_of_seconds = (uint16_t) (reaction_time / 10000000u);
            num_of_ms = (uint16_t) ((reaction_time % 10000000u) / 10000u);    // Take remainder and then that / 10,000 is ms
            num_of_us = (uint16_t) ((reaction_time % 10000000u) % 10000u) / 10u;   // Take the remainder of the ms division and / 10 is us

            // Now display the numbers!
            // Format: ---s:---ms:---us --> 16 characters
            sprintf(result_msg, "%3us:%3ums:%3uus", num_of_seconds, num_of_ms, num_of_us);   // Use the handy-dandy sprintf function
            // Onto display!
            LCD_set_cursor_position(1,1);
            for(uint8_t i=0; i<7; i++) LCD_write_data_byte_4bit(result_msg_title[i]);
            LCD_set_cursor_position(2,1);
            for(uint8_t i=0; i<16; i++) LCD_write_data_byte_4bit(result_msg[i]);
            
            
            __delay_ms(3000);
            __delay_ms(2000);
            
            // Reset game
            OUTPUT_LED = 0u;
            elapsed_time = 0u;
            LCD_clear_display();
            game_done_flag = 0x00;
            
        }
		
	}
    
    return;
}


static char * hex_to_bit_string(uint8_t hex_val){
    
    static char bit_string[9];
    // I will do: bit_string[0] = MSb, bit_string[7] = LSb
    for(uint8_t i=0; i<8; i++){
        bit_string[i] = (hex_val & (0x1 << (7-i))) ? '1' : '0';
    }
    bit_string[8] = '\0';   // Manually terminate with null character
    
    return bit_string;
    
}


//    
//#ifdef DEBUG_RESET
//    /* Ok. Need to debug why PIC is resetting...
//     * I will have the PIC print out to the display the contents
//     * of RCON and STKPTR to help determine what caused the reset...
//     */
//    // Layout of registers:
//    /* STKPTR - Stack Pointer Register -  Register 5-1 in Datasheet
//    * Default/POR: 0 0 00 0 0 0 0
//    * +-------------------------------------------------------------------------------------------------+
//    * |    bit 7   |    bit 6   |   bit 5   |   bit 4   |   bit 3   |   bit 2   |   bit 1   |   bit 0   |
//    * +-------------------------------------------------------------------------------------------------+
//    * |...STKFUL...|....STKUNF..|...UNDEF...|..........................SP...............................|
//    * +-------------------------------------------------------------------------------------------------+
//    */
//    /* RCON - Reset Control Register -  Register 4-1 in Datasheet
//    * Default/POR: 0 0 00 0 0 0 0
//    * +-------------------------------------------------------------------------------------------------+
//    * |    bit 7   |    bit 6   |   bit 5   |   bit 4   |   bit 3   |   bit 2   |   bit 1   |   bit 0   |
//    * +-------------------------------------------------------------------------------------------------+
//    * |...IPEN.....|...SBOREN...|...undef...|...~RI.....|....~TO....|....~PD....|....~POR...|...~BOR....|
//    * +-------------------------------------------------------------------------------------------------+
//    */
//    
//    char rcon_reg_string[16] = "RCON: ";
//    char stkptr_reg_string[16] = "STKPTR: ";
//    strcat(rcon_reg_string, hex_to_bit_string( (uint8_t)RCON ) );
//    strcat(stkptr_reg_string, hex_to_bit_string( (uint8_t)STKPTR ) );
//    
//    LCD_set_cursor_position(1,1);
//    for(uint8_t i=0; i<14; i++) LCD_write_data_byte_4bit(rcon_reg_string[i]);
//    LCD_set_cursor_position(2,1);
//    for(uint8_t i=0; i<16; i++) LCD_write_data_byte_4bit(stkptr_reg_string[i]);
//    
//    // Set the ~POR and ~BOR bits so that we can tell if this is what happens with the rest next time around...
//    RCONbits.POR = 1u;
//    RCONbits.BOR = 1u;
//    
//    while(!DEBUG_BUTTON);       // Wait until user presses the debug button to continue
//    LCD_clear_display();
//    LCD_set_cursor_position(1,1);
//    __delay_ms(2000);
//    
//#endif
//   