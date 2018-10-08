/*
 * ansteuerung.c
 *
 * Created: 05.09.2018 15:43:01
 * Author : armin
 *
 *
 *Dieses Programm dient zur Ansteuersoftware des BLDC
 *
 *
 *
 *
 *
 *
 *
 */ 


#define F_CPU 16000000UL



#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include "berechnung.h"
#include "motoransteuerung.h"



int main(void)
{
	
	CLKPR = 0x80;						//Clock prescaler 16MHz
	CLKPR = 0x00;

	MCUCR = MCUCR | (1<<JTD);			//JTD Schnittstelle ausschalten für PWM6 Mode
	MCUCR = MCUCR | (1<<JTD);
	
	//MOTOR PWM PINS
	DDRC = DDRC | (1<<DDC7);	//OC4A -Pin (PC7) als OUTPUT	//PHASE A
	DDRC = DDRC | (1<<DDC6);	//OC4A#-Pin (PC6) als OUTPUT	//PHASE A
	DDRB = DDRB | (1<<DDB6);	//OC4B -Pin (PB6) als OUTPUT	//PHASE B
	DDRB = DDRB | (1<<DDB5);	//OC4B#-Pin (PB5) als OUTPUT	//PHASE B
	DDRD = DDRD | (1<<DDD7);	//OC4D -Pin (PD7) als OUTPUT	//PHASE C
	DDRD = DDRD | (1<<DDD6);	//OC4D#-Pin (PD6) als OUTPUT	//PHASE C
	
	//HALL SENSORS PINS
	DDRB = DDRB &~ (1<<DDB1);	//PCINT1-Pin (PB1) als INPUT	//HALL A
	DDRB = DDRB &~ (1<<DDB2);	//PCINT2-Pin (PB2) als INPUT	//HALL B
	DDRB = DDRB &~ (1<<DDB3);	//PCINT3-Pin (PB3) als INPUT	//HALL C
	PORTB = PORTB | (1<<PORTB1);	//PULL-UP
	PORTB = PORTB | (1<<PORTB2);	//PULL-UP
	PORTB = PORTB | (1<<PORTB3);	//PULL-UP

	//Vorwärts - Rückwärts Schalter
	DDRB = DDRB &~ (1<<DDB4);	//PB4 als INPUT		//vorwärts / rückwärst Schalter
	PORTB = PORTB | (1<<PORTB4);	//PULL-UP
	
	//Shutdown Pin
	DDRE = DDRE | (1<<DDE6);	//Shutdown-Pin (PE6) als OUTPUT
	PORTE = PORTE | (PORTE6);	//Shutdown-Pin auf HIGH -> da er LOW-AKTIVE ist
	
	//ADC
	DDRF = DDRF &~ (1<<DDF0);
	
		
	
	Init_Pinchange();
	
	Init_PWM();
	
	Init_ADC();
	
	PORTE = PORTE &~ (PORTE6);	//Shutdown-Pin auf LOW -> um Treiber einzuschalten
	
	sei();
	
    while (1) 
    {
		geschwindigkeit_berechnung();
		
    }
	
}

