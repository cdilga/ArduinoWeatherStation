//
#include <idDHT11.h>

#include <OneWire.h>

#include <DallasTemperature.h>

#include <SdFat.h>

// Data wire is plugged into pin 8 on the Arduino
#define ONE_WIRE_BUS 8
#define TEMPERATURE_PRECISION 12

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

//A float value to hold tempoary temperature values
float temperatureDegC;
//Declare wrapper for isrCallback function
void dht11_wrapper()
{
	DHT11.isrCallback();
}

void setup()
{
	//Initialise Serial connectionat 9600
	Serial.begin(9600);

	// Initialise the DS18B20 thermometer library
	sensors.begin();

	//Set software CS pin as output
	pinMode(chipSelect, OUTPUT);

	//Set the hardware CS pin as an output. Must be set as output or SdFatLib will not work
	pinMode(10, OUTPUT);

	//Begin sdfat with chipselectpin at full speed
	if(!sd.begin(chipSelect, SPI_FULL_SPEED))
	{
		
		ledError();
			
	}
	// open the file for write headings at end 
	if (!logFile.open("weatherLog.csv", O_RDWR | O_CREAT | O_AT_END)) {
		//Display error on the led
		ledError();
	}
	logFile.println("Number of One Wire devices,Temperature sensor 1,Temperature *C sensor 1,Temperature sensor 2,Temperature *C sensor 2,Temperature sensor 3,Temperature *C sensor 3,Humidity %,Temperature sensor 4,Temperature *C sensor 4,Barometric pressure");
}

void loop()
{
	//Set logStgring to null character
	String logString = "";

	//Add device address of sensor 0 to logString, and comma at end
	sensors.getAddress(thermometer0, 0);
	logString += tempSensorAddress(thermometer0), ",";

	//Read temperature value and add to the logString
	temperatureDegC = sensors.getTempC(thermometer0);

	//Call the conversion function to convert float -> string with 4 decimal places and add to logstring
	logString += floatString(temperatureDegC, 4), ",";
	
	//Repeat with sensor 2
	sensors.getAddress(thermometer1, 0);
	logString += tempSensorAddress(thermometer1), ",";
	temperatureDegC = sensors.getTempC(thermometer1);
	logString += floatString(temperatureDegC, 4), ",";
	
	//Aquire DHT11 data, then notify of any errors and add to logstring
	DHT11.acquire();
	while (DHT11.acquiring());
	if (!DHT11.getStatus() = IDDHTLIB_OK)
	{
		ledError();
	}else{
		ledError(NULL);
	}
	logString += "DHT11,", DHT11.getCelsius(), ",", DHT11.getHumidity(), ",";

	//Currently BMP085 unsupported, add a string with value of "NULL" to all relevant BMP085 values
	logString += "NULL,NULL,NULL";
	
	// open the file for write at end like the Native SD library
	if (!logFile.open("weatherLog.csv", O_RDWR | O_CREAT | O_AT_END)) {
		ledError();
	}
	logFile.println(logString);
	logFile.close();

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

String floatString(float temp, uint8_t decimalPlaces)
{
	String floatStringReturn = "";
	floatStringReturn += int(temp);
	floatStringReturn += ".";
	floatStringReturn += int((10 ^ decimalPlaces) * (temp - int(temp)));
	return floatStringReturn;
}
	