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

unsigned int step_time;			//Timer 1 Schrittweite (3km/h - 100km/h)
int timer1_teiler_mult = 16;	//Timer 1 Teiler
int motor_teiler = 3;			//Elektrische Teilung vom Motor
float uebersetzung = 1;			//Übersetzung
float raddurchmesser = 0.2;		//In Meter

long drehzahl_pro_sekunde;
float geschwindigkeit_help;
unsigned int geschwindigkeit;
unsigned int drehzahl;

void geschwindigkeit_berechnung(void);

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
	
	geschwindigkeit_berechnung();
}
void geschwindigkeit_berechnung(void)
{
	
	drehzahl_pro_sekunde = (step_time * timer1_teiler_mult*6)/100;			//Timer 1 Wert * 6 Steps * Übersetzungsverhältnis * 16 -> in µs dauer
	
	/*
	drehzahl_pro_sekunde = drehzahl_pro_sekunde / 1000000;	//durch 1000*1000 um auf sec zu kommen
	
	drehzahl = drehzahl_pro_sekunde * 60;
	drehzahl = ceil(drehzahl);
	
	geschwindigkeit_help = ((drehzahl_pro_sekunde * raddurchmesser * 3.14) / uebersetzung) * 3.6;
	geschwindigkeit = ceil (geschwindigkeit_help);
	*/
	
}
ISR(TIMER1_OVF_vect)			//Motor steht
{
	geschwindigkeit = 1000;
	drehzahl = 99;
	drehzahl_pro_sekunde = 2000;
	step_time = 1000;
	
}