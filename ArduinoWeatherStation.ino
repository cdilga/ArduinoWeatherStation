/*
Arduino Weather Station

Please install nesessary headers in the arduino<Version>/libraries folder
More in formation in the README

Next version:
Add switch for ledError codes
Make more codes, including a halt code (No led on)
When there is an error from one sensor, but is still able to function, flash continuously
Add support for full float values to be printed onto the SD card

Code by Chris Dilger
Hosted on GitHub
*/
#include <idDHT11.h>

#include <OneWire.h>

#include <DallasTemperature.h>

#include <SdFat.h>

// Data wire is plugged into pin 8 on the Arduino
#define ONE_WIRE_BUS 8
#define TEMPERATURE_PRECISION 9

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress thermometer0, thermometer1;

int idDHT11pin = 2; //Digital pin for comunications
int idDHT11intNumber = 0; //interrupt number (Which number interupt) i.e. 2 on Uno is interrupt 0

// Lib declaration
idDHT11 DHT11(idDHT11pin,idDHT11intNumber,dht11_wrapper);

//SdFatLib declare file and card
SdFat sd;
SdFile logFile;

//Define the chipSelect pin number
const byte chipSelect = 7;

//The pin number the LED is connected to 
int ledPin = 6;
//Declare wrapper for isrCallback function
void dht11_wrapper()
{
	DHT11.isrCallback();
}

//The error display function
//This is using an led that will change colours and if reset, will start again from red
//Fix this, led does not indicate, rather keep on shinig if there is an error
void ledError(int errorCode)
{
	if(errorCode = NULL)
	{
		digitalWrite(ledPin, HIGH);
	}else if(errorCode = 1)
	{
		digitalWrite(ledPin, LOW);
	}
}

void setup()
{
	//Initialise Serial connection 115200
	Serial.begin(115200);

	//Setup status led
	pinMode(ledPin, OUTPUT);
	ledError(NULL);
	//digitalWrite(ledPin, HIGH);

	// Initialise the DS18B20 thermometer library
	sensors.begin();

	//Set resouution of the DS18B20s to 12 bit
	sensors.setResolution(thermometer0, TEMPERATURE_PRECISION);
	sensors.setResolution(thermometer1, TEMPERATURE_PRECISION);

	//Set software CS pin as output
	pinMode(chipSelect, OUTPUT);

	//Set the hardware CS pin as an output. Must be set as output or SdFatLib will not work
	pinMode(10, OUTPUT);

	//Begin sdfat with chipselectpin at full speed
	if(!sd.begin(chipSelect, SPI_FULL_SPEED))
	{
		ledError(1);
	}
	// open the file for write headings at start
	if (!logFile.open("WTHR.CSV", O_RDWR | O_CREAT | O_AT_END)) 
	{
		//Display error on the led
		ledError(1);
	}else{
	logFile.println("Temperature sensor 1,Temperature *C sensor 1,Temperature sensor 2,Temperature *C sensor 2,Temperature sensor 3,Temperature *C sensor 3,Humidity %,Temperature sensor 4,Temperature *C sensor 4,Barometric pressure");
	//Write the changes to the file and close it
	logFile.close();
	}
	//Get addresses of the thermometers
	if(!sensors.getAddress(thermometer0, 0))
	{
			
	}
	if(!sensors.getAddress(thermometer1, 1))
	{
			
	}
	
}

void loop()
{
	//Get temperature readings for OneWire devices
	sensors.requestTemperatures();
	//Set logStgring to null character
	String logString = "";

	//Reset the float value to hold tempoary temperature values
	float temperatureDegC = 0.00;

	//Add device address of sensor 0 to logString, and comma at end
	//logString += String(tempSensorAddress(thermometer0));
	logString += ",";

	//Read temperature value and add to the logString
	temperatureDegC = sensors.getTempC(thermometer0);

	//Convert to an int, and add the cell
	logString += int(temperatureDegC);
	logString += ",";

	//Reset the temperatureDegC
	temperatureDegC = 0.00;

	//Repeat with sensor 1
	logString += String(tempSensorAddress(thermometer1));
	logString += ",";
	temperatureDegC = sensors.getTempC(thermometer1);
	logString += int(temperatureDegC);
	logString += ",";
	
	//Aquire DHT11 data, then notify of any errors and add to logstring
	DHT11.acquire();
	while (DHT11.acquiring());
	int status = DHT11.getStatus();
	if (status != IDDHTLIB_OK)
	{
		ledError(1);
	}
	logString += "DHT11,";
	logString += int(DHT11.getCelsius());
	logString += ",";
	logString += int(DHT11.getHumidity());
	logString += ",";

	//Currently BMP085 unsupported, add a string with value of "NULL" to all relevant BMP085 values
	logString += "NULL,NULL,NULL";
	
	// open the file for write at end like the Native SD library
	if (!logFile.open("WTHR.CSV", O_RDWR | O_CREAT | O_AT_END)) {
		ledError(1);
	}else{
	ledError(NULL);
	logFile.println(logString);
	logFile.close();
	}
	delay(100);
}


// function to print a device address (used from multiple example)
String tempSensorAddress(DeviceAddress deviceAddress)
{
	String hexDeviceAddress = "";
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) hexDeviceAddress += "0";
    hexDeviceAddress += deviceAddress[i];
  }
  return hexDeviceAddress;
}

//Function for convering float values to string type
/*
String floatString(float temp, uint8_t decimalPlaces)
{
	String floatStringReturn = "";
	floatStringReturn += int(temp);
	floatStringReturn += ".";
	//This only works because of the way arduino uses floating point numbers
	floatStringReturn += int((10 ^ decimalPlaces) * (temp - int(temp)));
	return floatStringReturn;
}
*/

