/*
 * datenverarbeitung.h
 *
 * Created: 05.09.2018 15:43:01
 * Author : Armin
 *
 *
 *Dieses Programm dient zur verarbeitung der Daten zwischen Akkumanagement und Motorcontroller
 *
 *	Übermittelte Daten über UART werden verwendet und zur Motorsteuerung genutzt
 *
 *
 */ 

#define LADUNG100	3650
#define LADUNG80	3600
#define LADUNG60	3600
#define LADUNG40	3600
#define LADUNG20	3600
#define LADUNG00	2800



volatile uint16_t voltage;	//Akku Spannung	0-3650mV
volatile uint8_t temperatur;//Temperatur	0-120C


void daten_aufteilen(void)
{
	temperatur = akku_daten[0];					//Temperatur
	voltage = akku_daten[1];					//Low-Spannung
	voltage = voltage | (akku_daten[2]<<8);		//HIGH Byte der Spannung
	
}

int akku_ladestand (uint16_t spannung)
{
	uint8_t ladestand=100;
	
	if(spannung >= LADUNG100)
	{
		ladestand = 100;
	}
	else if(ladestand >= LADUNG80 && ladestand <= LADUNG100)
	{
		ladestand = 80;
	}
	else if(ladestand >= LADUNG60 && ladestand <= LADUNG80)
	{
		ladestand = 60;
	}
	else if(ladestand >= LADUNG40 && ladestand <= LADUNG60)
	{
		ladestand = 40;
	}
	else if(ladestand >= LADUNG20 && ladestand <= LADUNG40)
	{
		ladestand = 20;
	}
	else if(ladestand >= LADUNG00 && ladestand <= LADUNG20)
	{
		ladestand = 10;
	}
	else if(ladestand <= LADUNG00)
	{
		ladestand = 0;
	}
	
	return ladestand;
	
}