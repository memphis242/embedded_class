/*
 * File:   adc.c
 * Author: Abdullah Almosalami
 *
 * Created on November 15, 2021, 11:14 PM
 */


#include <xc.h>
#include "adc.h"


// Functions

//TODO: adc_init(...)

/**
 * <h3>Function: adc_init_default</h3>
 * <p>Initializes the ADC to a default I've defined as:
 * <ul>
 *          <li>AN0 channel is used (RA0)</li>
 *          <li>ADC will be enabled</li>
 *          <li>ADC interrupt will also be enabled</li>
 *          <li>ADC will use VREF+ and - pins</li>
 *          <li>PCFG = 0xA (all but AN0 to AN4 are digital)</li>
 *          <li>ADC result will be right justified</li>
 *          <li>Acquisition time will be 4 TAD</li>
 *          <li>Clock source will be FOSC/64 = 625kHz --> 1.6us period</li>
 *          <li>CCP2 will trigger ADC conversion</li>
 * </ul>
 * </p>
 * 
 * @param none
 * @return none
 * 
 */
void adc_init_default(void){
    
    // Configure channel, pin, vref, and justification
    ADC_PCFG_DEFAULT;       // Configuring pins as digital or analog --> PORTA pins used for ADC
    ADCON0_CHS_DEFAULT;     // Selecting analog channel --> AN0
    ADC_TRIS_DEFAULT;       // Setting pin directions --> RA0 as input
    ADC_DONT_USE_VREF;      // Have analog reference come from VDD and VSS
    ADC_RESULT_RIGHT_JUSTIFY;   // Right justify the 10-bit result in ADRESH and ADRESL
    
    // Configure acquisition time and adc conversion clock
    ADC_ACQ_TIME_DEFAULT;           // 4 TAD
    ADC_CLOCK_SOURCE_DEFAULT;       // FOSC/64
    
    // Enable interrupts
    ADC_CLEAR_IF;
    ADC_INTERRUPT_ENABLE;
    
    // Turn on module
    ADC_ON;
    
}

