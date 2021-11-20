///*
// * File:   question5.c
// * Author: abdullah
// *
// * Created on November 19, 2021, 9:38 PM
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
//#include "adc.h"
//#include "ccp.h"
//
//
//// Definitions/Macros
//
//#define _XTAL_FREQ      40000000U   // 40MHz FOSC --> Defining this will allow use of __delay_ms())
//
//#define TIMER_10MS_COUNT        50000u  // At 200ns timer clock cycle, we get 10ms
//
//
//// Global and static variables
//volatile static uint16_t y[3] = {0u};
//volatile static uint16_t x[3] = {0u};
//// The relationship: yn = 0.01xn-1 - 0.009xn-2 + 1.5yn-1 - 0.56yn-2
//// I will use index 0 for the current measurement, index 1 for the previous measurement, and so on...
//
//// Function declaration
//
//
//// ISR
//void __interrupt() isr(void) {
//    
//    // ADC interrupt, assuming right justified ADC Result
//    if(ADC_IF && ADC_INTERRUPT_ENABLE_BIT) {
//        
//        // Execute filter computation...
//        // Update state and output values
//        x[2] = x[1];
//        x[1] = x[0];
//        x[0] = ADRES;
//        
//        y[2] = y[1];
//        y[1] = y[0];
//        y[0] = 0.01*x[1] - 0.009*x[2] + 1.5*y[1] - 0.56*y[2];
//        
//        /* Computation method
//         * The above is definitely not the way to go as it's
//         *      1- Floating point math on a non-FPU processor in an ISR...
//         *      2- Mixing integers with floats (yikes)
//         *      3- Should be implemented using fixed-point integer MAC-type instructions
//         * However, I've really run out of time, so...
//         * 
//         */
//        
//        
//        ADC_CLEAR_IF;
//    }
//    
//    
//}p/
//
//
//void main(void) {
//    
//    // Initialize CCP2 in Compare Mode with Special Event Trigger, which should
//    // also initiate an ADC conversion
//    CCP2_Compare_Init_Default(TIMER_10MS_COUNT);
//    // Initialize ADC read random voltage on AN0 --> RA0 pin
//    adc_init_default();
//    
//    // Enable all globally unmasked interrupts
//    ei();
//    
//    // Event-handler --> Nothing to do here
//    while(1);
//    
//    return;
//}
//
