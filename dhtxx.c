#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <wiringPi.h>

#define DHT_PIN 17

int main()
{
	//Init GPIO using wiringPi
	if(wiringPiSetupGpio())
	{
		printf("Error, can not init GPIO by wiringPi. Exit...");
		fflush(stdout);
		exit(1);
	}
	
	int err = 0, total = 0;

	while(1)
	{
		char data[5] = {0,0,0,0,0};
		
		//Init GPIO For DHT using output mode
		pinMode(DHT_PIN, OUTPUT);
		//Start at hight voltage mode
		digitalWrite(DHT_PIN, HIGH);
		//Send out start signal by pulls down voltage 20ms (at lest 18ms) to let DHT11 detect the signal
		digitalWrite(DHT_PIN, LOW);
		//sleep(.018);
		usleep(20000);
		//Pulls up voltage (20 - 40 us) and swith to input mode to wait for DHT response
		digitalWrite(DHT_PIN, HIGH);
		//delayMicroseconds(40);
		pinMode(DHT_PIN, INPUT);
		int timeCount = 0;
		
		/*Detect response signal from DHT11 (it send out response signal by pulls down voltage and keep it for 80 us)
		 * */
		
		while(digitalRead(DHT_PIN) == HIGH)
		{
			timeCount++;
			if(timeCount > 40) break;
			delayMicroseconds(1);
		}
		
		//Wait when DHT pulls up voltage after response start signal (80 us)
		while(digitalRead(DHT_PIN) == LOW)
		{
			//sleep(.001);
			delayMicroseconds(1);
		}
		
		//Wait Data pin pulls down
		timeCount = 0;
		while(digitalRead(DHT_PIN) == HIGH)
		{
			timeCount++;
			if(timeCount > 80) break;
			delayMicroseconds(1);
		}

		// DHT start sending data, read 40 bit data
		for(int i = 0; i < 40; i++)
		{
			//Wait when DHT11 start to transmit 1 bit data (every bit of data begins with 50 us low voltage level)
			while(digitalRead(DHT_PIN) == LOW)
			{
				delayMicroseconds(1);
			}
			//Count how many us DHT11 keep high voltage
			delayMicroseconds(29);			
			/*Save each bit into data array by bitwise operators
			 * 26 - 28 us high voltage length mean data "0"
			 * 70 us high voltage length mean 1 bit data "1" */
			data[i/8] <<= 1;
			if(digitalRead(DHT_PIN) == HIGH)
			{
				data[i/8] |= 1;
				timeCount = 0;
				while(digitalRead(DHT_PIN) == HIGH)
				{
					timeCount++;
					if(timeCount > 41)
					{
						break;
						printf("Time count > 41: %d", timeCount);
					}
					delayMicroseconds(1);
				}
			}
		}
		
		/*int j = 0;
		// detect change and read data
		for (int i = 0; i< 83; i++) {
			timeCount = 0;
			while (digitalRead(DHT_PIN) == laststate) {
				timeCount++;
				delayMicroseconds(1);
				if (timeCount == 255) {
					break;
				}
			}
			laststate = digitalRead(DHT_PIN);

			if (timeCount == 255) break;

			// ignore first 3 transitions
			if ((i >= 2) && (i%2 == 0)) {
				// shove each bit into the storage bytes
				data[j/8] <<= 1;
				if (timeCount > 16)
					data[j/8] |= 1;
				j++;
			}
		}*/
		total++;

		//Verify data was sent by checksum calculate
		if(data[4] == data[0] + data[1] + data[2] + data[3])
		{
			//Convert celciut to F
			float f = data[2] * 9. / 5. + 32;
			printf("Humidity = %d.%d %% Temperature = %d.%d *C (%.1f *F)\n",
				data[0], data[1], data[2], data[3], f);
			fflush(stdout);
		}
		else
		{
			printf("Error. Checksum mismatch.\n data[0] = %d data[1] = %d data[2] = %d data[3] = %d data[4] = %d\n",
				data[0], data[1], data[2], data[3], data[4]);
			fflush(stdout);
			err++;
			printf("Error/Total: %d/%d", err, total);
		}
		
		//sleep(1);
		delay(1000);
	}
	
	exit(0);
}
