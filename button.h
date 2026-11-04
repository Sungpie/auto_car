/*
 * button.h
 *
 * Created: 2026-06-10 오전 11:45:01
 * Author: user
 */ 

#define F_CPU 16000000UL  // 16MHz
#include <avr/io.h>       

#define  BUTTON_DDR  DDRA
#define  BUTTON_PIN  PINA   // PORTA를 읽는 register (5v:1, 0v:0)

#define  BUTTON0PIN  4      // PORTA의 4번 핀 사용

#define  BUTTON0     0      // 0번 버튼의 가상 index 
#define  BUTTON_NUMBER  1   // 버튼 개수 

#define  BUTTON_PRESS   1   // 버튼을 누르면 HIGH (Active-High)
#define  BUTTON_RELEASE 0   // 버튼을 뗀 상태 (LOW)