/*
 * File:   question2.c
 * Author: Abdullah Almosalami
 *
 * Created on December 13, 2021, 11:30 PM
 */



// <editor-fold defaultstate="collapsed" desc="PIC18F4620 Configuration Bit Settings">
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
#include "adc.h"


// Definitions/Macros

#define _XTAL_FREQ      40000000U   // 40MHz FOSC --> Defining this will allow use of __delay_ms())

// I/O for this problem
#define RANGE_1_OUT         (PORTC = 0x00u, PORTD = 0x01u);
#define RANGE_2_OUT         (PORTC = 0x00u, PORTD = 0x03u);
#define RANGE_3_OUT         (PORTC = 0x00u, PORTD = 0x07u);
#define RANGE_4_OUT         (PORTC = 0x00u, PORTD = 0x0Fu);
#define RANGE_5_OUT         (PORTC = 0x00u, PORTD = 0x1Fu);
#define RANGE_6_OUT         (PORTC = 0x00u, PORTD = 0x3Fu);
#define RANGE_7_OUT         (PORTC = 0x00u, PORTD = 0x7Fu);
#define RANGE_8_OUT         (PORTC = 0x00u, PORTD = 0xFFu);
#define RANGE_9_OUT         (PORTC = 0x00u, PORTD = 0xFFu);
#define RANGE_10_OUT         (PORTC = 0x01u, PORTD = 0xFFu);
#define RANGE_11_OUT         (PORTC = 0x03u, PORTD = 0xFFu);
#define RANGE_12_OUT         (PORTC = 0x07u, PORTD = 0xFFu);
#define RANGE_13_OUT         (PORTC = 0x1Fu, PORTD = 0xFFu);
#define RANGE_14_OUT         (PORTC = 0x3Fu, PORTD = 0xFFu);
#define RANGE_15_OUT         (PORTC = 0x7Fu, PORTD = 0xFFu);
#define RANGE_16_OUT         (PORTC = 0xFFu, PORTD = 0xFFu);

// Logic macros
#define IN_RANGE_1(X)        ( (X > 0u) && (X < 63u) )
#define IN_RANGE_2(X)        ( (X >= 64u) && (X < 127u) )
#define IN_RANGE_3(X)        ( (X >= 128u) && (X < 191u) )
#define IN_RANGE_4(X)        ( (X >= 192u) && (X < 255u) )
#define IN_RANGE_5(X)        ( (X >= 256u) && (X < 319u) )
#define IN_RANGE_6(X)        ( (X >= 320u) && (X < 383u) )
#define IN_RANGE_7(X)        ( (X >= 384u) && (X < 447u) )
#define IN_RANGE_8(X)        ( (X >= 448u) && (X < 511u) )
#define IN_RANGE_9(X)        ( (X >= 512u) && (X < 575u) )
#define IN_RANGE_10(X)       ( (X >= 576u) && (X < 639u) )
#define IN_RANGE_11(X)       ( (X >= 640u) && (X < 703u) )
#define IN_RANGE_12(X)       ( (X >= 704u) && (X < 767u) )
#define IN_RANGE_13(X)       ( (X >= 768u) && (X < 831u) )
#define IN_RANGE_14(X)       ( (X >= 832u) && (X < 895u) )
#define IN_RANGE_15(X)       ( (X >= 896u) && (X < 959u) )
#define IN_RANGE_16(X)       ( (X >= 960u) && (X <= 1023u) )

// Enumerations


// Global and static variables



// Global function
static void BarGraph(void) {
    
    ADC_START_CONVERSION;
    uint16_t adc_in = ADRES;
    
    if( IN_RANGE_1(adc_in) )         { RANGE_1_OUT; }
    else if(IN_RANGE_2(adc_in) )     { RANGE_2_OUT; }
    else if(IN_RANGE_3(adc_in) )     { RANGE_3_OUT; }
    else if(IN_RANGE_4(adc_in) )     { RANGE_4_OUT; }
    else if(IN_RANGE_5(adc_in) )     { RANGE_5_OUT; }
    else if(IN_RANGE_6(adc_in) )     { RANGE_6_OUT; }
    else if(IN_RANGE_7(adc_in) )     { RANGE_7_OUT; }
    else if(IN_RANGE_8(adc_in) )     { RANGE_8_OUT; }
    else if(IN_RANGE_9(adc_in) )     { RANGE_9_OUT; }
    else if(IN_RANGE_10(adc_in) )    { RANGE_10_OUT; }
    else if(IN_RANGE_11(adc_in) )    { RANGE_11_OUT; }
    else if(IN_RANGE_12(adc_in) )    { RANGE_12_OUT; }
    else if(IN_RANGE_13(adc_in) )    { RANGE_13_OUT; }
    else if(IN_RANGE_14(adc_in) )    { RANGE_14_OUT; }
    else if(IN_RANGE_15(adc_in) )    { RANGE_15_OUT; }
    else if(IN_RANGE_16(adc_in) )    { RANGE_16_OUT; }
    else                             { PORTB = 0; PORTC = 0; }
    
}


// Main
void main(void) {
    
    // Initialize I/O and ADC
    adc_init_default();
    TRISC = 0xFFu;  // PORTC output
    TRISD = 0x00u;  // PORTD output
    
    
    // Main loop
    while(1) {
        
    }    
    
    return;
}

