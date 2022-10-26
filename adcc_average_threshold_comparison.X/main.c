
/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

/* WDT operating mode ? WDT Disabled */
#pragma config WDTE = OFF
/* Low-voltage programming enabled, RE3 pin is MCLR */
#pragma config LVP = ON

#include <xc.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define _XTAL_FREQ 1000000UL
#define ADC_VREF                (3.3f)
#define ADC_MAX_COUNT           (4095)

void initSysClk(void);
void initUsart(void);
void initPort(void);
void writeUsart(uint8_t txData);
void putch(char txData);
void initAdc(void);
uint16_t readAdcvalue(uint8_t channel);
void initInterrupt(void);
void adcc_ThresholdISR(void);
void __interrupt() INTERRUPT_InterruptMAnger(void);
volatile uint16_t err = 0;


void initSysClk(void) {
    OSCCON1 = 0x60; /* set HFINTOSC as new oscillator source */
    OSCFRQ = 0x00; /* set HFFRQ to 1 MHz */
}

void initUsart(void) {
    /* Transmit Enable */
    U1CON0bits.TXEN = 1;
    /*Baud Rate Generator is high speed with 4 baud clocks per bit */
    U1CON0bits.BRGS = 1;
    /*Asynchronous 8-bit UART mode*/
    U1CON0bits.MODE = 0;
    /* Baud rate 9600 */
    U1BRG = 25; //see Page 559 for more details
    /*Serial port enabled*/
    U1CON1bits.ON = 1;
}

void initPort(void) {
    /* Configure RF0 as output. */
    TRISFbits.TRISF0 = 0;
    /* RF0 is TX1 */
    RF0PPS = 0x20;
    /*set pin RA0 as InPut*/
    TRISAbits.TRISA0 = 1;
    /*set pin RA0 as analog*/
    ANSELAbits.ANSELA0 = 1;
}

void writeUsart(uint8_t txData) {
    while (0 == PIR4bits.U1TXIF) {
        ;
    }
    U1TXB = txData;
}

void putch(char txData) {
    writeUsart(txData);
}

void initAdc(void) {
    /*ADCC Module Enable*/
    ADCON0bits.ADON = 1;
    /* select FRC clock*/
    ADCON0bits.ADCS = 1;
    /*result right justified*/
    ADCON0bits.ADFM = 1;
    /*channel number to connect to VSS*/
    ADPCH = 0b111100;
    /*Average Mode*/
    ADCON2bits.MD = 2;
    /*repeat Value */
     ADRPT = 8;
    /*result is rightshifted by 3*/
    ADCON2bits.CRS = 3;
    /* error calculation Method*/
    ADCON3bits.CALC = 1;
    
    /*upper threshold 3V*/
    ADUTH = 1241;
    /*lower threshold 1V*/
    ADLTH = -1241;
    
    /* setpoint 2V*/
    ADSTPT = 2482;

    /* mode: ADERR < ADLTH or ADERR > ADUTH*/
    ADCON3bits.TMD = 4;
    
     // Clear the ADC interrupt flag
    PIR1bits.ADIF = 0;
    
    /*clear ADCC Threshold Interrupt Flag*/
    PIR2bits.ADTIF = 0;
    
    /*Enable ADCC threshold interrupt*/
    PIE2bits.ADTIE = 1;
        
}

void initInterrupt(void) {
    /*enable global interrupt*/
    INTCON0bits.GIE = 1;  
}

void adcc_ThresholdISR(void) {
    /*real error value*/
    err = (uint16_t)((ADERRH << 8) + ADRESL);
    
     printf("Threshold crossed\n\r");
      /* clear interrupt flag*/
     PIR2bits.ADTIF = 0;        
}

void __interrupt(irq(IRQ_ADT)) INTERRUPT_InterruptMAnger(void) {

    if(PIE2bits.ADTIE == 1 && PIR2bits.ADTIF == 1)
    {
        adcc_ThresholdISR();
    }

}
uint16_t readAdcvalue(uint8_t channel) {
    
    ADPCH = channel;
    /*Start conversion */
    ADCON0bits.ADGO = 1;
    while (ADCON0bits.ADGO) {
        ;
    }
    return ((uint16_t) ((ADRESH << 8) + ADRESL));
}

void main(void) {

    float voltage = 0;
    uint16_t potiValue = 0;
    initSysClk();
    initPort();
    initUsart();
    initAdc();
    initInterrupt();
 
    while (1) {   
        
        potiValue = readAdcvalue(0); 
        voltage = (float) (potiValue * ADC_VREF / ADC_MAX_COUNT); 
        printf("ADC count =  %d,Voltage = %.2f\n\r", potiValue,voltage);
        if (ADSTATbits.LTHR) {
            printf("Lower Threshold Crossed\n\r");    
        }    
         if (ADSTATbits.UTHR) {
            printf("Upper Threshold Crossed\n\r");
        }
        __delay_ms(1000);
    }
}
