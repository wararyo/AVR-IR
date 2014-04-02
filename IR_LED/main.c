/*
 * IR_LED.c
 *
 * Created: 2012/09/15 22:25:25
 *  Author: wararyo
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include "IR.h"

#define cbi(addr,bit)     addr &= ~(1<<bit)
#define sbi(addr,bit)     addr |=  (1<<bit)
#define tbi(addr,bit)	  addr ^=  (1<<bit)
#define wait(ms) _delay_ms(ms)

int main(void)
{
	DDRD = 0b11111111;
	PORTD = 0b00000000;
	DDRB = 0b11111111;
	PORTB = 0b00000000;
	IR_initialize();
	sei();
    while(1)
    {
		//sbi(PWM_CONTROL_B,0);
		IR_send(0b0010000111000111,0b10010100);
		wait(5000);
    }
}