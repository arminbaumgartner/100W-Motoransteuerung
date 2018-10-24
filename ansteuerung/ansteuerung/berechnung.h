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

volatile unsigned int steps;	//Timer 1 Schrittweite (3km/h - 100km/h)
int timer1_teiler_mult = 64;	//Timer 1 Teilerzeit
int motor_teiler = 3;			//Elektrische Teilung vom Motor
float uebersetzung = 1;			//Übersetzung
float raddurchmesser = 0.2;		//In Meter

volatile unsigned int step_dauer;
volatile unsigned int step_dauer_help;
volatile unsigned int drehzahl_pro_sekunde;
volatile float geschwindigkeit_help;
volatile unsigned int geschwindigkeit;
volatile unsigned int drehzahl;

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
	steps = TCNT1;
	TCNT1 = 0;
	
	geschwindigkeit_berechnung();

}
void geschwindigkeit_berechnung(void)
{
	
	if(steps < 20)			//Geschwindigkeits überhohung abfangen
	{
		steps = 20;
	}

	step_dauer = steps*timer1_teiler_mult;		//Werte von max 1000*16 ->	16.000	µs
	step_dauer = step_dauer/100;				//Werte von 3 bis 160
	
	
	step_dauer_help = (step_dauer*6*motor_teiler);	//Werte von 54 bis 2880
	step_dauer_help = step_dauer_help/10;			//Werte von 5 bis 288
	
	drehzahl_pro_sekunde = 1000/step_dauer_help;	//Werte von 3 bis 200
	
	drehzahl = drehzahl_pro_sekunde*60;
	
	geschwindigkeit_help = ((drehzahl_pro_sekunde*raddurchmesser*3.14)/uebersetzung) * 3.6;
	
	
	//drehzahl = ceil(drehzahl);
	
	geschwindigkeit_help = ((drehzahl_pro_sekunde * raddurchmesser * 3.14) / uebersetzung) * 3.6;
	geschwindigkeit = ceil (geschwindigkeit_help);
	
	
}
ISR(TIMER1_OVF_vect)			//Motor steht
{
	geschwindigkeit = 0;
	drehzahl = 0;
	drehzahl_pro_sekunde = 0;
	step_dauer = 0;
	
}