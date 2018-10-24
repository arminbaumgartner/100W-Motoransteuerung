/*
 * motoransteuerung.h
 *
 * Created: 05.09.2018 15:43:01
 
 * Author : Armin
 *
 *
 *Dieses Programm dient zur initalisierung der Funktionen
 *
 *
 *	6-fach PWM realisiert durch den Timer 4 + Death time zwischen dem Schalten
 *
 *	Auslesen der Hallsensoren realisiert durch den Pinchange interrupt PCINTT0 (PB1-PB3)
 *
 *	Auslesen des Gaspedals realisiert durch den internen ADC
 *
 *
 */ 

#define PHASE_A_PWM (TCCR4E | (1<<OC4OE0)|(1<<OC4OE1))					//PHASE_A PWM	PWM signal an Phase A
#define PHASE_A_GND1 (PORTC &~ (1<<PORTC7))								//PHASE_A GND1	Stufe_1		HS deaktiviert
#define PHASE_A_GND2 (PORTC | (1<<PORTC6))								//PHASE_A GND2	Stufe_2		LS aktiviert
#define PHASE_A_GND ((PORTC &~ (1<<PORTC7)) | (1<<PORTC6))				//PHASE_A GND	HS deaktiviert	LS aktiviert
#define PHASE_A_OFF (PORTC &~ (1<<PORTC7) &~ (1<<PORTC6))				//PHASE_A OFF	Hochohmig

#define PHASE_B_PWM (TCCR4E | (1<<OC4OE2) | (1<<OC4OE3))				//PHASE_B PWM	PWM signal an Phase A
#define PHASE_B_GND1 (PORTB &~ (1<<PORTB6))								//PHASE_B GND1	Stufe_1		HS deatkiviert
#define PHASE_B_GND2 (PORTB | (1<<PORTB5))								//PHASE_B GND2	Stufe_2		LS aktiviert
#define PHASE_B_GND ((PORTB &~ (1<<PORTB6)) | (1<<PORTB5))				//PHASE_A GND	HS deaktiviert	LS aktiviert
#define PHASE_B_OFF (PORTB &~ (1<<PORTB6) &~ (1<<PORTB5))				//PHASE_B OFF	Hochohmig

#define PHASE_C_PWM (TCCR4E | (1<<OC4OE5)|(1<<OC4OE4))					//PHASE_C PWM	PWM signal an Phase A
#define PHASE_C_GND1 (PORTD &~ (1<<PORTD7))								//PHASE_C_GND1	Stufe_1		HS deaktiviert	
#define PHASE_C_GND2 (PORTD | (1<<PORTD6))								//PHASE_C GND2	Stufe_2		LS aktiviert
#define PHASE_C_GND ((PORTD &~ (1<<PORTD7)) | (1<<PORTD6))				//PHASE_A GND	HS deaktiviert	LS aktiviert
#define PHASE_C_OFF (PORTD &~ (1<<PORTD7) &~ (1<<PORTD6))				//PHASE_C OFF	Hochohmig

#include <avr/io.h>
#include <avr/interrupt.h>


char stufe;
char richtung = 1;
char vor = 1;
char adc_low;
char adc_high;


void Init_PWM (void){
	
	TCCR4B = TCCR4B &~ (1<<CS40);		//CLKT4 Teiler auf 8 gesetzt -> 2MHz
	TCCR4B = TCCR4B &~ (1<<CS41);
	TCCR4B = TCCR4B | (1<<CS42);
	TCCR4B = TCCR4B &~ (1<<CS43);
	
	TCCR4A = TCCR4A | (1<<PWM4A);
	TCCR4D = TCCR4D &~ (1<<WGM40);		//PWM6 mode Single slope
	TCCR4D = TCCR4D | (1<<WGM41);
	
	TCCR4A = TCCR4A | (1<<COM4A0);		//OC4A pins belegung wenn PWM6 mode ausgewählt ist
	TCCR4A = TCCR4A &~ (1<<COM4A1);
	
	TCCR4A = TCCR4A | (1<<COM4B0);		//Alle 6 Pins freischalten
	TCCR4A = TCCR4A &~ (1<<COM4B1);
	TCCR4C = TCCR4C | (1<<COM4D0);
	TCCR4C = TCCR4C &~ (1<<COM4D1);
	
	TCCR4B = TCCR4B | (1<<DTPS40);		//Death Time presacler auf 1
	TCCR4B = TCCR4B &~ (1<<DTPS41);
	
	//TCCR4B = TCCR4B | (1<<PWM4X);		//Inversion mode -> kann die ausgänge invertieren bei 1
	
	//TIMSK4 = TIMSK4 | (OCIE4A);
	
	OCR4C = 255;						//200*500ns = 100µs = 10kHz  //umgeändert auf 255 test zwecke
	OCR4A = 150;
	
	TCCR4E = TCCR4E | (1<<OC4OE0)|(1<<OC4OE1); //Start Ausgang
	
	DT4 = 0xFF;							//Death time
	
}
 void Init_Pinchange( void )
 {
	PCICR =  PCICR | (1<<PCIE0);		//Enable pin change interrupt0 wenn 1 interrupt von den interrupts auslöst (PORTB)
	 
	PCMSK0 = PCMSK0 | (1<<PCINT1);		//Enable pin change interrupt on PB1
	PCMSK0 = PCMSK0 | (1<<PCINT2);		//Enable pin change interrupt on PB2
	PCMSK0 = PCMSK0 | (1<<PCINT3);		//Enable pin change interrupt on PB3
	//PCMSK0 = PCMSK0 | (1<<PCINT4);		//Enable pin change interrupt on PB4 für Schalter

	
	//Externer Interrupt für Umschalter
	EICRA = EICRA | (1<<ISC00);		//Interrupt bei low pegel
	EICRA = EICRA &~ (1<<ISC01);
	
	EIMSK = EIMSK | (1<<INT0);		//Externer Interrupt 0 freischalten
	
 }
 void Init_ADC(void)
 {
	 ADMUX = ADMUX | (1<<REFS0);  //Vref=AVCC
	 ADMUX = ADMUX &~(1<<REFS1);
	 
	 ADMUX  = ADMUX | (1<<ADLAR);  //linksbündig
	 
	 ADMUX = ADMUX &~ (1<<MUX0);	//single ended measurement
	 ADMUX = ADMUX &~ (1<<MUX1);	//auf ADC0 (PF0)
	 ADMUX = ADMUX &~ (1<<MUX2);
	 ADMUX = ADMUX &~ (1<<MUX3);
	 ADMUX = ADMUX &~ (1<<MUX4);
	 ADCSRB = ADCSRB &~ (1<<MUX5);
	 
	 ADCSRA = ADCSRA | (1<<ADEN);		//ADC einschalten
	 ADCSRA = ADCSRA | (1<<ADIE);	//ADC Interrupt freischalten
	 
	 ADCSRA = ADCSRA &~ (1<<ADPS0);	//Prescaler für ADC-Clock
	 ADCSRA = ADCSRA | (1<<ADPS1);	//64 Teiler
	 ADCSRA = ADCSRA | (1<<ADPS2);
	 
	 ADCSRA = ADCSRA | (1<<ADSC);	//Wandlung starten
	 
 }
ISR(PCINT0_vect)
{
	
	stufe = PINB & 0x0e;
	stufe = stufe/2;		//herunterbrechen von XXX0 -> 0XXXX			z.b. 1110 -> 0111
	
	if(drehzahl <= 100)		//wechsel nur unter 100U/m zulassen
	{
		vor = richtung;
	}
	
	vor=1;		//Test
	
	if(vor)		//Schalter AUS(vorwärts)	ACHTUNG: es wird PINB abgefragt nicht sufe (0x10)
	{	
			
		switch(stufe)
		{
			case 0x05:				//HALL_A + HALL_C
			{
				TCCR4E = 0x00;
				PORTD = PHASE_C_OFF;
				//PORTB = PHASE_B_GND1;
				//PORTB = PHASE_B_GND2;
				PORTB = PHASE_B_GND;
				//TCCR4E = TCCR4E | (1<<OC4OE0)|(1<<OC4OE1);
				TCCR4E = PHASE_A_PWM;
				break;
			}
			case 0x01:				//HALL_A
			{
				TCCR4E = 0x00;
				PORTB = PHASE_B_OFF;
				//PORTD = PHASE_C_GND1;
				//PORTD = PHASE_C_GND2;
				PORTD = PHASE_C_GND;
				//TCCR4E = TCCR4E | (1<<OC4OE0)|(1<<OC4OE1);
				TCCR4E = PHASE_A_PWM;
				break;
			}
			case 0x03:				//HALL_A + HALL_B
			{
				TCCR4E = 0x00;
				PORTC = PHASE_A_OFF;
				//PORTD = PHASE_C_GND1;
				//PORTD = PHASE_C_GND2;
				PORTD = PHASE_C_GND;
				//TCCR4E = TCCR4E | (1<<OC4OE2) | (1<<OC4OE3);
				TCCR4E = PHASE_B_PWM;
				break;
			}
			case 0x02:				//HALL_B
			{
				TCCR4E = 0x00;
				PORTD = PHASE_C_OFF;
				//PORTC = PHASE_A_GND1;
				//PORTC = PHASE_A_GND2;
				PORTC = PHASE_A_GND;
				//TCCR4E = TCCR4E | (1<<OC4OE2) | (1<<OC4OE3);
				TCCR4E = PHASE_B_PWM;
				break;
			}
			case 0x06:			//HALL_B + HALL_C
			{
				TCCR4E = 0x00;
				PORTB = PHASE_B_OFF;
				//PORTC = PHASE_A_GND1;
				//PORTC = PHASE_A_GND2;
				PORTC = PHASE_A_GND;
				//TCCR4E = TCCR4E | (1<<OC4OE5)|(1<<OC4OE4);
				TCCR4E = PHASE_C_PWM;
				break;			
			}
			case 0x04:			//HALL_C
			{
				TCCR4E = 0x00;
				PORTC = PHASE_A_OFF;
				//PORTB = PHASE_B_GND1;
				//PORTB = PHASE_B_GND2;
				PORTB = PHASE_B_GND;
				//TCCR4E = TCCR4E | (1<<OC4OE5)|(1<<OC4OE4);
				TCCR4E = PHASE_C_PWM;
				break;
			}
			default:
			{
				TCCR4E = 0x00;
			}
		}	//Klammen Switch
		
	}	//Klemmen if
	else			//Schalter EIN(rückwärts) LOW-AKTIV		ACHTUNG: es wird PINB abgefragt nicht stufe (0x10)
	{
		switch(stufe)
		{
			case 0x05:			//HALL_A + HALL_C
			{
				TCCR4E = 0x00;
				PORTD = PHASE_C_OFF;
				//PORTC = PHASE_A_GND1;
				//PORTC = PHASE_A_GND2;
				PORTC = PHASE_A_GND;
				//TCCR4E = TCCR4E | (1<<OC4OE2) | (1<<OC4OE3);
				TCCR4E = PHASE_B_PWM;
				break;
			}
			case 0x01:			//HALL_A
			{
				TCCR4E = 0x00;
				PORTB = PHASE_B_OFF;
				//PORTC = PHASE_A_GND1;
				//PORTC = PHASE_A_GND2;
				PORTC = PHASE_A_GND;
				//TCCR4E = TCCR4E | (1<<OC4OE5)|(1<<OC4OE4);
				TCCR4E = PHASE_C_PWM;
				break;
			}
			case 0x03:			//HALL_A + HALL_B
			{
				TCCR4E = 0x00;
				PORTC = PHASE_A_OFF;
				//PORTB = PHASE_B_GND1;
				//PORTB = PHASE_B_GND2;
				PORTB = PHASE_B_GND;
				//TCCR4E = TCCR4E | (1<<OC4OE5)|(1<<OC4OE4);
				TCCR4E = PHASE_C_PWM;
				break;
			}
			case 0x02:			//HALL_B
			{
				TCCR4E = 0x00;
				PORTD = PHASE_C_OFF;
				//PORTB = PHASE_B_GND1;
				//PORTB = PHASE_B_GND2;
				PORTB = PHASE_B_GND;
				//TCCR4E = TCCR4E | (1<<OC4OE0)|(1<<OC4OE1);
				TCCR4E = PHASE_A_PWM;
				break;
			}
			case 0x06:			//HALL_B + HALL_C
			{
				TCCR4E = 0x00;
				PORTB = PHASE_B_OFF;
				//PORTD = PHASE_C_GND1;
				//PORTD = PHASE_C_GND2;
				PORTD = PHASE_C_GND;
				//TCCR4E = TCCR4E | (1<<OC4OE0)|(1<<OC4OE1);
				TCCR4E = PHASE_A_PWM;
				break;
			}
			case 0x04:			//HALL_C
			{
				TCCR4E = 0x00;
				PORTC = PHASE_A_OFF;
				//PORTD = PHASE_C_GND1;
				//PORTD = PHASE_C_GND2;
				PORTD = PHASE_C_GND;
				//TCCR4E = TCCR4E | (1<<OC4OE2) | (1<<OC4OE3);
				TCCR4E = PHASE_B_PWM;
				break;
			}
			default:
			{
				TCCR4E = 0x00;
			}
		}	//Klammer Switch
	}	//Klammer Else
	
	geschwindigkeit_auslesen();
	
}	//Klammer Pin change
ISR(INT0_vect)
{
	///		Vorwärts - Rückwärts	 ///
	if((PIND & (1<<PIND0)) == 0x01)		//Schalter AUS(vorwärts)	ACHTUNG: es wird PINB abgefragt nicht sufe (0x10)
	{
			richtung = 1;
	}
	else
	{
			richtung = 0;
	}

}
ISR(ADC_vect)						//Löst aus, wenn die Konversation beendet ist
{
	adc_low = ADCL;					//zuerst immer Low Bits holen
	adc_high = ADCH;				//dann High Bits holen
	if(adc_high >= 250)
	{
		adc_high = 250;
	}
	OCR4A = adc_high;
	ADCSRA = ADCSRA | (1<<ADSC);	//Wandlung starten
}
