///*
// * File:   q1.c
// * Author: Abdullah Almosalami
// *
// * Created on December 13, 2021, 9:48 PM
// */
//
//
//// <editor-fold defaultstate="collapsed" desc="PIC18F4620 Configuration Bit Settings">
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
//// </editor-fold>
//
//
//#include <xc.h>
//#include <stdint.h>
//
//
//// Definitions/Macros
//
//#define _XTAL_FREQ      40000000U   // 40MHz FOSC --> Defining this will allow use of __delay_ms())
//
//// I/O for this problem
//#define RB0_PRESSED     PORTBbits.RB0
//#define RB0_NOT_PRESSED !PORTBbits.RB0
//#define RB1_PRESSED     PORTBbits.RB1
//#define RB2_PRESSED     PORTBbits.RB2
//#define RB3_PRESSED     PORTBbits.RB3
//
//
//// Enumerations
//enum input_buttons { B0, B1, B2, B3, NO_BUTTON };
//
//
//// Global and static variables
//static enum input_buttons button_pressed = NO_BUTTON;
//
//
//// ISR
//void __interrupt() isr(void) {
//    
//        
//    
//}
//
//
//void main(void) {
//    
//    // Initialize I/O
//    ADCON1bits.PCFG = 0xF;  // All analog port pins are set to digital
//    TRISB = 0xFFu;  // PORTB input
//    TRISC = 0x00u;  // PORTC output
//    
//    
//    // Main loop
//    while(1) {
//        
//        // Increment PORTC to generate random output once user presses button
//        while( RB0_NOT_PRESSED ) PORTC = (PORTC + 1) % 0xFu;
//        
//        // Once user presses RB0, game begins
//        while( PORTC != 0x00u ) {
//            
//            // Check which button is pressed, if any
//            if( RB0_PRESSED )       button_pressed = B0;
//            else if( RB1_PRESSED )  button_pressed = B1;
//            else if( RB2_PRESSED )  button_pressed = B2;
//            else if( RB3_PRESSED )  button_pressed = B3;
//            else                    button_pressed = NO_BUTTON;
//            
//            if( button_pressed != NO_BUTTON ) {
//                
//                switch( button_pressed ) {
//                    
//                    case B0:
//                        PORTC ^= 0b0011u;    // Toggle RC0 and RC1
//                        break;
//                        
//                    case B1:
//                        PORTC ^= 0b0111u;    // Toggle RC0, RC1, and RC2
//                        break;
//                        
//                    case B2:
//                        PORTC ^= 0b1110u;    // Toggle RC1, RC2, and RC3
//                        break;
//                        
//                    case B3:
//                        PORTC ^= 0b1100u;   // Toggle RC2 and RC3
//                        break;
//                        
//                    case NO_BUTTON:
//                        break;
//                    
//                }
//                
//            }
//            
//        }
//        
//        // Do some game-success sequence
//        __delay_ms(500);
//        PORTC = 0b0001u;
//        __delay_ms(250);
//        PORTC = 0b0010u;
//        __delay_ms(250);
//        PORTC = 0b0100u;
//        __delay_ms(250);
//        PORTC = 0b1000u;
//        __delay_ms(250);
//        
//        PORTC = 0x00u;
//        __delay_ms(1000);
//        
//    }
//    
//    return;
//}
//
//
//// Function implementations
