/*
 * Mc1.c
 *
 *  Created on: Dec 13, 2020
 *      Author: Dell
 */

//Included drivers
#include "lcd.h"
#include "keypad.h"
#include "uart.h"
#include "timer.h"

//Some command to be Sent
#define M1_READY 0x01
#define HOLD 0x02
#define WRITE_IN_EEPROM 0x03
#define BUZZER_RING 0x04
#define STOP_BUZZER_RING 0x05
#define DONE 0x06
#define OPEN_THE_DOOR 0x07
#define CLOSE_THE_DOOR 0x08
#define OK 0x09

uint8 g_tick=0;             //Global Variable To Get Number of Ticks

ISR(TIMER1_COMPA_vect)      // ISR For Timer1 To increment Tick
{
	g_tick++;

}


int main(void) //start main
{

	uint8 key;                 		//TO Get The Pressed Key From KeyPad
	uint8 i;				   		//Counter For loops
	uint8 Conifirmation_flag = 0;   //This Flag is Set When Confirmation For PW Done
	uint8 done_flag=0;				//Sensitive Case To Enter while loop for Two Scenarios
	uint8 error1_counter=0;			//This Flag is Set When The User Enter Wrong PW
	uint8 NewPW[5];					//Array For New PASSWORD TO Be sent TO Mc2 Using UART
	uint8 pwback[5];				//Array Of PW Sent By EEPROM From Mc2
	uint8 password[5];				//User Password To be Compared With PwBack[5]

	LCD_init();						//Initialization For LCD
	UART_init();					//Initialization For UART

	LCD_clearScreen();				//Clear LCD Screen
	LCD_displayString("PLZ ENTER NEW PW");  //Display String on LCD
	LCD_goToRowColumn(1,0);
	UART_sendByte(WRITE_IN_EEPROM);      //Send TO Mc2 request To Write In EEPROM

	for(i = 0 ; i < 5 ; i++)
	{
		key = KeyPad_getPressedKey();    //get the Pressed Key from Keypad
		if((key >= 0) && (key <= 9))
		{
			NewPW[i] = key;              //Saving The Key into NewPW Array To be sent
			LCD_displayCharacter('#');   //display PW using #
		}
		_delay_ms(100); //new button every 100ms
	}
	if(KeyPad_getPressedKey() == 13)      //When Press ON Key From KeyPad
	{
		LCD_clearScreen();                //Clear screen
		LCD_displayString("SAVING YOUR PW");  // display strin on lcd
		for(i = 0 ; i < 5 ; i++)
		{
			UART_sendByte(NewPW[i]);          //Send PW to Mc2 By UART
			_delay_ms(100);
		}

	}

	UART_sendByte(M1_READY);             //Send To Mc2 that Mc1 Ready To receive

	//RECEIVE
	for ( i = 0 ; i < 5 ; i++)
	{
		pwback[i] = UART_recieveByte();      //Receiving Pw From EEPROM From MC2
	}



	LCD_clearScreen();                    //clear lcd screen
	LCD_displayString("ENTER SAME PW");   //display String on lcd
	LCD_goToRowColumn(1,0);

	while(Conifirmation_flag == 0)       //condition for Entering the loop
	{
		for(i = 0 ; i < 5 ; i++)
		{
			key = KeyPad_getPressedKey();   //get the key From Keypad
			if((key >= 0) && (key <= 9))
			{
				password[i] = key;          //Save the keys to an Array
				LCD_displayCharacter('#');
			}
			_delay_ms(100); //new button every 100ms
		}
		if(KeyPad_getPressedKey() == 13)  //When Press ON Key From KeyPad
		{
			//Compare The Entered PW With pw that stored in EEPROM in MC2
			if(pwback[0] == password[0] && pwback[1] == password[1] && pwback[2] == password[2] && pwback[3] == password[3] && pwback[4] == password[4] )
			{
				LCD_clearScreen();   //clear lcd screen
				LCD_displayString("DONE");  //display String on lcd
				_delay_ms(400);
				Conifirmation_flag = 1;   //Set that flag means pw check complete
				//now Exit from the loop
			}
			else   //Means Check Failed and Wrong Pw is entered by user in confirmation phase
			{
				LCD_clearScreen();  //clear lcd screen
				LCD_displayString("RE-ENTER THE PW");       //display String on lcd
				LCD_goToRowColumn(1,0);
			}

		}
	}
	Conifirmation_flag = 0;   //The Flag is Still Zero means To ask the User To Enter confirmation PW Again




	while(1)
	{
		//main menu make the user to choose between change pw OR open the door
		LCD_clearScreen();
		LCD_displayString("x: CHANGE PW");
		LCD_goToRowColumn(1,0);
		LCD_displayString("+: OPEN DOOR");

		if(KeyPad_getPressedKey() == '*')   // waiting key  * for changing pw
		{
			while(error1_counter < 3 && done_flag == 0) //condition for Entering the loop
			{
				LCD_clearScreen();  //clear lcd screen
				LCD_displayString("ENTER OLD PW"); //display string on lcd
				LCD_goToRowColumn(1,0);
				for(i = 0 ; i < 5 ; i++)
				{
					key = KeyPad_getPressedKey(); //waiting  key from KeyPad
					if((key >= 0) && (key <= 9))  //check the key is number or not?
					{
						password[i] = key;   //save the key in an Array of PASSWORD
						LCD_displayCharacter('#');
					}
					_delay_ms(100); //new button every 100ms
				}
				if(KeyPad_getPressedKey() == 13) //When Press ON Key From KeyPad
				{
					//Compare The Entered PW With pw that stored in EEPROM in MC2
					if(pwback[0] == password[0] && pwback[1] == password[1] && pwback[2] == password[2] && pwback[3] == password[3] && pwback[4] == password[4] )
					{
						LCD_clearScreen();  //clear lcd screen
						LCD_displayString("DONE"); //display string on lcd
						_delay_ms(500); //delay for 0.5 sec
						done_flag = 1;  //this flag is set when pw check successfully complete


						//Enter new PW for 2nd Time

						LCD_clearScreen();  // clear lcd screen
						LCD_displayString("PLZ ENTER New PW");  //display string on lcd
						LCD_goToRowColumn(1,0);
						UART_sendByte(WRITE_IN_EEPROM); //send request to Mc2 To write in EEPROM

						for(i = 0 ; i < 5 ; i++)
						{
							key = KeyPad_getPressedKey(); //get the key From Keypad
							if((key >= 0) && (key <= 9))  //check the key is number or not
							{
								NewPW[i] = key;  // save the key in an Array of pw
								LCD_displayCharacter('#');  //display pw in #
							}
							_delay_ms(100); //new button every 100ms
						}
						if(KeyPad_getPressedKey() == 13) //When Press ON Key From KeyPad
						{
							for(i = 0 ; i < 5 ; i++)
							{
								UART_sendByte(NewPW[i]); //send pw to MC2 to write it in EEPROM
								_delay_ms(100);
							}

						}

						UART_sendByte(M1_READY);  //Send To Mc2 that Mc1 Ready To receive

						//RECEIVE
						for ( i = 0 ; i < 5 ; i++)
						{
							pwback[i] = UART_recieveByte(); //Receiving Pw From EEPROM From MC2
						}


						LCD_clearScreen();  //clear lcd screen
						LCD_displayString("ENTER SAME PW"); //display string on lcd
						LCD_goToRowColumn(1,0);

						while(Conifirmation_flag == 0) //condition for Entering the loop
						{
							for(i = 0 ; i < 5 ; i++)
							{
								key = KeyPad_getPressedKey(); //get the key From Keypad
								if((key >= 0) && (key <= 9)) //check the key if it number or not
								{
									password[i] = key;  //Save the key in an Array of Password
									LCD_displayCharacter('#'); //display pw in #
								}
								_delay_ms(100); //new button every 100ms
							}
							if(KeyPad_getPressedKey() == 13) //if ON key is pressed from the Keypad
							{
								//Compare The Entered PW With pw that stored in EEPROM in MC2
								if(pwback[0] == password[0] && pwback[1] == password[1] && pwback[2] == password[2] && pwback[3] == password[3] && pwback[4] == password[4] )
								{
									LCD_clearScreen(); //clear lcd screen
									LCD_displayString("DONE"); //display string on lcd
									_delay_ms(400); //delay 400 ms
									Conifirmation_flag = 1; //Set that flag means pw check complete
									//now you can Exit from the loop
								}
								else
								{
									LCD_clearScreen(); //clear lcd screen
									LCD_displayString("RE-ENTER THE PW"); //display string on lcd
									LCD_goToRowColumn(1,0);
								}

							}
						}
						//The Flag is Still Zero means user entered confirmation pw wrong
						//and the User will Enter confirmation PW Again
						Conifirmation_flag = 0;
					}

					else
					{
						LCD_clearScreen(); //clear lcd screen
						LCD_displayString("INCORRECT PW"); //display string on lcd
						_delay_ms(500); //delay for 0.5 sec

						error1_counter++; //this error is incremented means that User Enter Wrong PW
						if(error1_counter == 3)
						{
							//when User Entered 3 times Wrong PW
							UART_sendByte(BUZZER_RING); //Send To MC2 To Ring BUZZER
							LCD_clearScreen(); //clear lcd screen
							LCD_displayString("LOCKED FOR 1 min"); //display string on lcd
							TIMER1_CTC_MODE(); //Activate Timer1
							while(g_tick != 60)
							{/*JUST WIAT 1 min*/}
							Stop_Timer1(); //Stop Timer1
							g_tick = 0; //Clear global tick variable
							UART_sendByte(STOP_BUZZER_RING); //Send to Mc2 To Stop the BUZZER
						}
					}
				}
			}
			error1_counter = 0; //clear number of errors
			done_flag = 0; //clear done flag to Enter the main menu again
		}
		else if (KeyPad_getPressedKey() == '+') // waiting key  + Opening the door
		{
			while(error1_counter < 3 && done_flag == 0) //condition for Entering the loop
			{
				LCD_clearScreen(); //clear lcd scrren
				LCD_displayString("ENTER YOUR PW"); //display string on lcd screen
				LCD_goToRowColumn(1,0);
				for(i = 0 ; i < 5 ; i++)
				{
					key = KeyPad_getPressedKey(); //get the key From Keypad
					if((key >= 0) && (key <= 9)) //check the key is number or not?
					{
						password[i] = key;      //Save the key in an Array of Password
						LCD_displayCharacter('#');  //display pw in #
					}
					_delay_ms(100); //new button every 100ms
				}
				if(KeyPad_getPressedKey() == 13)  //if ON key is pressed from the Keypad
				{
					//Compare The Entered PW With pw that stored in EEPROM in MC2
					if(pwback[0] == password[0] && pwback[1] == password[1] && pwback[2] == password[2] && pwback[3] == password[3] && pwback[4] == password[4] )
					{
						LCD_clearScreen();  //clear lcd screen
						LCD_displayString("OPENING");  //display strin on lcd

						UART_sendByte(OPEN_THE_DOOR); //Send request to Mc2 to OPEN THE DOOR
						TIMER1_CTC_MODE();    //Activate Timer1
						while(g_tick != 15)
						{/*JUST WIAT 15 sec*/}
						Stop_Timer1();  //stop Timer1
						g_tick = 0; //clear global variable tick
						UART_sendByte(OK); //Send to MC2 To stop opening the door

						LCD_clearScreen(); //clear lcd screen
						LCD_displayString("HOLD"); //display string on lcd

						UART_sendByte(HOLD); //Send request to MC2 to HOLD for 3 sec
						TIMER1_CTC_MODE(); //Activate TIMER1
						while(g_tick != 3)
						{/*JUST WIAT 3 sec*/}
						Stop_Timer1(); //stop TIMER1
						g_tick = 0; //Clear Global variable tick
						UART_sendByte(OK); //send to MC2 to stop HOLDING

						LCD_clearScreen(); //clear lcd screen
						LCD_displayString("CLOSING"); //display string on lcd

						UART_sendByte(CLOSE_THE_DOOR); //send request to MC2 TO CLOSE THE DOOR
						TIMER1_CTC_MODE(); //Activate TIMER1
						while(g_tick != 15)
						{/*JUST WIAT 15 sec*/}
						Stop_Timer1(); //Stop TIMER1
						g_tick = 0; //Clear global Variable Tick
						UART_sendByte(OK); //send to MC2 to stop CLOSING THE DOOR

						done_flag = 1; //process complete (OPENING ,HOLD,CLOSING)

					}
					else
					{
						LCD_clearScreen(); //clear lcd screen
						LCD_displayString("INCORRECT PW"); // display string on lcd
						_delay_ms(500); //delay for 0.5 sec
						error1_counter++; //this error is incremented means that User Enter Wrong PW
						if(error1_counter == 3)
						{
							//when User Entered 3 times Wrong PW
							LCD_clearScreen(); //clear lcd screen
							LCD_displayString("LOCKED FOR 1 min"); //display string on lcd
							UART_sendByte(BUZZER_RING); //send to MC2 to START BUZZER
							TIMER1_CTC_MODE(); //Activate Timer1
							while(g_tick != 60)
							{/*JUST WIAT 1 min*/}
							Stop_Timer1(); //stop  TIMER1
							g_tick = 0; //clear global variable tick
							UART_sendByte(STOP_BUZZER_RING); //send to MC2 to Stop BUZZER

						}
					}
				}
			}
			error1_counter = 0; //clear number of errors
			done_flag = 0; //clear done flag to Enter the main menu again

		}

	}







} //end of the main















