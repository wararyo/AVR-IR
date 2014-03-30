/*
 * IR.h
 *
 * Created: 2014/03/30 22:35:21
 *  Author: wararyo
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define cbi(addr,bit)     addr &= ~(1<<bit)
#define sbi(addr,bit)     addr |=  (1<<bit)
#define wait(ms) _delay_ms(ms)

#ifndef IR_H_
#define IR_H_

#define TIMER_CONTROL_A TCCR0A
#define TIMER_CONTROL_B TCCR0B
#define TIMER_COMP OCR0A
#define TIMER_INTERRUPT TIMSK0
#define TIMER_INTERRUPT_BIT OCIE0A

#define PWM_CONTROL_A TCCR1A
#define PWM_CONTROL_B TCCR1B
#define PWM_COMP OCR1AL
#define PWM_TOP ICR1  

#define IR_LED_DDR DDRB
#define IR_LED_PORT PORTB
#define IR_LED_PIN PB1 //OC1A

#define IR_T 70// 562/8
#define IR_CAREER 26
#define IR_DUTY 9 // 26/3

int IR_count;
char IR_data[154];
void IR_initialize();
void IR_send(int customer,char data);


#endif /* IR_H_ */