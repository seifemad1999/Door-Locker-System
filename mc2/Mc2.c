/*
 * Mc2.c
 *
 *  Created on: Dec 13, 2020
 *      Author: Dell
 */

//Included drivers
#include "uart.h"
#include "external_eeprom.h"
#include "timer.h"
//Some command to be sent or Received
#define M1_READY 0x01
#define HOLD 0x02
#define WRITE_IN_EEPROM 0x03
#define BUZZER_RING 0x04
#define STOP_BUZZER_RING 0x05
#define DONE 0x06
#define OPEN_THE_DOOR 0x07
#define CLOSE_THE_DOOR 0x08
#define OK 0x09

int main(void)   //Start Main
{

	DDRB = 0xFF;               //Set PORTB as OUTPUT
	uint8 val = 0;             //EEPROM Put it's Read data on it
	uint8 i;				   //Counter for loops
	uint8 RECEVIED;			   //Variable to Received command from MC1
	uint8 recieved[5];		   //Array to put on it The Received PW from MC1
	uint8 temp[5];             //Array to save on it PW Read from EEPROM and send it to MC1

	EEPROM_init();             //Initialization EEPROM
	UART_init();               //Initialization UART



	while(1)
	{
		RECEVIED = UART_recieveByte();  //Command REcieved from MC1

		if(RECEVIED == WRITE_IN_EEPROM)  //Condition write in EEPROM
		{
			for ( i = 0 ; i < 5 ; i++)
			{
				recieved[i] = UART_recieveByte();   //Store Received PW from MC1 in An Array
			}

			for(i = 0 ; i < 5 ; i++)
			{
				EEPROM_writeByte(0x0311+i, recieved[i]); /* Write Recevied PW in the external EEPROM */
				_delay_ms(10); //delay

			}

			_delay_ms(100); //delay 1 ms

			for(i = 0 ; i < 5 ; i++)
			{
				EEPROM_readByte(0x0311+i, &val);  /* Read The PW from the external EEPROM */
				_delay_ms(10); //delay
				temp[i] = val;  //Store the read PW in an TEMP Array
			}
			while(UART_recieveByte() != M1_READY){} // wait Command M1 READY
			for(i = 0 ; i < 5 ; i++)
			{
				UART_sendByte(temp[i]);  //Send Stored PW in EEPROM to MC1
				_delay_ms(100); //delay 1 ms
			}


		}

		else if(RECEVIED == BUZZER_RING)  //Condition TO RING BUZZER
		{
			PORTB = 0x01;   //Ring BUZZER
			while(UART_recieveByte() != STOP_BUZZER_RING)
			{/*WAIT STOP BUZZER Command*/}
			PORTB = 0;      //Stop BUZZER
		}


		else if(RECEVIED == OPEN_THE_DOOR) //Condition To OPEN the DOOR
		{
			//Rotate Motor ClockWise
			PORTB |= (1<<PB2);
			PORTB &=~(1<<PB1);
			while(UART_recieveByte() != OK)
			{/*WAIT OK Command*/}

		}
		else if(RECEVIED == HOLD)         //Condition To HOLD the DOOR
		{
			//Stop Motor Rotation For 3 sec
			PORTB = 0;
			while(UART_recieveByte() != OK)
			{/*WAIT OK Command*/}
			//PORTB=0;
		}
		else if(RECEVIED == CLOSE_THE_DOOR)  ////Condition To CLOSE the DOOR
		{
			//Rotate Motor Anti ClockWise
			PORTB |= (1<<PB1);
			PORTB &=~(1<<PB2);
			while(UART_recieveByte() != OK)
			{/*WAIT OK Command*/}
			PORTB = 0;  //Stop Motor Rotation
		}

	}


} //End Main
