/*
 * uart0.h
 *
 * Created: 2026-06-16 오전 9:57:40
 *  Author: Cho
 */ 

#define F_CPU 16000000UL  // 16MHz
#include <avr/io.h>  // PORTA PORTB PORTD... IO관련 reg가 들어 있다.
#include <util/delay.h>  // _delay_ms _delay_us 등
#include <avr/interrupt.h>
#include <string.h>

#define QUEUE_SIZE 6
#define COMMAND_LENGTH 80
volatile int rear = 0;  // interrup에서 queue에 data를 저장하는 위치값
volatile int front = 0;  // pc_command_processing에서 가져가는 위치값
volatile unsigned char rx_buff[QUEUE_SIZE][COMMAND_LENGTH];
