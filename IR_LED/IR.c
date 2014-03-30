/*
 * IR.c
 *
 * Created: 2014/03/30 20:39:15
 *  Author: wararyo
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "IR.h"

#define cbi(addr,bit)     addr &= ~(1<<bit)
#define sbi(addr,bit)     addr |=  (1<<bit)
#define tbi(addr,bit)	  addr ^=  (1<<bit)
#define wait(ms) _delay_ms(ms)

/*const char IR_FORMAT_NEC = 0;
const char IR_FORMAT_AEHA = 1;
const char IR_FORMAT_SONY = 2;

char IR_format = IR_FORMAT_NEC;*/

int IR_count = 0;
char IR_data[154];//16+8+(4*32)+1

ISR ( TIMER0_COMPA_vect ){
	sbi(PORTD,PD6);
	if(IR_data[IR_count]){
		sbi(PWM_CONTROL_B,0);
	}
	else{
		cbi(PWM_CONTROL_B,0);
	}
	IR_count++;
	if(IR_count >= 154) {
		cbi(TIMER_INTERRUPT,TIMER_INTERRUPT_BIT);
		IR_count = 0;
		cbi(PORTD,PD6);
	}
}

void IR_initialize(){
	TIMER_CONTROL_A = 0b00000010;//OC0A切断 CTC
	TIMER_CONTROL_B = 0b00000010;//1/8 8μs/1カウント
	TIMER_COMP = IR_T;
	
	sbi(IR_LED_DDR,IR_LED_PIN);//output
	cbi(IR_LED_PORT,IR_LED_PIN);
	
	PWM_CONTROL_A = 0b10000010;
	PWM_CONTROL_B = 0b00011001;
	PWM_TOP = IR_CAREER;
	PWM_COMP = IR_DUTY;
	
	sei();
}

int add_data(char mvalue,int count){
	if(mvalue){
		IR_data[count] = 1;count++;
		for(char ii=0;ii < 3;ii++){IR_data[count] = 0;count++;}
	}
	else{
		IR_data[count] = 1;count++;
		IR_data[count] = 0;count++;
	}
	return count;
}


void IR_send(int customer,char data){

	int count = 0;
	for(char i=0;i < 16;i++){
		IR_data[count] = 1;
		count++;
	}
	for(char i=0;i < 8;i++){
		IR_data[count] = 0;
		count++;
	}
	
	for(char i=0;i < 16;i++){
		count = add_data(((customer >> (15 - i)) & 0x0001),count);//カスタマーコード　上位ビットからiビット目が1の時
	}
	for(char i=0;i < 8;i++){
		count = add_data(((data >> (7 - i)) & 0x01),count);
		/*if((data >> (7 - i)) & 0x01){
			IR_data[count] = 1;count++;
			IR_data[count] = 0;count++;
		}
		else{
			IR_data[count] = 1;count++;
			for(char ii=0;ii < 3;ii++){IR_data[count] = 0;count++;}
		}*/
	}
	for(char i=0;i < 8;i++){
		count = add_data(((~data >> (7 - i)) & 0x01),count);
		/*if((~data >> (7 - i)) & 0x01){
			IR_data[count] = 1;count++;
			IR_data[count] = 0;count++;
		}
		else{
			IR_data[count] = 1;count++;
			for(char ii=0;ii < 3;ii++){IR_data[count] = 0;count++;}
		}*/
	}
	IR_data[count] = 1;
	count++;
	
	while(count < 154){
		IR_data[count] = 0;
		count++;
	}
	
				
	//sbi(PORTD,PD6);
	sbi(TIMER_INTERRUPT,TIMER_INTERRUPT_BIT);
}