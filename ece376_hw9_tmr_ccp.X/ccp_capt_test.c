///*
// * File:   ccp_capt_test.c
// * Author: Abdullah Almosalami
// *
// * Created on November 10, 2021, 10:46 PM
// *
// * 
// */
//
//
//// <editor-fold defaultstate="collapsed" desc="CONFIGURATION REGISTERS">
//
//// CONFIG1H
//#pragma config OSC = HSPLL      // Oscillator Selection bits (HS oscillator, PLL enabled (Clock Frequency = 4 x FOSC1))
//#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
//#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)
//
//// CONFIG2L
//#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
//#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
//#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)
//
//// CONFIG2H
//#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
//#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)
//
//// CONFIG3H
//#pragma config CCP2MX = PORTBE  // CCP2 MUX bit (CCP2 input/output is multiplexed with RB3)
//#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
//#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
//#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)
//
//// CONFIG4L
//#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
//#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
//#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))
//
//// CONFIG5L
//#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-003FFFh) not code-protected)
//#pragma config CP1 = OFF        // Code Protection bit (Block 1 (004000-007FFFh) not code-protected)
//#pragma config CP2 = OFF        // Code Protection bit (Block 2 (008000-00BFFFh) not code-protected)
//#pragma config CP3 = OFF        // Code Protection bit (Block 3 (00C000-00FFFFh) not code-protected)
//
//// CONFIG5H
//#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
//#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)
//
//// CONFIG6L
//#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-003FFFh) not write-protected)
//#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (004000-007FFFh) not write-protected)
//#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (008000-00BFFFh) not write-protected)
//#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (00C000-00FFFFh) not write-protected)
//
//// CONFIG6H
//#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
//#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
//#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)
//
//// CONFIG7L
//#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
//#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
//#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
//#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)
//
//// CONFIG7H
//#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)
//
//// </editor-fold>
//
//
//#include <xc.h>
//#include <stdint.h>
//#include <string.h>
//#include <stdio.h>  // For sprintf()
//#include "ccp.h"
//#include "timer.h"
//#include "lcd_driver.h"
//
//
//#define OUTPUT_LED          LATDbits.LATD0
//#define START_GAME_BUTTON   PORTBbits.RB0
//#define DEBUG_BUTTON        START_GAME_BUTTON
//
//#define DEBUG_RESET // Define this to allow the section for reset debugging to compile
//
//
//// as global variables...
//volatile static uint32_t elapsed_time = 0u;
//const static char init_msg[] = "Init success!";
//static char result_msg[17] = "";
//
//
//// Function prototypes
//static char * hex_to_bit_string(uint8_t hex_val);
//
//
///******************************************************************************
// * Interrupt Routine
// * 
// * TODO: 
// */
//void __interrupt() isr(void){
//      
//    /* ****************************************************
//     * Timer1 Interrupt
//     * ****************************************************
//     */
//    if(TMR1_IF && TMR1_ENABLE_BIT) {
////        timer1_overflow_count++;
//        elapsed_time += 0x10000u;
//        // Clear flag
//        CLEAR_TMR1_IF;
//    }
//    
//    
////    /* ****************************************************
////     * CCP1 INTERRUPT (Capture)
////     * ****************************************************
////     */
////    if(CCP1_IF_BIT && CCP1_INT_ENABLE_BIT){
////        elapsed_time = elapsed_time + (uint32_t) CCPR1 - 30000000u;    // minus 3s 
//////		game_done_flag = 0x01u;
////        
////		// Clear flag
////		CLEAR_CCP1_IF;
////    }
//    
//    /* ****************************************************
//     * CCP2 INTERRUPT (Capture)
//     * ****************************************************
//     */
//    if(CCP2_IF_BIT && CCP2_INT_ENABLE_BIT){
//        elapsed_time = elapsed_time + (uint32_t) CCPR1 - 30000000u;    // minus 3s 
////		game_done_flag = 0x01u;
//        
//		// Clear flag
//		CLEAR_CCP2_IF;
//    }
//    
//    return;
//}
//
//
//void main(void) {
//    
//    // Initialization...
//    /* Configure I/O
//     * We're using RB3 as the CCP2 trigger input,
//     * RB0 as the button-start,
//     * and RD0 as the output LED
//     */
//	ADCON1 |= 0x0F;	// Just have all pins be digital
//    TRISB |= (1u << 3);
//	TRISB |= (1u << 0);
//    TRISD &= ~(1u << 0);
//    // Clear ports just to make behavior afterwards clear
//    PORTB = 0x00;
//    PORTC = 0x00;
//    PORTD = 0x00;
//	
//	// LCD Init
//	LCD_Init_ECE376();
//    LCD_set_cursor_position(1,1);
//    for(uint8_t i=0; i<13; i++){
//        LCD_write_data_byte_4bit(init_msg[i]);
//    }
//    OUTPUT_LED = 1u;
//    __delay_ms(2000);   // Hold the display for 2s
//    OUTPUT_LED = 0u;
//    LCD_clear_display();
//    LCD_set_cursor_position(1,1);  
//
//	// Now Timer1 and CCP1...
//	Timer1_Init_Default();
////    CCP1_Capture_Init_Default();
//    CCP2_Capture_Init_Default();
//    
//    // Now turn on Timer1 and unmask peripheral interrupts and enable all unmasked interrupts...
//    Timer1_Enable();
//    ENABLE_PERIPHERAL_INTERRUPTS;
//    ei();
//    
//    
//    PORTB = 0x00;
//    PORTC = 0x00;
//    PORTD = 0x00;
//    
//    // Now while(1) loop...
//	while(1){
//        
//        strcat(result_msg, hex_to_bit_string( (uint8_t) (CCPR2 & 0xFF00) >> 8u ));
//        strcat(result_msg, hex_to_bit_string( (uint8_t) (CCPR2 & 0x00FF) ));
//        LCD_set_cursor_position(1,1);
//		for(uint8_t i=0; i<16; i++) LCD_write_data_byte_4bit(result_msg[i]);
//        __delay_ms(500);
//        
//    }
//    
//    return;
//}
//
//
//static char * hex_to_bit_string(uint8_t hex_val){
//    
//    static char bit_string[9];
//    // I will do: bit_string[0] = MSb, bit_string[7] = LSb
//    for(uint8_t i=0; i<8; i++){
//        bit_string[i] = (hex_val & (0x1 << (7-i))) ? '1' : '0';
//    }
//    bit_string[8] = '\0';   // Manually terminate with null character
//    
//    return bit_string;
//    
//}