/*
 * berechnung.h
 *
 * Created: 05.09.2018 15:43:01
 * Author : Armin
 *
 *
 *Dieses Programm dient zur Berechnung des Motors
 *
 *	Geschwindgkeits / Umdrehungen pro min berrechnung durch Timer 1
 *	Erkennung des Stilstandes durch Timer 1
 *
 *	Berechnungsfunktionen die für das Hauptprogramm nötig sind
 *
 *
 */ 

#include <avr/io.h>

int step_time;					//Timer 1 Schrittweite (3km/h - 100km/h)
int motor_teiler = 3;			//Elektrische Teilung vom Motor
float uebersetzung = 3;			//Übersetzung
float raddurchmesser = 0.2;		//In Meter

float geschwindigkeit;
float drehzahl;


void Init_Timer1 (void)
{
	TCCR1B = TCCR1B &~ (1<<CS10);		// Teiler 256 (16MHz / 256 = 16µs)
	TCCR1B = TCCR1B &~ (1<<CS11);		//Kleiner Schritt 16µs
	TCCR1B = TCCR1B | (1<<CS12);		//Größter Schritt 16384µs
	
	TIMSK1 = TIMSK1 | (1<<TOIE1);		//OVERFLOW-Interrupt aktivieren
}
void geschwindigkeit_auslesen(void)
{
	step_time = TCNT1;
	TCNT1 = 0;
}
void geschwindigkeit_berechnung(void)
{
	float drehzahl_pro_sekunde;
	
	drehzahl_pro_sekunde = step_time*uebersetzung*6;	//Timer 1 Wert * Übersetzungsverhältnis * 6 Steps
	
	drehzahl = drehzahl_pro_sekunde * 60;
	
	geschwindigkeit = ((drehzahl_pro_sekunde * raddurchmesser * 3.14) / uebersetzung) * 3.6;
	geschwindigkeit = ceil (geschwindigkeit);
}
ISR(TIMER1_OVF_vect)			//Motor steht
{
	geschwindigkeit = 0;
	drehzahl = 0;
	
}