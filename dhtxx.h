#ifndef DHTxx_H
#define DHTxx_H

//GPIO pin (BCM numbering) that DHTxx connect
#define DHT_PIN 17

// Define errors and return values.
#define DHT_ERROR_TIMEOUT -1
#define DHT_ERROR_CHECKSUM -2
#define DHT_ERROR_ARGUMENT -3
#define DHT_ERROR_GPIO -4
#define DHT_SUCCESS 0

// Define sensor types.
#define DHT11 11
#define DHT22 22
#define AM2302 22

// Read DHT sensor connected to GPIO pin (using BCM numbering).  Humidity and temperature will be 
// returned in the provided parameters. If a successfull reading could be made a value of 0 
// (DHT_SUCCESS) will be returned.  If there was an error reading the sensor a negative value will
// be returned.  Some errors can be ignored and retried, specifically DHT_ERROR_TIMEOUT or DHT_ERROR_CHECKSUM.
int dht_read(char *data);

#endif
