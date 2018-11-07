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

int akku_ladestand (uint16_t spannung)
{
	uint8_t ladestand=100;
	
	if(spannung >= 3600)
	{
		ladestand = 100;
	}
	else if(ladestand <= 2800)
	{
		ladestand = 0;
	}
	
	return ladestand;
	
}