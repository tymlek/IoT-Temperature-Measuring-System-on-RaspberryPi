// Tymoteusz Lekszycki
// 04.05.2016

#include <cstdio>
#include <cstring>
#include <errno.h>
#include <cmath>
#include <iostream>
#include <ctime>

#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include "mcp3008.hpp"

using namespace std;

#define HOST_ADDR "localhost"
#define USER_NAME "your username"
#define USER_PASSWD "your password"

int32_t getReading(MCP3008* adc, int32_t analogPin, double refVoltage) {
    int32_t temp = 0;
    int val = 0;
    
    val = adc->analogRead(analogPin);   // Two first readings discarted
    usleep(60000);                          // due tofluctuations
    val = adc->analogRead(analogPin);
    usleep(60000);
    val = adc->analogRead(analogPin);

    temp = round((refVoltage * 100.0 * val) / 1024);

    cout << "Current temperature: " << temp << "C" << endl;

    return temp;
}

void writeToDB(sql::Connection* con, int32_t reading) {
    sql::PreparedStatement* pstmt;
    
    pstmt = con->prepareStatement("insert into tempData "
            "(tDate,tTime,tLoc,temp) values "
            "(date(current_date()),time(current_time()),'Bedroom',?)");
    pstmt->setInt(1,reading);
    pstmt->executeUpdate();
    delete pstmt;
}

void readFromDB(sql::Connection * con) {
    sql::Statement* stmt;
    sql::ResultSet* res;
    
    stmt = con->createStatement();
    res = stmt->executeQuery("select * from tempData");
    delete stmt;

    while (res->next()) {
        cout << res->getString("tDate") << "\t" << res->getString("tTime")
                << "\t" << res->getString("tLoc") << "\t"
                << res->getInt("temp") << endl;
    }
    delete res;
}

int main ()
{
    sql::Driver* driver;
    sql::Connection* con;

    MCP3008* adc;
    const int32_t analogPin = 0;
    const int readingFrequency = 900;            // in seconds
    const int numOfReadings = 4;
    const double refVoltage = 3.29;             // in Volts

    try {
        adc = new MCP3008();
        
        // Create a connection
        driver = get_driver_instance();
        con = driver->connect(HOST_ADDR,USER_NAME,USER_PASSWD);
        
        // Connect to the MySQL test database
        con->setSchema("test");
    } catch (exception& e) {
        cerr << e.what() << endl;
        exit(0);
    }
    
    cout << "Running..." << endl;
    
    int reading = 0;
    reading = getReading(adc,analogPin,refVoltage);
    writeToDB(con,reading);
    
    for (int i = 0; i < (numOfReadings - 1); ++i) {
        sleep(readingFrequency);
        
        // Get reading from sensor
        reading = getReading(adc,analogPin,refVoltage);
        
        // Execute query
        writeToDB(con,reading);
    }
    
    delete con;
    delete adc;

    return EXIT_SUCCESS;
}
