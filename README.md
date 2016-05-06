# IoT-Temperature-Measuring-System-on-RaspberryPi
The system measures temperature and saves results in MySQL database

The database requires a table with tDate - Date, tTime - Time, tLoc - Text and temp - Int fields.

The system uses RaspberryPi v3, MCP3008 ADC and LM35 temperature sensor.

To compile the program libmysqlclient-dev, libmysqlcppconnect and wiringPi are required.
