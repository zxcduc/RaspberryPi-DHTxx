#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "dhtxx.h"

int main()
{
	//Print err and exit if init GPIO fail
	if(dhtxx_init())
	{
		printf("Error, can not init GPIO by wiringPi. Exit...");
		fflush(stdout);
		exit(-1);
	}

	char data[5] = {0,0,0,0,0};

	//Create file I/O
	FILE *fp;

	while(1)
	{
		//Get error result when read sensor
		int result = readSensor(data);
		
		if(result == DHT_SUCCESS)
		{
			float humidity, temperature, f;
			if(sensor == 11)
			{
				//Convert celciut to F
				humidity = (data[0] << 8 | data[1]) / 10.0f;
				temperature = (data[2] << 8 | data[3]) / 10.0f;
				f = (data[2] << 8) * 9. / 5. + 32;
			}
			else if(sensor == 22)
			{
				humidity = (data[0] * 256 + data[1]) / 10.0f;
				temperature = ((data[2] & 0x7F) * 256 + data[3]) / 10.0f;
				//Convert celciut to F
				f = temperature * 9. / 5. + 32;
			}
			
			#ifdef DEBUG
				printf("Humidity = %f %% Temperature = %f *C (%.1f *F)\n",
					humidity, temperature, f);
				fflush(stdout);
			#endif
			/*fputs(strcat((char) (int) data[0] + '.' + (char) (int) data[1] + '\n'
				+ (char) (int) data[2] + '.' + (char) (int) data[3] + '\n'
				+ '1' + '\n', fp);*/
			
			//Write to file
			fp = fopen(".dhtxxsensor", "w+");
			fprintf(fp, "%f\n%f\n1\n", humidity, temperature);
			fclose(fp);
		}
		else if(result == DHT_ERROR_CHECKSUM)
		{
			#ifdef DEBUG
				printf("Error. Checksum mismatch.\n");
				fflush(stdout);
			#endif
			//Open file and read last RH and Temperature data
			fp = fopen(".dhtxxsensor", "r");
			if(fp == NULL) printf("Error when open file to read\n");
			char oldTemp[12], oldRH[12];
			
			#ifdef DEBUG
				if(fgets(oldRH, 12, fp) == NULL) printf("Error when read file");
				if(fgets(oldTemp, 12, fp) == NULL) printf("Error when read Temp in file");
				printf("Old RH: %s\tOld Temerature: %s\n", oldRH, oldTemp);
			#else
				fgets(oldRH, 12, fp);
				fgets(oldTemp, 12, fp);
			#endif
			
			fclose(fp);
			
			//Write last result to file with flag 0 (fail)
			fp = fopen(".dhtxxsensor", "w");
			fprintf(fp, "%s%s0\n", oldRH, oldTemp);
			fclose(fp);
		}
		else
		{
			#ifdef DEBUG
				printf("Error when reading. Error code %d\n", result);
				fflush(stdout);
			#endif
		}
		
		sleep(collectingPeriodTime);
	}

	exit(0);
}
