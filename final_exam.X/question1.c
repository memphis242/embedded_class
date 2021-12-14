/*
 * File:   q1.c
 * Author: Abdullah Almosalami
 *
 * Created on December 13, 2021, 9:48 PM
 */


/*
 * File:   question1.c
 * Author: Abdullah Almosalami
 *
 * Created on November 19, 2021, 4:53 PM
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
#define TRIS_LIGHTS         (TRISC &= ~(0x07u))     // RC0, RC1, and RC2 as outputs
#define GREEN_LIGHT_ON      (LATCbits.LATC0 = 1u)
#define GREEN_LIGHT_OFF     (LATCbits.LATC0 = 0u)
#define YELLOW_LIGHT_ON     (LATCbits.LATC1 = 1u)
#define YELLOW_LIGHT_OFF    (LATCbits.LATC1 = 0u)
#define RED_LIGHT_ON        (LATCbits.LATC2 = 1u)
#define RED_LIGHT_OFF       (LATCbits.LATC2 = 0u)

// Interrupt-Related
#define ENABLE_PERIPHERAL_INTERRUPTS    (INTCONbits.PEIE = 1u)
#define CLEAR_TMR2_IF                   (PIR1bits.TMR2IF = 0u)
#define ENABLE_TMR2_INTERRUPT           (PIE1bits.TMR2IE = 1u)
#define TMR2_IF_BIT                     (PIR1bits.TMR2IF)
#define TMR2_IE_BIT                     (PIE1bits.TMR2IE)

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
typedef enum { GREEN, YELLOW, RED } traffic_light_t;
typedef enum { PRESCALE_1, PRESCALE_4, PRESCALE_16 } tmr2_prescaler_t;


// Global and static variables
volatile static uint16_t tmr2_interrupt_flag_count = 0x0000u;       // How many times the timer2 interrupt was triggered (each trigger should = 1ms)
volatile static traffic_light_t current_light = RED;        // What the current light is
volatile static uint16_t green_light_time = 5000u;          // The green light time which will vary depending on the random value input from the ADC

// Function declaration
void tmr2_init_default(void);


// ISR
void __interrupt() isr(void) {
    
    // Timer2 Interrupt
    if(TMR2_IF_BIT && TMR2_IE_BIT) {        // Timer2 interrupt flag is set and enabled?
        
        tmr2_interrupt_flag_count++;    // Increment interrupt counter (how many ms have passed)
        
        switch(current_light) {
            
            case RED:
                if(tmr2_interrupt_flag_count >= 5000u) {    // Have 5 seconds gone by?
                    // If yes, then next light yellow should go on
                    current_light = YELLOW;
                    RED_LIGHT_OFF;
                    GREEN_LIGHT_OFF;        // Just in case it was on
                    YELLOW_LIGHT_ON;
                    
                    // Reset counter
                    tmr2_interrupt_flag_count = 0x0000u;
                }
                break;
                
            case YELLOW:
                 if(tmr2_interrupt_flag_count >= 1000u) {    // Has 1 second gone by?
                    // If yes, then next light green should go on
                    current_light = GREEN;
                    YELLOW_LIGHT_OFF;
                    RED_LIGHT_OFF;        // Just in case it was on
                    GREEN_LIGHT_ON;
                    
                    // Reset counter
                    tmr2_interrupt_flag_count = 0x0000u;
                }
                break;
                
            case GREEN:
                if(tmr2_interrupt_flag_count >= green_light_time) {    // Has 5+ seconds gone by?
                    // If yes, then next light red should go on
                    current_light = RED;
                    GREEN_LIGHT_OFF;
                    YELLOW_LIGHT_OFF;        // Just in case it was on
                    RED_LIGHT_ON;
                    
                    // Reset counter
                    tmr2_interrupt_flag_count = 0x0000u;
                }
                break;
            
        }
        
        CLEAR_TMR2_IF;
    }
    
    // ADC interrupt, assuming right justified ADC Result
    if(ADC_IF && ADC_INTERRUPT_ENABLE_BIT) {
        // Update green_light_time
         green_light_time = 5000u + (ADRES * 10u);  // Approximately converts 0 to 1023 of ADRES into 0 to 10,000 for ms
                                                    // which amounts to the 0 to 10s additional time put in for green light
         
         // Just to help see what the ADRES is at...
         PORTD = (uint8_t) (ADRES & 0x0FF);
         
        ADC_CLEAR_IF;
    }
    
    
}


void main(void) {
    
    // Initialize I/O
    TRIS_LIGHTS;
    TRISD = 0u;     // I will use PORTD to display the lower byte of ADRES
    PORTC = 0u;
    PORTD = 0u;
    // Initialize Timer2
    tmr2_init_default();
    // Initialize ADC read random voltage on AN0 --> RA0 pin
    adc_init_default();
    
    // Enable all globally unmasked interrupts
    ENABLE_PERIPHERAL_INTERRUPTS;
    ei();
    
    // Event-handler --> Will just update the green_light_time variable every 1s by starting an ADC conversion for it
    while(1) {
        __delay_ms(500u);
        ADC_START_CONVERSION;
    }
    
    return;
}


// Function implementations

void tmr2_init_default(void) {
    
    // Set up timing
    TMR2_SET_PRESCALER(PRESCALE_4);
    TMR2_SET_POSTSCALER(9u);
    TMR2_SET_PERIOD(249u);
    
    // Set up interrupt-related stuff
    CLEAR_TMR2_IF;
    ENABLE_TMR2_INTERRUPT;
    
    // Turn on Timer2
    TMR2_ON;
    
}