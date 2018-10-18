#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "MHEtLiveRGB.h"
#include "Arduino.h"

#define set(x) |= (1<<x) 
#define clr(x) &=~(1<<x) 
#define inv(x) ^=(1<<x)

#define BLUE_CLEAR (pinlevelB &= ~(1 << BLUE)) // map BLUE to PB2
#define GREEN_CLEAR (pinlevelB &= ~(1 << GREEN)) // map BLUE to PB2
#define RED_CLEAR (pinlevelB &= ~(1 << RED)) // map BLUE to PB2
#define PORTB_MASK  (1 << PB0) | (1 << PB1) | (1 << PB2)
#define BLUE PB2
#define GREEN PB1
#define RED PB0


unsigned char MHEtLivePWMcompare[3]; 
volatile unsigned char MHEtLivePWMcompbuff[3];

void MHEtLiveRGBBegin() {                

  pinMode(2, OUTPUT); 
  pinMode(1, OUTPUT); 
  pinMode(0, OUTPUT); 
  //CLKPR = (1 << CLKPCE);        // enable clock prescaler update
  //CLKPR = 0;                    // set clock to maximum (= crystal)


  MHEtLivePWMcompare[0] = 0x00;// set default PWM values
  MHEtLivePWMcompare[1] = 0x00;// set default PWM values
  MHEtLivePWMcompare[2] = 0x00;// set default PWM values
  MHEtLivePWMcompbuff[0] = 0x00;// set default PWM values
  MHEtLivePWMcompbuff[1] = 0x00;// set default PWM values
  MHEtLivePWMcompbuff[2] = 0x00;// set default PWM values
  
#ifdef TIFR
  TIFR = (1 << TOV0);           // clear interrupt flag
#endif
#ifdef TIFR0
  TIFR0 = (1 << TOV0);           // clear interrupt flag
#endif
  #ifdef TIMSK
  TIMSK = (1 << TOV0);           // enable overflow interrupt
#endif
#ifdef TIMSK0
  TIMSK0 = (1 << TOV0);           // enable overflow interrupt
#endif
#ifdef TCCR0B
  TCCR0B = (1 << CS00);         // start timer, no prescale
#endif


  sei(); 
}

void MHEtLiveRGB(int pin,int value){
	MHEtLivePWMcompbuff[pin] = value;
}

void MHEtLiveRGBDelay(int ms) {
  while (ms) {
    _delay_ms(1);
    ms--;
  }
}

ISR (TIM0_OVF_vect) {
  static unsigned char pinlevelB=PORTB_MASK;
  static unsigned char softcount=0xFF;

  PORTB = pinlevelB;            // update outputs
  
  if(++softcount == 0){         // increment modulo 256 counter and update
                                // the compare values only when counter = 0.
    MHEtLivePWMcompare[0] = MHEtLivePWMcompbuff[0]; // verbose code for speed
    MHEtLivePWMcompare[1] = MHEtLivePWMcompbuff[1]; // verbose code for speed
    MHEtLivePWMcompare[2] = MHEtLivePWMcompbuff[2]; // verbose code for speed

    pinlevelB = PORTB_MASK;     // set all port pins high
  }
  // clear port pin on compare match (executed on next interrupt)
  if(MHEtLivePWMcompare[0] == softcount) RED_CLEAR;
  if(MHEtLivePWMcompare[1] == softcount) GREEN_CLEAR;
  if(MHEtLivePWMcompare[2] == softcount) BLUE_CLEAR;
}

