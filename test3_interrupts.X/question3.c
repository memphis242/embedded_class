///*
// * File:   question3.c
// * Author: Abdullah Almosalami
// *
// * Created on November 19, 2021, 8:17 PM
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
//#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
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
//#include "ccp.h"
//#include "timer.h"
//
//
//// Definitions & Macros
//#define PLAYER_A_LIGHT_ON          (LATAbits.LATA2 = 1u)
//#define PLAYER_A_LIGHT_OFF         (LATAbits.LATA2 = 0u)
//#define PLAYER_B_LIGHT_ON          (LATAbits.LATA1 = 1u)
//#define PLAYER_B_LIGHT_OFF         (LATAbits.LATA1 = 0u)
//#define TRIS_LIGHTS                    (TRISA &= ~(0x06u))
//
//#define START_GAME_BUTTON              (PORTBbits.RB0)
//
//
//// Global variables...
//volatile static uint32_t elapsed_time = 0u;
//volatile static uint32_t timeA = 0u;
//volatile static uint32_t timeB = 0u;
//volatile static uint8_t game_done_flag = 0x00u;
//
//
//// Function prototypes
//
//
///******************************************************************************
// * Interrupt Routine
// *
// */
//void __interrupt() isr(void){
//      
//    /* ****************************************************
//     * Timer1 Interrupt
//     * ****************************************************
//     */
//    if(TMR1_IF && TMR1_ENABLE_BIT) {
//        elapsed_time += 0x10000u;
//        // Clear flag
//        CLEAR_TMR1_IF;
//    }
//    
//    
//    /* ****************************************************
//     * CCP1 INTERRUPT (Capture) --> For timeA
//     * ****************************************************
//     */
//    if(CCP1_IF_BIT && CCP1_INT_ENABLE_BIT){
//        timeA = elapsed_time + (uint32_t) CCPR1;
//        
//		game_done_flag = 0x01u;
//        
//		// Clear flag
//		CLEAR_CCP1_IF;
//    }
//    
//    /* ****************************************************
//     * CCP2 INTERRUPT (Capture) --> For timeB
//     * ****************************************************
//     */
//    if(CCP2_IF_BIT && CCP2_INT_ENABLE_BIT){
//        timeB = elapsed_time + (uint32_t) CCPR2;
//        
//        game_done_flag = 0x01u;
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
//     * We're using RA2 and RA1 as contestant light outputs and RB0 as a game
//     * start button. We're also using RC2 as player A's button and RC1 as player
//     * B's button; these buttons will be used with the corresponding CCP module
//     * for time capture.
//     */
//	TRISC |= 0x06u;         // RC2 for CCP1, RC1 for CCP2, inputs
//	ADCON1 |= 0x0F;         // Just have all pins be digital
//	TRISB |= (1u << 0);     // RB0 for start button
//    TRIS_LIGHTS;
//    // Clear ports just to make behavior afterwards clear
//    PORTA = 0x00u;
//    PORTB = 0x00u;
//
//	// Now Timer1, CCP1, and CCP2...
//	Timer1_Init_Default();      // 16-bit, prescaler of 1, internal osc, off
//    CCP1_Capture_Init_Default();    // Capture mode, every rising edge of the CCP1 pin, use Timer1
//    CCP2_Capture_Init_Default();    // Capture mode, every rising edge of the CCP2 pin, use Timer1
//    
//    
//    // Now while(1) loop...
//	while(1){
//        
//		if(START_GAME_BUTTON) {
//            
//            elapsed_time = 0u;  // Reset elapsed_time
//            TMR1_ON;
//            ENABLE_PERIPHERAL_INTERRUPTS;
//            ei();
//            
//            
//            // Await user's reaction...
//            while(!game_done_flag);
//            
//            if(timeA > timeB) {
//                PLAYER_A_LIGHT_ON;
//                PLAYER_B_LIGHT_OFF;
//            } else if(timeB > timeA) {
//                PLAYER_A_LIGHT_OFF;
//                PLAYER_B_LIGHT_ON;
//            } else {
//                PLAYER_A_LIGHT_OFF;
//                PLAYER_B_LIGHT_OFF;
//            }
//            
//            // Reset game
//            elapsed_time = 0u;
//            game_done_flag = 0x00;
//            
//        }
//		
//	}
//    
//    return;
//}
//
