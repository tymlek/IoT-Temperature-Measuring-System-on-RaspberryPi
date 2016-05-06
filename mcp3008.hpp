/* 
 * File:   mcp3008.hpp
 * Author: Tymoteusz Lekszycki
 *
 * Created on 06 May 2016, 10:24
 */

#ifndef MCP3008_HPP
#define	MCP3008_HPP

#include <wiringPiSPI.h>
#include <cstdint>
#include <stdexcept>

#define buffSize 3

class MCP3008 {
private:
    int32_t channel;
    int32_t speed;
    uint8_t spiData[buffSize];
    
public:
    MCP3008()
        : channel(0), speed(10000)              // 10 KHz
    {   
        if (wiringPiSPISetup(channel,speed) < 0) {
            throw std::runtime_error("Cannot connect to spi device");
        }
    }
    
    MCP3008(int32_t channel, int32_t speed)
        : channel(channel), speed(speed)
    {
        if (wiringPiSPISetup(channel,speed) < 0) {
            throw std::runtime_error("Cannot connect to spi device");
        }
    }
    
    int32_t analogRead(int32_t analogInPin) {
        uint8_t command;
        
        command = (0x80 | (analogInPin << 4));
        
        spiData[0] = 1;
        spiData[1] = command;
        spiData[2] = 0;
        
        wiringPiSPIDataRW(channel,spiData,buffSize);
        
        return (((spiData[1] << 8) | spiData[2]) & 0x3ff);
    }
};

#endif	/* MCP3008_HPP */

