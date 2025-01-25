/*
* process.c
*
* Created: 8.2.2022 г. 8:47:49
*  Author: Steven
*/
#include "Drivers/defines.h"

extern const char termination[4];

void execute_drink(struct recipe *drink)
{
	if (drink->drinkMilliliters > process.milliliters && drink->waterDelay < process.timer)
	{
		WTR_PUMP_ON;
		if(drink->product1Request || drink->product2Request)
		{
			MIXER_ON;	
			VALVE_OFF;
		}
		else if(process.milliliters > 15 || waterValve) VALVE_ON;
		process.waterFinished = false;
	}
	else
	{
		WTR_PUMP_OFF;
		MIXER_OFF;
		VALVE_OFF;
		process.waterFinished = true;
	}
	
	if (drink->product1Request)
	{
		if(drink->product1StartDelay+5 < process.timer && drink->product1StopDelay+5 > process.timer && !process.flowmeterError) MOTOR1_PWM = drink->product1Speed;
		else
		{
			MOTOR1_PWM = 0;
			if(drink->product1StopDelay+5 < process.timer) process.product1Finished = true;
		}
	}
	else process.product1Finished = true;
	
	if (drink->product2Request)
	{
		if(drink->product2StartDelay+5 < process.timer && drink->product2StopDelay+5 > process.timer && !process.flowmeterError) MOTOR2_PWM = drink->product2Speed;
		else
		{
			MOTOR2_PWM = 0;
			if(drink->product2StopDelay+5 < process.timer) process.product2Finished = true;
		}
	}
	else process.product2Finished = true;
	
/*	----------------------- sirop 1 */
	
	if (drink->syrup1Request && process.timer > 5)
	{
		if (drink->syrup1Length+5 > process.timer && !process.flowmeterError) PUMP1_ON;
		else
		{
			PUMP1_OFF;
			if(drink->syrup1Length+5 < process.timer) process.syrup1Finished = true;
		}
	}
	else if(!drink->syrup1Request && process.timer > 5) process.syrup1Finished = true;

/*	----------------------- sirop 2 */

	if (drink->syrup2Request && process.timer > 5)
	{
		if (drink->syrup2Length+5 > process.timer && !process.flowmeterError) PUMP2_ON;
		else
		{
			PUMP2_OFF;
			if(drink->syrup2Length+5 < process.timer) process.syrup2Finished = true;
		}
	}
	else if(!drink->syrup2Request && process.timer > 5) process.syrup2Finished = true;
	
/*	----------------------- sirop 3 */

 	if (drink->syrup3Request && process.timer > 5)
	{
 		if (drink->syrup3Length+5 > process.timer && !process.flowmeterError) PUMP3_ON;
 		else
 		{
 			PUMP3_OFF;
 			if(drink->syrup3Length+5 < process.timer) process.syrup3Finished = true;
 		}
	}
	else if(!drink->syrup3Request && process.timer > 5) process.syrup3Finished = true;

	
	if (process.waterFinished && process.product1Finished && process.product2Finished && process.syrup1Finished && process.syrup2Finished  && process.syrup3Finished )
	{
		//drinkStats[process.drinkID]++;
		//EEPROM_write_word(drinkStatsaddress+(drinkStatsAddressOffset*process.drinkID), drinkStats[process.drinkID]);	
		process_reset();
	}
}

void process_reset()
{
	memset(&process, 0, sizeof(process));
	MIXER_OFF;
	MOTOR1_PWM = 0;
	MOTOR2_PWM = 0;
	PUMP1_OFF;
	PUMP2_OFF;
	PUMP3_OFF;
	WTR_PUMP_OFF;
	VALVE_OFF;
	printf("drinkExecution=0%s", termination);
	process.soundRequest = true;
}

void flowmeter_monitoring()
{
	if (process.flowmeterPreviousVal == process.flowmeterPulses)
	{
		process.flowmeterError = true;
		process.flowmeterErrorCounter++;
		if (process.flowmeterErrorCounter >= 10)
		{
			process.execute = false;
			MIXER_OFF;
			MOTOR1_PWM = 0;
			MOTOR2_PWM = 0;
			PUMP1_OFF;
			PUMP2_OFF;
			PUMP3_OFF;
			WTR_PUMP_OFF;
			VALVE_OFF;
			printf("page WaterError%s", termination);
			printf("drinkExecution=0%s", termination);
			process.soundRequest = true;		
		}
	}
	else
	{
		process.flowmeterError = false;
	}
	if(process.execute) process.flowmeterPreviousVal = process.flowmeterPulses;
}

void sound_on()
{
	if(delay(500))
	{
		process.beep_count++;
		BUZZER_TGL;
		if(process.beep_count > 3) 
		{
			process.soundRequest = false;
			process.beep_count = 0;
			BUZZER_OFF;
		}
	}
}

uint8_t percent_to_PWM(uint8_t percent)
{
	switch(percent)
	{
		case 0:
			return 0;
			break;
		case 10:
			return 26;
			break;
		case 20:
			return 51;
			break;
		case 30:
			return 77;
			break;
		case 40:
			return 102;
			break;
		case 50:
			return 128;
			break;
		case 60:
			return 153;
			break;
		case 70:
			return 179;
			break;
		case 80:
			return 204;
			break;
		case 90:
			return 230;
			break;
		case 100:
			return 255;
			break;																		
	}
	return 0;
}

uint8_t PWM_to_percent(uint8_t byteData)
{
	switch(byteData)
	{
		case 0:
			return 0;
			break;
		case 26:
			return 10;
			break;
		case 51:
			return 20;
			break;
		case 77:
			return 30;
			break;
		case 102:
			return 40;
			break;
		case 128:
			return 50;
			break;
		case 153:
			return 60;
			break;
		case 179:
			return 70;
			break;
		case 204:
			return 80;
			break;
		case 230:
			return 90;
			break;
		case 255:
			return 100;
			break;
	}
	return 0;
}
