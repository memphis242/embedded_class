/*
 * File:   ece376_hw9_p1.c
 * Author: Abdullah Almosalami
 *
 * Created on November 7, 2021, 12:28 AM
 * 
 * ****************************************************************************
 * The Assigned Problem
 * ****************************************************************************
 * Using Timer1 and CCP1 in Capture mode, measure reflex time of the user.
 * For example, have the user press a button to start the game. Then, at some
 * random point 3 to 7 seconds after, an LED lights up and the user must press
 * a button as soon as they can afterwards. Use this button press to trigger
 * the capture event and collect the needed time information.
 * 
 * ****************************************************************************
 * My Solution
 * ****************************************************************************
 * The CCP1 pin is RC2 so I'll place a button there for that. And I'll use a
 * display to show the reaction time result.
 * 
 * In addition, I will use RB0 to start the game, which will include enabling
 * Timer1 and CCP1 in Capture mode, awaiting RC1 to go high (i.e., user to press
 * the button). I will also enable Timer1 interrupts so that if it overflows,
 * I will increment a counter timer1_overflow_count. Once RC2 is pressed and a
 * CCP1 capture is triggered and an interrupt is generated, I will set the elapsed
 * time in a uint32_t time_elapsed variable (which will be able to hold up to
 * 2^32 100ns counts, which amounts to about 7 min; time_elapsed will account for
 * any timer1 overflows and the value captured in the CCPR1 register-pair.
 * This elapsed time value is then displayed.
 * 
 * NOTE: Since Timer1 is 16-bit and we are using a prescalar of 1 on an instruction
 * clock rate of 10MHz, then it overflows every ~6.5 ms! Hence, unless you're dealing
 * with Superman, expect there to be some tiemr1_overflow_count counts.
 * 
 * For now, I will not attempt to generate a random time from 3 to 7 seconds and
 * will just go with 4s using __delay_ms().
 * 
 * For translating the elapsed_time amount into seconds::milliseconds::microseconds,
 * I did some division and modulo operations, as can be seen below.
 * 
 * More can be understood by reading the code. Or contact me for questions.
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
#include <string.h>
#include <stdio.h>  // For sprintf()
#include "ccp.h"
#include "timer.h"
#include "lcd_driver.h"


#define OUTPUT_LED          LATDbits.LATD0
#define START_GAME_BUTTON   PORTBbits.RB0
#define DEBUG_BUTTON        START_GAME_BUTTON

#define DEBUG_RESET // Define this to allow the section for reset debugging to compile


// as global variables...
volatile static uint32_t elapsed_time = 0u;
volatile static uint32_t timer1_overflow_count = 0u;
volatile static uint8_t game_done_flag = 0x00u;
const static char init_msg[] = "Init success!";
const static char start_game_msg[] = "Game begun!";
const static char result_msg_title[] = "Result:";
static char result_msg[17];


// Function prototypes
static char * hex_to_bit_string(uint8_t hex_val);


/******************************************************************************
 * Interrupt Routine
 * 
 * TODO: 
 */
void __interrupt() isr(void){
      
    /* ****************************************************
     * Timer1 Interrupt
     * ****************************************************
     */
    if(TMR1_IF && TMR1_ENABLE_BIT) {
//        timer1_overflow_count++;
        elapsed_time += 0x10000u;
        // Clear flag
        CLEAR_TMR1_IF;
    }
    
    
    /* ****************************************************
     * CCP1 INTERRUPT (Capture)
     * ****************************************************
     */
    if(CCP1_IF_BIT && CCP1_INT_ENABLE_BIT){
        elapsed_time = elapsed_time + (uint32_t) CCPR1 - 30000000u;    // minus 3s 
		game_done_flag = 0x01u;
        
		// Clear flag
		CLEAR_CCP1_IF;
    }
    
    return;
}


void main(void) {
    
    // Initialization...
    /* Configure I/O
     * We're using RC2 as the CCP1 trigger input,
     * RB0 as the button-start,
     * and RD0 as the output LED
     */
	TRISC |= (1u << 2);
	ADCON1 |= 0x0F;	// Just have all pins be digital
	TRISB |= (1u << 0);
    TRISD &= ~(1u << 0);
    // Clear ports just to make behavior afterwards clear
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;
	
	// LCD Init
	LCD_Init_ECE376();
    LCD_set_cursor_position(1,1);
    for(uint8_t i=0; i<13; i++){
        LCD_write_data_byte_4bit(init_msg[i]);
    }
    OUTPUT_LED = 1u;
    __delay_ms(2000);   // Hold the display for 2s
    OUTPUT_LED = 0u;
    LCD_clear_display();
    LCD_set_cursor_position(1,1);
    
#ifdef DEBUG_RESET
    /* Ok. Need to debug why PIC is resetting...
     * I will have the PIC print out to the display the contents
     * of RCON and STKPTR to help determine what caused the reset...
     */
    // Layout of registers:
    /* STKPTR - Stack Pointer Register -  Register 5-1 in Datasheet
    * Default/POR: 0 0 00 0 0 0 0
    * +-------------------------------------------------------------------------------------------------+
    * |    bit 7   |    bit 6   |   bit 5   |   bit 4   |   bit 3   |   bit 2   |   bit 1   |   bit 0   |
    * +-------------------------------------------------------------------------------------------------+
    * |...STKFUL...|....STKUNF..|...UNDEF...|..........................SP...............................|
    * +-------------------------------------------------------------------------------------------------+
    */
    /* RCON - Reset Control Register -  Register 4-1 in Datasheet
    * Default/POR: 0 0 00 0 0 0 0
    * +-------------------------------------------------------------------------------------------------+
    * |    bit 7   |    bit 6   |   bit 5   |   bit 4   |   bit 3   |   bit 2   |   bit 1   |   bit 0   |
    * +-------------------------------------------------------------------------------------------------+
    * |...IPEN.....|...SBOREN...|...undef...|...~RI.....|....~TO....|....~PD....|....~POR...|...~BOR....|
    * +-------------------------------------------------------------------------------------------------+
    */
    
    char rcon_reg_string[16] = "RCON: ";
    char stkptr_reg_string[16] = "STKPTR: ";
    strcat(rcon_reg_string, hex_to_bit_string( (uint8_t)RCON ) );
    strcat(stkptr_reg_string, hex_to_bit_string( (uint8_t)STKPTR ) );
    
    LCD_set_cursor_position(1,1);
    for(uint8_t i=0; i<14; i++) LCD_write_data_byte_4bit(rcon_reg_string[i]);
    LCD_set_cursor_position(2,1);
    for(uint8_t i=0; i<16; i++) LCD_write_data_byte_4bit(stkptr_reg_string[i]);
    
    while(!DEBUG_BUTTON);       // Wait until user presses the debug button to continue
    LCD_clear_display();
    LCD_set_cursor_position(1,1);
    __delay_ms(2000);
    
#endif
    
    

	// Now Timer1 and CCP1...
	Timer1_Init_Default();
    CCP1_Capture_Init_Default();
    
    
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;
    
    // Now while(1) loop...
	while(1){
        
		if(START_GAME_BUTTON) {
            elapsed_time = 0u;  // Reset elapsed_time
            
            // Now turn on Timer1 and unmask peripheral interrupts and enable all unmasked interrupts...
            Timer1_Enable();
            ENABLE_PERIPHERAL_INTERRUPTS;
            ei();
            
            // Indicate on display that game has started and begin delay statement
            for(uint8_t i=0; i<11; i++) LCD_write_data_byte_4bit(start_game_msg[i]);
            __delay_ms(1000);
            LCD_clear_display();
            __delay_ms(2000);
            __delay_ms(2000);
            
            // Turn on LED!
            OUTPUT_LED = 1u;

            // Wait for user to press RC2 button for CCP1 pin
            while(!game_done_flag);
            
            // Now display time_elapsed!
            /* First convert time_elapsed into s::ms::us --> warning: division ahead...
             * Note, at 100ns steps, it's 10,000,000 steps per second, 10,000 steps per ms, and 10 steps per us
             * Also note that
             *      - num_of_seconds will be within 0 to 429s,
             *      - num_of_ms will be within 0 and 999
             *      - num_of_us will be within 0 and 999
             */
//            uint32_t elapsed_time = 0x0FAEF381u;    // (test example) 263,123,841, which should amount to 26s:312ms:384us
            uint16_t num_of_seconds = (uint16_t) (elapsed_time / 10000000u);
            uint16_t num_of_ms = (uint16_t) ((elapsed_time % 10000000u) / 10000u);    // Take remainder and then that / 10,000 is ms
            uint16_t num_of_us = (uint16_t) ((elapsed_time % 10000000u) % 10000u) / 10u;   // Take the remainder of the ms division and / 10 is us

            // Now display the numbers!
            // Format: ---s:---ms:---us --> 16 characters
            sprintf(result_msg, "%-3us:%-3ums:%-3uus", num_of_seconds, num_of_ms, num_of_us);   // Use the handy-dandy sprintf function
            // Onto display!
            LCD_set_cursor_position(1,1);
            for(uint8_t i=0; i<7; i++) LCD_write_data_byte_4bit(result_msg_title[i]);
            LCD_set_cursor_position(2,1);
            for(uint8_t i=0; i<16; i++) LCD_write_data_byte_4bit(result_msg[i]);
            
            __delay_ms(3000);
            __delay_ms(2000);
            
            // Reset game
            OUTPUT_LED = 0u;
            LCD_clear_display();
            game_done_flag = 0x00;
            Timer1_Disable();
            DISABLE_PERIPHERAL_INTERRUPTS;
            di();
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