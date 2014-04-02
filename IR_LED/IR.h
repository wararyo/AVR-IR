/*
 * IR.h
 *
 * Created: 2014/03/30 22:35:21
 *  Author: wararyo
 */ 
#ifndef IR_H_
#define IR_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
//#include "../../library/Serial.h"

#define cbi(addr,bit)     addr &= ~(1<<bit)
#define sbi(addr,bit)     addr |=  (1<<bit)
#define tbi(addr,bit)	  addr ^=  (1<<bit)
#define wait(ms) _delay_ms(ms)

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

#define IR_T 70// 562/8 560μs
#define IR_CAREER 26 //26μs
#define IR_DUTY 9 // 26/3 9μs

volatile int IR_count = 0;
volatile char IR_data[154];//16+8+(4*32)+1

ISR ( TIMER0_COMPA_vect ){
	if(IR_data[IR_count] & 0b00000001){
		sbi(PWM_CONTROL_A,7);
	}
	else{
		cbi(PWM_CONTROL_A,7);
	}
	//TIMER_COMP = IR_T;// * (IR_data[IR_count] >> 1);
	//sbi(TIMER_INTERRUPT,TIMER_INTERRUPT_BIT);
	IR_count++;
	if(IR_data[IR_count] == 0) {
		//tbi(PORTD,PD7);
		cbi(PWM_CONTROL_A,7);
		cbi(TIMER_INTERRUPT,TIMER_INTERRUPT_BIT);
		IR_count = 0;
		cbi(IR_LED_PORT,IR_LED_PIN);
	}
}

void IR_initialize(){
	TIMER_CONTROL_A = 0b00000010;//OC0A切断 CTC
	TIMER_CONTROL_B = 0b00000010;//1/8 8μs/1カウント
	TIMER_COMP = IR_T;
	
	sbi(IR_LED_DDR,IR_LED_PIN);//output
	cbi(IR_LED_PORT,IR_LED_PIN);
	
	PWM_CONTROL_A = 0b00000010;
	PWM_CONTROL_B = 0b00011001;
	PWM_TOP = IR_CAREER;
	PWM_COMP = IR_DUTY;
	
	//sei();
}

//IR_data一個の構造
//0ビット目でHIGHかLOWか
//残り7ビットでその状態の長さを表す
//ex) 0b00000100 LOWの状態を2カウント続ける　0b00000011 HIGHの状態を1カウント続ける

int add_data_raw(char mvalue,int count,char length){
	mvalue &= 0b00000001;//2以上の場合は1に
	if(!length){cbi(PORTD,PD7); return count;}else sbi(PORTD,PD7);
	IR_data[0] = 5;//(length << 1) | mvalue;
	return count++;
}

int add_data(char mvalue,int count){
	if(mvalue){
		count = add_data_raw(1,count,1);
		count = add_data_raw(0,count,3);
	}
	else{
		count = add_data_raw(1,count,1);
		count = add_data_raw(0,count,1);
	}
	return count;
}

void IR_send(int customer,char data){

	int count = 0;
	count = add_data_raw(1,count,16);
	count = add_data_raw(0,count,8);
	
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
	count = add_data_raw(1,count,1);
	
	while(count < 154){
		IR_data[count++] = 0;
	}
	

	
	
	//sbi(PORTD,PD6);
	sbi(TIMER_INTERRUPT,TIMER_INTERRUPT_BIT);
}


#endif /* IR_H_ */