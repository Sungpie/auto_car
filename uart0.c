/*
 * uart0.c
 *
 * Created: 2026-06-16 오전 9:57:55
 *  Author: kccistc
 */ 
#include "uart0.h"
#include <avr/io.h>

extern int func_state;

extern void (*fp[]) ();

void init_uart0(void);
void UART0_transmit(uint8_t data);
void pc_commnad_processing(void);

// P278 표 12-3 참고
// PC로부터 1byte가 들어 오면 자동적으로 이곳으로 진입한다.
// 예를 들어, led_all_on() 이면 11번 이곳으로 진입한다.
extern volatile uint8_t bt_data; // 

ISR(USART0_RX_vect)
{
	volatile uint8_t data;
	volatile static int i = 0;
	
	data = UDR0; 
	
	bt_data = data; // 
	
	if(data == '\n' || data == '\r')
	{
		if (((rear + 1) % QUEUE_SIZE) == (front % QUEUE_SIZE))
		{
			return;
		}
		rx_buff[rear][i] = '\0';
		i = 0;
		rear = (rear + 1) % QUEUE_SIZE;
	}
	else
	{
		if (((rear + 1) % QUEUE_SIZE) == (front % QUEUE_SIZE))
		return;
		
		rx_buff[rear][i++] = data;
	}
}


void init_uart0(void)
{
/*
1. 전송속도 : 9600bps
2. start/stop 설정
3. RX(수신) : interrupt로 설정
*/
	UBRR0H = 0x00;
	UBRR0L = 207; // 9600bps 9-9 219쪽
	UCSR0A |= (1 << U2X0); // 2배속
	// UART0을 송신, 수신이 다 가능하고 RX interrupt가 가능하도록 설정한다.
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); 
}

// UART0로 1bytes를 전송하는 함수
void UART0_transmit(uint8_t data)
{
	while (!(UCSR0A & 1 << UDRE0)) // data가 송신 중이면 송신이 끝날 때까지 기다림
		; // 동작 없음
	UDR0 = data;
}

void pc_commnad_processing(void)
{
	if(front != rear)  // data가 rx_buff에 존재하는지 확인
	{
		printf("%s", rx_buff[front]); //해당 행의 문자열 출력
		// printf("%s", rx_buff[front][0]); 과 같음
		
		if (strncmp((char* )rx_buff[front], "led_shift_left_on", strlen("led_shift_left_on"))  == 0)
		{
			func_state = 0;
		}
		else if (strncmp((char* )rx_buff[front], "led_shift_right_on", strlen("led_shift_right_on")) == 0)
		{
			func_state = 1;
		}
		else if (strncmp((char* )rx_buff[front], "led_shift_left_keep_on", strlen("led_shift_left_keep_on")) == 0)
		{
			func_state = 2;
		}
		else if (strncmp((char* )rx_buff[front], "led_shift_right_keep_on", strlen("led_shift_right_keep_on")) == 0)
		{
			func_state = 3;
		}
		else if (strncmp((char* )rx_buff[front], "led_flower_on", strlen("led_flower_on")) == 0)
		{
			func_state = 4;
		}
		else if (strncmp((char* )rx_buff[front], "led_flower_off", strlen("led_flower_off")) == 0)
		{
			func_state = 5;
		}
		front = (front + 1) % QUEUE_SIZE;
	}
	
	fp[func_state] ();
}