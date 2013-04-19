Arduino Weather Station

This arduino sketch allows the datalogging  DS18B20 temperature sensors and DHT11 temperature and humidity sensor. The data is logged on to SD, using sdfatlib in a .csv format. This type of file will open directly in excel, for easy creation of graphs and other data displays.

Can be run straight from Visual Studio, if using Visual Micro for Arduino in Visual studio
Otherwise, the .ino will run from the default Arduino IDE.

Depends on these libraries:

idDHT11.h

OneWire.h

DallasTemperature.h

OneWire.h

SdFat.h

Future versions:
Add support for the BMP085 barometer (With its temperature)
Allow any combination of sensors and any number to be detected automatically.


