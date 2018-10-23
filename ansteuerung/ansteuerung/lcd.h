/*
 * lcd.h
 *
 * Created: 05.09.2018 15:43:01
 * Author : Armin
 *
 *
 *Dieses Programm dient zur �usgabe der Geschwindigkeit und der Drehzahl mittel LC-Display
 *
 *	
 *
 *	
 *
 *
 */ 

////////////////////////////////////////////////////////////////////////////// 
// 
// Headerfile LCD.h zur LCD Ansteuerung am Port F des ATmega32u4 
// 
// Befehle: LCD_init(), LCD_cmd(char data), LCD_send(char data)  
//          LCD_string(char *data) 
// 
// LCD_init();               initialisiert Port F  
//                           und LCD im 4-Bit Mode, 2 Zeilen, 5x8 Dots 
//                           Bsp.: LCD_init(); 
// 
// LCD_cmd(char data);       schickt Befehl ans LCD 
//                           Bsp.: LCD_cmd(0xC5);  
//gehe zu 2. Zeile, 6. Position 
// 
// LCD_send(char data);      schickt Daten ans LCD 
//                           Bsp.: LCD_send(0xEF); sendet ein � 
// 
// LCD_string(char *data);   schickt eine Zeichenkette ans LCD 
//                           Bsp.: LCD_string("Hallo");    sendet Hallo 
// 
// Pinbelegung am Board: 
// LCD |   32U4   | Bemerkung 
// ----|----------|-------------------- 
// DB7 |    PF7   | 
// DB6 |    PF6   | 
// DB5 |    PF5   | 
// DB4 |    PF4   |  
// DB3 |     -    | wird nicht ben�tigt 
// DB2 |     -    | wird nicht ben�tigt 
// DB1 |     -    | wird nicht ben�tigt 
// DB0 |     -    | wird nicht ben�tigt 
// E   |    PF1   | 
// R/W |     -    | per HW auf GND gelegt 
// RS  |    PB0   | 
//
// //////////////////////////////////////////////////////////////////////////////// 


//-- Hier die Pinzuordnung bei Bedarf aendern (siehe Tabelle oben) --//
 
#define DB7 PORTF7 
#define DB6 PORTF6 
#define DB5 PORTF5 
#define DB4 PORTF4  
#define E PORTF1 
#define RS PORTB0

// PORT Zuordnung muss im gesamten LCD.h angepasst werden!!! 
// d.h. PORTF und DDRF durch gew�nschten anderen Port ersetzen // 

//------------------- Ende Pinzuordnung ------------------------------//


#include <avr/io.h> 
#include <util/delay.h>      // _delay_ms() geht nur bis max. 262.14 ms / F_CPU !!!


void delay_ms (unsigned int ms)   //Hilfsfunktion: Zeitvernichtung 
{  
	for (unsigned int i=0; i<ms; i++)
	{
		_delay_ms(1);  
	}

}

void Enable(void)  //Hilfsfunktion: H=>L Flanke der Enable Leitung (E) 
{  
	PORTF = PORTF | (1<<E); //E = 1   
	delay_ms(5);  
	PORTF = PORTF &~(1<<E); //E = 0  
	delay_ms(5); 
}

//////////////////////////////////////////////////////////////////////////// // 
// LCD_init(..) Initialisierung: Port F, 4-Bit Mode, 2 Zeilen, 5x8 Dots  
// ////////////////////////////////////////////////////////////////////////////

void LCD_init(void)
{  
	//DDRF = DDRF | (1<<E);										//E als Ausgang
	//DDRB = DDRB | (1<<RS);										//RS als Ausgang
	//DDRF = DDRF | (1<<DB7) | (1<<DB6) | (1<<DB5) | (1<<DB4);	//DB7..DB4 als Ausgang
	
	delay_ms(50);									//lt. Datenblatt min. 15ms nach Power ON warten  
	PORTF = PORTF &~ (1<<E);			//E=0 (RW=0 per HW)
	PORTB = PORTB &~ (1<<RS);			//RS=0
	
	
	// Function Set  
		//DB7..DB4 = 0011  
		PORTF = PORTF & (~(1<<DB7) & ~(1<<DB6));	//Interface auf 8 Bit  
		PORTF = PORTF | (1<<DB5) | (1<<DB4);    
		Enable();
	
		//DB7..DB4 = 0011  #
		PORTF = PORTF & (~(1<<DB7) & ~(1<<DB6));	//Interface auf 8 Bit  
		PORTF = PORTF | (1<<DB5) | (1<<DB4);    
		Enable();
	
		//DB7..DB4 = 0011  
		PORTF = PORTF & (~(1<<DB7) & ~(1<<DB6));	//Interface auf 8 Bit  
		PORTF = PORTF | (1<<DB5) | (1<<DB4);    
		Enable();
	
		//DB7..DB4 = 0010  
		PORTF = PORTF & (~(1<<DB7) & ~(1<<DB6) &~(1<<DB4));  
		PORTF = PORTF | (1<<DB5);					//Interface auf 4 Bit  
		Enable();
		
	// 2-zeilig, 5x8 Matrix //  
	
	//DB7..DB4 = 0010  
	PORTF = PORTF & (~(1<<DB7) & ~(1<<DB6) &~(1<<DB4));  
	PORTF = PORTF | (1<<DB5);   //Upper Nibble  
	Enable();
	
	//DB7..DB4 = 1000  
	PORTF = PORTF | (1<<DB7);   //Lower Nibble  
	PORTF = PORTF & (~(1<<DB6) & ~(1<<DB5) & ~(1<<DB4));  
	Enable();
	
	//Display Off //  
	//DB7..DB4 = 0000  
	PORTF = PORTF & (~(1<<DB7) & ~(1<<DB6) & ~(1<<DB5) & ~(1<<DB4)); //Upper Nibble  
	Enable();
	
	//DB7..DB4 = 1000  
	PORTF = PORTF | (1<<DB7);   //Lower Nibble  
	PORTF = PORTF & (~(1<<DB6) & ~(1<<DB5) & ~(1<<DB4));  
	Enable();
	
	//Clear Display //  
	//DB7..DB4 = 0000  
	PORTF = PORTF & (~(1<<DB7) & ~(1<<DB6) & ~(1<<DB5) & ~(1<<DB4)); //Upper Nibble  
	Enable();
	
	//DB7..DB4 = 0001  
	PORTF = PORTF & (~(1<<DB7) & ~(1<<DB6) & ~(1<<DB5)); //Lower Nibble  
	PORTF = PORTF | (1<<DB4);  
	Enable();
	
	//No Display Shift //  
	//DB7..DB4 = 0000  
	PORTF = PORTF & (~(1<<DB7) & ~(1<<DB6) & ~(1<<DB5) & ~(1<<DB4)); //Upper Nibble  
	Enable();
	
	//DB7..DB4 = 0011  
	PORTF = PORTF & (~(1<<DB7) & ~(1<<DB6)); //Lower Nibble  
	PORTF = PORTF | (1<<DB5) | (1<<DB4);  
	Enable();
	
	// Display ON , Cursor ON, Blinken ON //  
	//DB7..DB4 = 0000  
	PORTF = PORTF & (~(1<<DB7) & ~(1<<DB6) & ~(1<<DB5) & ~(1<<DB4)); //Upper Nibble  
	Enable();
	
	//DB7..DB4 = 1111  
	PORTF = PORTF | (1<<DB7) | (1<<DB6) | (1<<DB5) | (1<<DB4); //Lower Nibble  
	Enable(); 
	
}

//////////////////////////////////////////////////////////////////////////// 
// 
// LCD_send(..) sendet 1 Byte im 4-Bit Mode 
// 
////////////////////////////////////////////////////////////////////////////

void LCD_send(char data) 
{  
	char temp = data;
	
	PORTB = PORTB | (1<<RS); //SFR vom LCD mit RS auf Daten umschalten      
	
	//Upper Nibble senden   
	if (temp & 0b10000000) { PORTF = PORTF | (1<<DB7);}   
	else { PORTF = PORTF & ~(1<<DB7); }
	
	if (temp & 0b01000000) { PORTF = PORTF | (1<<DB6); }   
	else { PORTF = PORTF & ~(1<<DB6); }
	
	if (temp & 0b00100000) { PORTF = PORTF | (1<<DB5); }   
	else { PORTF = PORTF & ~(1<<DB5); }
	
	if (temp & 0b00010000) { PORTF = PORTF | (1<<DB4); }   
	else { PORTF = PORTF & ~(1<<DB4); }
	
	Enable();   
	delay_ms(1);       
	
	//Lower Nibble senden   
	if (temp & 0b00001000) { PORTF = PORTF | (1<<DB7); }
	else { PORTF = PORTF & ~(1<<DB7); }
	
	if (temp & 0b00000100) { PORTF = PORTF | (1<<DB6); }
	else { PORTF = PORTF & ~(1<<DB6); }
	
	if (temp & 0b00000010) { PORTF = PORTF | (1<<DB5); }
	else { PORTF = PORTF & ~(1<<DB5); }
	
	if (temp & 0b00000001) { PORTF = PORTF | (1<<DB4); }   
	else { PORTF = PORTF & ~(1<<DB4); }
	
	Enable();   
	delay_ms(1);
	
}

//////////////////////////////////////////////////////////////////////////// 
// 
// LCD_cmd(..) Befehl senden im 4-Bit Mode 
// 
////////////////////////////////////////////////////////////////////////////

void LCD_cmd(char data) 
{  
	char temp = data;
	
	PORTB = PORTB & ~(1<<RS); //SFR vom LCD mit RS auf Befehle umschalten
	      
	//Upper Nibble senden   
	if (temp & 0b10000000) { PORTF = PORTF | (1<<DB7); }   
	else { PORTF = PORTF & ~(1<<DB7); }
	
	if (temp & 0b01000000) { PORTF = PORTF | (1<<DB6); }
	else { PORTF = PORTF & ~(1<<DB6); }
	
	if (temp & 0b00100000) { PORTF = PORTF | (1<<DB5); }
	else { PORTF = PORTF & ~(1<<DB5); }
	
	if (temp & 0b00010000) { PORTF = PORTF | (1<<DB4); }
	else { PORTF = PORTF & ~(1<<DB4); }
	
	Enable();   
	delay_ms(1);       
	
	//Lower Nibble senden   
	if (temp & 0b00001000) { PORTF = PORTF | (1<<DB7); }
	else { PORTF = PORTF & ~(1<<DB7); }
	
	if (temp & 0b00000100) { PORTF = PORTF | (1<<DB6); }
	else { PORTF = PORTF & ~(1<<DB6); }
	
	if (temp & 0b00000010) { PORTF = PORTF | (1<<DB5); }
	else { PORTF = PORTF & ~(1<<DB5); }
	
	if (temp & 0b00000001) { PORTF = PORTF | (1<<DB4); }   
	else { PORTF = PORTF & ~(1<<DB4); }
	
	Enable();   
	delay_ms(1);
	
}

//////////////////////////////////////////////////////////////////////////// 
// 
// LCD_string(..) sendet ganzen String im 4-Bit Mode 
// 
//////////////////////////////////////////////////////////////////////////// 

void LCD_string(char *data) 
{      
	while (*data != '\0') //bis zum letzten Zeichen            
	{
		LCD_send(*data++);
	} 
} 