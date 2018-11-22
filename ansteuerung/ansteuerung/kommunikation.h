/*
 * kommunikation.h
 *
 * Created: 05.09.2018 15:43:01
 * Author : Armin
 *
 *
 *Dieses Programm dient zur übermittlung der Daten zwischen Akkumanagement und Motorcontroller
 *
 *	Dabei wird UART verwendet
 *	Timer 0 sorgt für die syncronisation der Datenübertragung
 *	Für Detail sihe Dokumentation
 *
 *
 */ 

#define F_CPU  16000000UL   //Takt 
#define BAUD  9600UL		//gewünschte Baudrate 
#define UBRR_CALC (F_CPU/16UL/BAUD-1)  //Baudrtate aus Takt berechnen

char start = 0;

char  bf=0;

char i=0;				//laufvariable für empfangdaten[i]
volatile unsigned char empfangs_daten[3];	//dynamischer Speicher der Akkudaten
volatile unsigned char akku_daten[3];		//statischer Speicher der Akkudaten
char overflow_counter=0;	//Zählt Overflows für Pause

char empfang_test;




void init_usart (void)
{  
	UBRR1H = (unsigned char)(UBRR_CALC>>8); //Baudrate einstellen  
	UBRR1L = (unsigned char)(UBRR_CALC);    
	UCSR1B = UCSR1B | (1<<RXEN1);	//Empfang ein
	UCSR1B = UCSR1B | (1<<RXCIE1);	//Empfang-Interruput ein
	UCSR1B = UCSR1B | (1<<TXEN1);	//Sender ein  
	
	UCSR1C = UCSR1C &~ (1<<UMSEL10);	//asynchr.
	UCSR1C = UCSR1C &~ (1<<UMSEL11);	
	
	UCSR1C = UCSR1C &~ (1<<UPM10);		//even parity
	UCSR1C = UCSR1C | (1<<UPM11);
	
	UCSR1C = UCSR1C &~ (1<<USBS1);		//1-Stop-Bit
	
	UCSR1C = UCSR1C | (1<<UCSZ10);		//8-Bit data
	UCSR1C = UCSR1C | (1<<UCSZ11);
	UCSR1B = UCSR1B &~ (1<<UCSZ12);
	
	UCSR1C = UCSR1C &~ (UCPOL1);		//muss low sein im asynchron mode
	
	
}
void init_transmission_timer(void)
{
	TCCR0A = TCCR0A &~ (1<<COM0A0);		//Normal Port Operations
	TCCR0A = TCCR0A &~ (1<<COM0A1);
	
	TCCR0A = TCCR0A &~ (1<<WGM00);		//Normal Mode
	TCCR0A = TCCR0A &~ (1<<WGM01);
	TCCR0B = TCCR0B &~ (1<<WGM02);
	
	TCCR0B = TCCR0B &~ (1<<CS00);		//Teiler 256 (16MHz / 256 = 16µs)
	TCCR0B = TCCR0B &~ (1<<CS01);
	TCCR0B = TCCR0B | (1<<CS02);
	
	OCR0A = 100;	//Compare bei 1,6ms	(16µs * 100 = 1,6ms)
	
	TIMSK0 = TIMSK0 | (1<<OCIE0A);		//Interrupt nach 1,6ms
	//TIMSK0 = TIMSK0 | (1<<TOIE0);		//Overflow Interrupt nach 4ms
	
}
void save_akku_daten(void)
{
		
	akku_daten[0] = empfangs_daten[0];		//Temperatur
	akku_daten[1] = empfangs_daten[1];		//Low-Spannung
	akku_daten[2] = empfangs_daten[2];		//High-Spannung
	
	daten_aufteilen();
	
	/*
	
	if(akku_daten[0] == 0b01010101)
	{
		//Debug Pin
	}
	
	*/
	
	
	
}

ISR(USART1_RX_vect)     //Interrupt für Empfang 
{  
	
	
	if(start == 1 && overflow_counter >= 5)			//Wenn nicht gerade in Daten ist && pause eingehalten wurde		//5*1,6ms = 8ms
	{
		
		
		TCNT0 = 0;					//nötig um nicht in de overflow zu geraten
		overflow_counter = 0;		//Counter wird auf 0 gesetzt
		/*
		for(i=0; i<3; i++)
		{
			while(!(UCSR1A & (1<<RXC1)));   //warten bis Zeichen fertig empfangen
			empfangs_daten[i] = UDR1;		//Zeichen in Variable ablegen	//UDR1 -> 8 Bit daten 9.Bit wäre in UCSR1B
		}
		*/
		
		while( !(UCSR1A & (1<<RXC1)) );   //warten bis Zeichen fertig empfangen
		empfangs_daten[0] = UDR1;		//Zeichen in Variable ablegen	//UDR1 -> 8 Bit daten 9.Bit wäre in UCSR1B			//Temperatur
		while( !(UCSR1A & (1<<RXC1)) );   //warten bis Zeichen fertig empfangen
		empfangs_daten[1] = UDR1;		//Zeichen in Variable ablegen	//UDR1 -> 8 Bit daten 9.Bit wäre in UCSR1B			//LOW-Spannung
		while( !(UCSR1A & (1<<RXC1)) );   //warten bis Zeichen fertig empfangen
		empfangs_daten[2] = UDR1;		//Zeichen in Variable ablegen	//UDR1 -> 8 Bit daten 9.Bit wäre in UCSR1B			//HIGH-Spannung
		
		
				
	}
}
ISR (TIMER0_COMPA_vect)
{
	TCNT0 = 0;
	start = 1;
	
	
	overflow_counter++;		//Zählen der Overflows
	
	if(overflow_counter == 3)		//nach 3*1,6ms= 4,8ms werden Daten gespeichert
	{
		save_akku_daten();
	}
	
}