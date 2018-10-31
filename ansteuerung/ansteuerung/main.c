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
#include <stdio.h>
#include <stdlib.h>
#include "berechnung.h"
#include "motoransteuerung.h"
#include "lcd.h"
#include "kommunikation.h"

char ausgabe[10];
int x=0;


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
	DDRD = DDRD &~ (1<<DDD0);		//PD0 als INPUT //vorwärts / rückwärts Schalter
	PORTD = PORTD | (1<<PORTD0);		//PULL-UP
		
	DDRB = DDRB &~ (1<<DDB4);	//PB4 als INPUT		//vorwärts / rückwärst Schalter
	PORTB = PORTB | (1<<PORTB4);	//PULL-UP
	
	//Shutdown Pin	
	DDRE = DDRE | (1<<DDE6);	//Shutdown-Pin (PE6) als OUTPUT
	PORTE = PORTE | (PORTE6);	//Shutdown-Pin auf HIGH -> da er LOW-AKTIVE ist
	
	//ADC
	DDRF = DDRF &~ (1<<DDF0);	//ADC0-Pin (PF0) als INPUT
	
	//LCD - Pins
	DDRB = DDRB | (1<<PORTB0);		//RS (PB0) als OUTPUT
	DDRF = DDRF | (1<<PORTF1);		//Enable (PF1) als OUTPUT
	DDRF = DDRF | (1<<PORTF7);		//LCD-DB7 (PF1 µC) als OUTPUT
	DDRF = DDRF | (1<<PORTF6);		//LCD-DB6 (PF1 µC) als OUTPUT
	DDRF = DDRF | (1<<PORTF5);		//LCD-DB5 (PF1 µC) als OUTPUT
	DDRF = DDRF | (1<<PORTF4);		//LCD-DB4 (PF1 µC) als OUTPUT
	
	//UART
	
	//Debug-Pin
	DDRD = DDRD | (1<<DDD0);
	PORTD = PORTD | (1<<PORTD0);
	
	DDRD = DDRD | (1<<DDD4);
	PORTD = PORTD | (1<<PORTD4);
	
	
	Init_Pinchange();	//Initialisierung Hallsensoren
	
	Init_PWM();			//Initialisierung 6-fach PWM signale
	
	Init_ADC();			//Initialisierung ADC
	
	Init_Timer1();		//Initialisierung Berechnungen Geschw. Drehzahl
	
	LCD_init();			//Initialisierung  LCD
	LCD_cmd(0x0C);		//Display ON, Cursor OFF, Blinking OFF 
	
	
	PORTE = PORTE &~ (PORTE6);	//Shutdown-Pin auf LOW -> um Treiber einzuschalten
	
	sei();
	
    while (1) 
    {		
		
		x++;
		_delay_ms(1);
		
		while(x<1000)
		{
		
		geschwindigkeit_berechnung();
		
		//dtostrf((float)drehzahl, 5, 0, ausgabe);
		sprintf(ausgabe,"%d",drehzahl);
		LCD_cmd(0x80);   //gehe zu 1. Zeile, 1. Position
		LCD_string("Drehzahl: ");
		LCD_cmd(0x8a);   //gehe zu 1. Zeile, 25. Position
		LCD_string(ausgabe); 
		LCD_cmd(0x8f);
		LCD_string("U/m");     
		
		//dtostrf((float)geschwindigkeit, 5, 0, ausgabe);
		sprintf(ausgabe,"%d",geschwindigkeit);
		LCD_cmd(0xC0);   //gehe zu 2. Zeile, 1. Position 
		LCD_string("U/s:");
		LCD_cmd(0xca);   //gehe zu 2. Zeile, 25. Position
		LCD_string(ausgabe);
		LCD_cmd(0xcf);
		LCD_string("km/h"); 
		
		x=0;
		}
		
    }
	
}

