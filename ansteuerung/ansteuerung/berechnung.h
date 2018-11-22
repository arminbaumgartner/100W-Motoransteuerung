/*
 * berechnung.h
 *
 * Created: 05.09.2018 15:43:01
 * Author : Armin
 *
 *
 *Dieses Programm dient zur Berechnung der Fahrdaten
 *
 *	Geschwindgkeits / Umdrehungen pro min berrechnung durch Timer 1
 *	Erkennung des Stilstandes durch Timer 1
 *
 *	Berechnungsfunktionen die für das Hauptprogramm nötig sind
 *
 *
 */ 

#include <avr/io.h>

volatile uint16_t steps;	//Timer 1 Schrittweite (3km/h - 100km/h)
int timer1_teiler_mult = 4;	//Timer 1 Teilerzeit
int motor_teiler = 3;			//Elektrische Teilung vom Motor
float uebersetzung = 1;			//Übersetzung
float raddurchmesser = 0.2;		//In Meter

char overflow;

volatile uint16_t step_dauer;
volatile uint16_t step_dauer_help;
volatile uint16_t drehzahl_pro_sekunde;
volatile float geschwindigkeit_help;
volatile uint16_t geschwindigkeit;
volatile uint16_t drehzahl;

void geschwindigkeit_berechnung(void);

void Init_Timer1 (void)
{
	TCCR1B = TCCR1B | (1<<CS10);		// Teiler 256 (16MHz / 64 = 4µs)
	TCCR1B = TCCR1B | (1<<CS11);		//Kleiner Schritt 4µs		(1*4µs)
	TCCR1B = TCCR1B &~ (1<<CS12);		//Größter Schritt 262ms	(65535*4µs)
	
	TIMSK1 = TIMSK1 | (1<<TOIE1);		//OVERFLOW-Interrupt aktivieren
}
void geschwindigkeit_auslesen(void)
{
	
	overflow = 0;
	steps = TCNT1;
	TCNT1 = 0;
	

}
void geschwindigkeit_berechnung(void)
{
	
	if(overflow)
	{
		steps = 0;
		geschwindigkeit = 0;
		drehzahl = 0;
		drehzahl_pro_sekunde = 0;
		step_dauer = 0;
	}
	else
	{
		
	
	
		if(steps <= 25)			//Geschwindigkeits überhohung abfangen -> 25*4 = 100
		{
			steps = 25;
		}
	
	
		if(steps >= 13750)		//Geschwindigkeits unterschreitung -> 13.750 nötig um 1 U/s zu generieren
		{
			steps=13750;
		}
	

		step_dauer = steps*timer1_teiler_mult;		//Werte von max 55.000µs um auf 1U/s zu kommen
		step_dauer = step_dauer/100;				//Werte von 1 bis 550				///////ab hier korriegieren /// Teiler vieleicht auf 64 statt 256
	
	
		step_dauer_help = (step_dauer*6*motor_teiler);	//Werte von 18 bis 9900
		step_dauer_help = step_dauer_help/10;			//Werte von 1 bis 990
	
		drehzahl_pro_sekunde = 1000/step_dauer_help;	//Werte von 1 bis 1000
	
		drehzahl = drehzahl_pro_sekunde*60;
	
		geschwindigkeit_help = ((drehzahl_pro_sekunde*raddurchmesser*3.14)/uebersetzung) * 3.6;
	
	
		//drehzahl = ceil(drehzahl);
	
		geschwindigkeit_help = ((drehzahl_pro_sekunde * raddurchmesser * 3.14) / uebersetzung) * 3.6;
		geschwindigkeit = ceil (geschwindigkeit_help);
	
		}
	
	
}
ISR(TIMER1_OVF_vect)			//Motor steht
{
	
	overflow = 1;
	
	steps = 0;
	geschwindigkeit = 0;
	drehzahl = 0;
	drehzahl_pro_sekunde = 0;
	step_dauer = 0;
	
}