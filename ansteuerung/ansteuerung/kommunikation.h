/*
 * kommunikation.h
 *
 * Created: 05.09.2018 15:43:01
 * Author : Armin
 *
 *
 *Dieses Programm dient zur übermittlung der Daten zwischen Akkumanagement und Motorcontroller
 *
 *	
 *
 *	
 *
 *
 */ 

#define F_CPU  16000000UL   //Takt 
#define BAUD  9600UL    //gewünschte Baudrate 
#define UBRR_CALC (F_CPU/16UL/BAUD-1)  //Baudrtate aus Takt berechnen


char out_text[]="\n\rTaste 'e' ==> LED ein\n\rTaste 'a' ==> LED aus\n\rBitte um Eingabe: \0";       //\n New Line, \r Carrige Return, \0 String Ende 

char i=0;		//Laufvariable für out_text[i] 
char in;		//Variable für empf. Zeichen

void init_usart (void)
{  
	UBRR1H = (unsigned char)(UBRR_CALC>>8); //Baudrate einstellen  
	UBRR1L = (unsigned char)(UBRR_CALC);    
	UCSR1B = UCSR1B | (1<<RXEN1);	//Empfang ein
	UCSR1B = UCSR1B | (1<<RXCIE1);	//Empfang-Interruput ein
	UCSR1B = UCSR1B | (1<<TXEN1);	//Sender ein  
	
	UCSR1C |= (1<<UCSZ11)|(1<<UCSZ10);  //asynchr., 8 Daten-, kein Parity-, 1 Stop 
}
ISR(USART1_RX_vect)     //Interrupt für Empfang 
{  
	while(!(UCSR1A & (1<<RXC1)));   //warten bis Zeichen fertig empfangen  
	in = UDR1;     //Zeichen in Variable ablegen

	if (in == 'e') {PORTB &=~(1<<PB0);}  //LED PB0 ein  
	if (in == 'a') {PORTB |= (1<<PB0);}  //LED PB0 aus  

}