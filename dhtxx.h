#ifndef DHTxx_H
#define DHTxx_H

//Define WiringPi features
#define WIRINGPI_CODES 1

#define WIRINGPISETUP 1
#define WIRINGPISETUPGPIO 2
#define WIRINGPISETUPPHYS 3
#define WIRINGPISETUPSYS 4

#define WIRINGPI_INIT_MODE WIRINGPISETUPGPIO

/* GPIO pin (BCM numbering) that DHTxx connect
 * use cmd "gpio readall" to see gpio mapping */
#if WIRINGPI_INIT_MODE == WIRINGPISETUP
	#define DHT_PIN 0
#elif WIRINGPI_INIT_MODE == WIRINGPISETUPGPIO || WIRINGPI_INIT_MODE == WIRINGPISETUPSYS
	#define DHT_PIN 17
#elif WIRINGPI_INIT_MODE == WIRINGPISETUPPHYS
	#define DHT_PIN 11
#endif

// Define errors and return values.
#define DHT_ERROR_TIMEOUT -1
#define DHT_ERROR_CHECKSUM -2
#define DHT_ERROR_READ_DATA -3
#define DHT_ERROR_GPIO -4
#define DHT_SUCCESS 0

// Define sensor types.
//#define DHT11 11
#define DHT22 22
//#define AM2302 22

#ifdef DHT11
	//pull down at least 18ms
	#define startSignalTime 20000
	#define collectingPeriodTime 1
	#define sensor 11
#elif defined(DHT22) || defined(AM2303)
	//pull down at least 1-10ms
	#define startSignalTime 10000
	#define collectingPeriodTime 2
	#define sensor 22
#endif

//Init GPIO Library (Wiring Py)
int dhtxx_init(void);

// Read DHT sensor connected to GPIO pin (using BCM numbering).  Humidity and temperature will be 
// returned in the provided parameters. If a successfull reading could be made a value of 0 
// (DHT_SUCCESS) will be returned.  If there was an error reading the sensor a negative value will
// be returned.  Some errors can be ignored and retried, specifically DHT_ERROR_TIMEOUT or DHT_ERROR_CHECKSUM.
int readSensor(char *data);

#endif
