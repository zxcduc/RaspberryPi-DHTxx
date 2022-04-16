#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include "dhtxx.h"

#ifdef DEBUG
	int err = 0, total = 0;
#endif

int dhtxx_init()
{
	int res;
	
	//Init GPIO using wiringPi
	#if WIRINGPI_INIT_MODE == WIRINGPISETUP
		res = wiringPiSetup();
	#elif WIRINGPI_INIT_MODE == WIRINGPISETUPGPIO || WIRINGPI_INIT_MODE == WIRINGPISETUPSYS
		res = wiringPiSetupGpio();
	#elif WIRINGPI_INIT_MODE == WIRINGPISETUPPHYS
		res = wiringPiSetupPhys();
	#endif
	
	if(res)
		return DHT_ERROR_GPIO;
	
	return DHT_SUCCESS;
}

int readSensor(char *data)
{
	data[0] = data[1] = data[2] = data[3] = data[4] = 0;
	
	//Init GPIO For DHT using output mode
	pinMode(DHT_PIN, OUTPUT);
	//Start at hight voltage mode
	digitalWrite(DHT_PIN, HIGH);
	//Send out start signal by pulls down voltage 20ms (at lest 18ms) to let DHT11 detect the signal
	digitalWrite(DHT_PIN, LOW);
	//sleep(.018);
	delayMicroseconds(startSignalTime);
	//Pulls up voltage (20 - 40 us) and swith to input mode to wait for DHT response
	digitalWrite(DHT_PIN, HIGH);
	//delayMicroseconds(40);
	pinMode(DHT_PIN, INPUT);
	int timeCount = 0;
	
	/*Detect response signal from DHT11 (it send out response signal by pulls down voltage and keep it for 80 us)
	 * */
	//Wait DHTxx pulls down voltage to response start signal (20 - 40 us after Raspberry Pi pulls up voltage)
	while(digitalRead(DHT_PIN) == HIGH)
	{
		timeCount++;
		if(timeCount > 40)
		{
			#ifdef DEBUG
				printf("Wait DHTxx response start signal fail. timeCount > 40: %d\n", timeCount);
			#endif
			
			return DHT_ERROR_TIMEOUT;
		}
		delayMicroseconds(1);
	}
	
	//Wait when DHT pulls up voltage after response start signal (80 us)
	while(digitalRead(DHT_PIN) == LOW)
	{
		//sleep(.001);
		delayMicroseconds(1);
	}
	
	//Wait Data pin pulls down after DHT pulls up
	timeCount = 0;
	while(digitalRead(DHT_PIN) == HIGH)
	{
		timeCount++;
		if(timeCount > 85)
		{
			#ifdef DEBUG
				printf("Wait Data pin pulls down after DHT pulls up fail. timeCount > 80: %d\n", timeCount);
			#endif
			
			return DHT_ERROR_TIMEOUT;
		}
		
		delayMicroseconds(1);
	}

	/* DHT start sending data, read 40 bit data (5 bytes) */
	for (int i = 0; i < 5; i++)
	{
		data[i] = 0;
		for (int j = 8; j > 0; j--)
		{
			/* We are in low signal now, wait for high signal and measure time */
			timeCount = 0;
			/* Wait high signal, about 57-63us long (measured with logic analyzer) */
			while (digitalRead(DHT_PIN) == LOW) 
			{
				if (timeCount > 75) {
					return DHT_ERROR_READ_DATA;
				}
				/* Increase time */
				timeCount++;
				/* Wait 1 us */
				delayMicroseconds(1);
			}
			/* High signal detected, start measure high signal, it can be 26us for 0 or 70us for 1 */
			timeCount = 0;
			/* Wait low signal, between 26 and 70us long (measured with logic analyzer) */
			delayMicroseconds(29);
			while (digitalRead(DHT_PIN) == HIGH) {
				//70 ~ 90 - 29 us delay = 41 ~ 61
				if (timeCount > 61) {
					return DHT_ERROR_READ_DATA;
				}
				/* Increase time */
				timeCount++;
				/* Wait 1 us */
				delayMicroseconds(1);
			}
			
			if (timeCount < 10) {
				/* We read 0 */
			} else {
				/* We read 1 */
				data[i] |= 1 << (j - 1);
			}
		}
	}
	
	#ifdef DEBUG
		total++;
	#endif

	//Verify data was sent by checksum calculate
	if(data[4] == ((data[0] + data[1] + data[2] + data[3]) & 0xFF))
	{
		return DHT_SUCCESS;
	}
	else
	{
		#ifdef DEBUG
			printf("Error. Checksum mismatch.\n data[0] = %d data[1] = %d data[2] = %d data[3] = %d data[4] = %d\n",
				data[0], data[1], data[2], data[3], data[4]);
			fflush(stdout);
			err++;
			printf("Error/Total: %d/%d", err, total);
		#endif
		return DHT_ERROR_CHECKSUM;
	}
}
