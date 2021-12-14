/*
 * File:   question3.c
 * Author: Abdullah Almosalami
 *
 * Created on December 14, 2021, 12:05 AM
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
#define TOGGLE_BUZZER_OUTPUT            (LATCbits.LATC0 ^= 1u)

// Interrupt-Related
#define ENABLE_PERIPHERAL_INTERRUPTS    (INTCONbits.PEIE = 1u)
#define CLEAR_TMR0_IF                   (INTCONbits.T0IF = 0u)
#define ENABLE_TMR0_INTERRUPT           (INTCONbits.T0IE = 1u)
#define TMR0_IF_BIT                     (INTCONbits.T0IF)
#define TMR0_IE_BIT                     (INTCONbits.T0IE)

#define CLEAR_TMR2_IF                   (PIR1bits.TMR2IF = 0u)
#define ENABLE_TMR2_INTERRUPT           (PIE1bits.TMR2IE = 1u)
#define TMR2_IF_BIT                     (PIR1bits.TMR2IF)
#define TMR2_IE_BIT                     (PIE1bits.TMR2IE)

#define TMR0_COUNT_372HZ                53u     // This isn't quite perfectly 372 Hz...
#define TMR2_COUNT_3MS                  3u


// Timer0 Specific
#define TMR0_ON                         T0CONbits.TMR0ON = 1u;
#define TMR0_8BIT                       T0CONbits.T08BIT = 1u;
#define TMR0_16BIT                      T0CONbits.T08BIT = 0u;
#define TMR0_INTERNAL_CLOCK             T0CONbits.T0CS = 0u;
#define TMR0_USE_PRESCALER              T0CONbits.PSA = 0u;
#define TMR0_DO_NOT_USE_PRESCALER       T0CONbits.PSA = 1u;
#define TMR0_SET_PRESCALER(X)           (T0CONbits.T0PS = (X))    // USE WITH tmr0_prescaler_t!! Only eight possibilites!

// Timer2 Specific
/* T2CON - Timer2 Control Register -  Register 13-1 in Datasheet
 * Default/POR: x 0000 0 00
 * +-------------------------------------------------------------------------------------------------+
 * |    bit 7   |    bit 6   |   bit 5   |   bit 4   |   bit 3   |   bit 2   |   bit 1   |   bit 0   |
 * +-------------------------------------------------------------------------------------------------+
 * |...undef....|.....................T2OUTPS....................|...TMR2ON..|..........T2CKPS.......|
 * +-------------------------------------------------------------------------------------------------+
 * This register contains:
 *      (R/W) T2OUTPS[3:0] : Timer2 Output Postscale bits
 *          0000 = 1:1
 *          0001 = 1:2
 *              ...
 *          1111 = 1:16
 *      (R/W) TMR2ON : Timer2 On bit
 *      (R/W) T2CKPS : Timer2 Clock Prescale bits
 *          00 = 1
 *          01 = 4
 *          1x = 16
 * 
 */
#define TMR2_ON                         (T2CONbits.TMR2ON = 1u)
#define TMR2_SET_PRESCALER(X)           (T2CONbits.T2CKPS = (X))    // USE WITH tmr2_prescaler_t!! Only three possibilites!
#define TMR2_SET_POSTSCALER(X)          (T2CON |= X << 3u)    // For some reason, no complete bit-field for T2OUTPS in xc.h...
#define TMR2_SET_PERIOD(X)              (PR2 = (X))

// Enumerations
typedef enum { TMR0_PRESCALE_2, TMR0_PRESCALE_4, TMR0_PRESCALE_8, TMR0_PRESCALE_16,
               TMR0_PRESCALE_32, TMR0_PRESCALE_64, TMR0_PRESCALE_128, TMR0_PRESCALE_256 };
typedef enum { TMR2_PRESCALE_1, TMR2_PRESCALE_4, TMR2_PRESCALE_16 } tmr2_prescaler_t;


// Global and static variables
volatile static uint8_t tmr0_match_count = 0u;      // How many 25.6 us intervals have passed
volatile static uint8_t timer2_match_count = 0u;    // How many ms have passed
volatile static uint16_t adc_sample = 0u;


// Function declaration
void tmr0_init_default(void);
void tmr2_init_default(void);


// ISR
void __interrupt() isr(void) {
    
    // Timer0 Interrupt
    if(TMR0_IF_BIT && TMR0_IE_BIT) {        // Timer0 interrupt flag is set and enabled?
        
        if( adc_sample <= 10u ) {
        
            tmr0_match_count++;

            if( tmr0_match_count == TMR0_COUNT_372HZ ) {

                // Reset tmr0_match_count and toggle RC0 pin
                tmr0_match_count = 0u;
                TOGGLE_BUZZER_OUTPUT;

            }
        
        } else {
            
            tmr0_match_count = 0u;
            
        }
        
    }
    
    // Timer2 Interrupt
    if(TMR2_IF_BIT && TMR2_IE_BIT) {        // Timer2 interrupt flag is set and enabled?
        
        timer2_match_count++;
        
        if( timer2_match_count == TMR2_COUNT_3MS ) {
            
            // Reset timer2_match_count and start an ADC conversion
            timer2_match_count = 0u;
            ADC_START_CONVERSION;
            
        }
        
        CLEAR_TMR2_IF;
    }
    
    // ADC interrupt, assuming right justified ADC Result
    if(ADC_IF && ADC_INTERRUPT_ENABLE_BIT) {    // ADC interrupt flag is set and enabled?
        
        adc_sample = ADRES;
         
        ADC_CLEAR_IF;
    }
    
    
}


void main(void) {
    
    // Initialize I/O
    TRISCbits.RC0 = 0u;     // RC0 output
    // Initialize timers
    tmr0_init_default();
    tmr2_init_default();
    // Initialize ADC read random voltage on AN0 --> RA0 pin
    adc_init_default();
    
    // Enable all globally unmasked interrupts
    ENABLE_PERIPHERAL_INTERRUPTS;
    ei();
    
    // Event-handler --> I'm doing all the functionality through interrupts
    while(1) {
        
    }
    
    return;
}


// Function implementations
// I will use Timer0 for playing the 372 Hz tone
void tmr0_init_default(void) {
    
    // Set up Timer0
    TMR0_8BIT;
    TMR0_INTERNAL_CLOCK;    // 10 MHz
    TMR0_DO_NOT_USE_PRESCALER;  // Every overflow is 25.6 us
    
    // Set up interrupt-related stuff
    CLEAR_TMR0_IF;
    ENABLE_TMR0_INTERRUPT;
    
    // Turn on Timer0
    TMR0_ON;
    
}

// I will use Timer2 to count in ms
void tmr2_init_default(void) {
    
    // Set up timing
    TMR2_SET_PRESCALER(TMR2_PRESCALE_4);
    TMR2_SET_POSTSCALER(9u);
    TMR2_SET_PERIOD(249u);
    
    // Set up interrupt-related stuff
    CLEAR_TMR2_IF;
    ENABLE_TMR2_INTERRUPT;
    
    // Turn on Timer2
    TMR2_ON;
    
}
