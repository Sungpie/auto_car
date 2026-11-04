#include <stdio.h>
#include "ultrasonic.h"

volatile int dist_left = 0;
volatile int dist_front = 0;
volatile int dist_right = 0;

volatile int sensor_state = 0; // 0: 왼쪽, 1: 정면, 2: 우측 측정 중
volatile int ultrasonic_flag = 1;

void init_ultrasonic(void)
{
	// TRIG 핀들 (PA0,1,2) 출력 설정
	TRIG_DDR |= (1 << TRIG_L_PIN) | (1 << TRIG_F_PIN) | (1 << TRIG_R_PIN);
	// ECHO 핀들 (PE4,5,6) 입력 설정
	ECHO_DDR &= ~((1 << ECHO_L_PIN) | (1 << ECHO_F_PIN) | (1 << ECHO_R_PIN));
	
	// 외부 인터럽트 EICRB 설정 (INT4, INT5, INT6 모두 상승/하강에지 둘다 트리거)
	EICRB |= (1 << ISC40) | (1 << ISC50) | (1 << ISC60);
	
	// 외부 인터럽트 4, 5, 6번 허용
	EIMSK |= (1 << INT4) | (1 << INT5) | (1 << INT6);
	
	// 타이머 3번 설정 (초음파 전용, 1024 분주)
	TCCR3B |= (1 << CS32) | (1 << CS30);
}

// 개별 트리거 생성 함수
void make_trigger(int pin)
{
	TRIG_PORT &= ~(1 << pin);
	_delay_us(2);
	TRIG_PORT |= (1 << pin);
	_delay_us(15);
	TRIG_PORT &= ~(1 << pin);
}

// 1. 왼쪽 센서 (INT4)
ISR(INT4_vect) {
	if (ECHO_PORT & (1 << ECHO_L_PIN)) {
		TCNT3 = 0; // 타이머 3번 리셋 (타이머 1번은 모터가 편하게 쓰도록 건들지 않음)
		} else {
		dist_left = (TCNT3 * 1000000.0 * 1024 / F_CPU) / 58;
		ultrasonic_flag = 1; // 측정 완료 신호
	}
}

// 2. 정면 센서 (INT5)
ISR(INT5_vect) {
	if (ECHO_PORT & (1 << ECHO_F_PIN)) {
		TCNT3 = 0;
		} else {
		dist_front = (TCNT3 * 1000000.0 * 1024 / F_CPU) / 58;
		ultrasonic_flag = 1;
	}
}

// 3. 우측 센서 (INT6)
ISR(INT6_vect) {
	if (ECHO_PORT & (1 << ECHO_R_PIN)) {
		TCNT3 = 0;
		} else {
		dist_right = (TCNT3 * 1000000.0 * 1024 / F_CPU) / 58;
		ultrasonic_flag = 1;
	}
}

// 메인 루프에서 계속 실행될 순차 제어 함수
void ultrasonic_processing(void)
{
	if (ultrasonic_flag == 1)
	{
		ultrasonic_flag = 0;
		
		// 60ms 정도의 간격을 두어 잔여 초음파가 사라지길 기다림 (안정적인 측정용)
		_delay_ms(60);
		
		// 순서대로 돌아가면서 쏘기 (State Machine)
		if (sensor_state == 0) {
			make_trigger(TRIG_L_PIN);
			printf("L:%dcm, F:%dcm, R:%dcm\n", dist_left, dist_front, dist_right);
			sensor_state = 1;
		}
		else if (sensor_state == 1) {
			make_trigger(TRIG_F_PIN);
			printf("L:%dcm, F:%dcm, R:%dcm\n", dist_left, dist_front, dist_right);
			sensor_state = 2;
		}
		else if (sensor_state == 2) {
			make_trigger(TRIG_R_PIN);
			printf("L:%dcm, F:%dcm, R:%dcm\n", dist_left, dist_front, dist_right);
			sensor_state = 0;
		}
	}
}