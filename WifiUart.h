/*
The MIT License (MIT)

Copyright (c) 2014 Matt Paine

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

using namespace std;

#include <Arduino.h>
#include <WifiUartCommand.h>
#include <WifiUartResponse.h>

#define WIFIU_DEFAULT_UART 9600 // Manual says 114200 but thats just wrong!

class WifiUart {
    public:
        WifiUart (Stream *serial);
        WifiUart (Stream *serial, bool isAlreadyInCommandMode);
        ~WifiUart ();

        void setEscapeCharacter (char character);
        void setEscapeTimeout (uint16_t milliseconds);
        void setEscapeTimeoutBuffer (uint16_t milliseconds);
        void setAutoFramingPeriod (uint16_t milliseconds);
        uint16_t getCommandTimeout();
        void setCommandTimeout (uint16_t milliseconds);
        Stream* getSerial();
        bool isInCommandMode();
        bool getIsSending();
        void setIsSending();
        void clearIsSending();
		bool setHardwareResetPin(short pin, bool init);

        bool enterCommandMode();
        bool exitCommandMode();
        bool enterManualMode(); // ENTM

        bool factoryReset(); // RSFT
        bool reset(); //  Z
		bool hardwareReset();

// #define WIFIU_ATP_ATM "ATM" // Set or query work mode of module
// #define WIFIU_ATP_CMDM "CMDM" // Set or query default command mode of system
// #define WIFIU_ATP_ESPC "ESPC" // Set or query escape character
// #define WIFIU_ATP_ESPT "ESPT" // Set or query escape time
// #define WIFIU_ATP_UART "UART" // Set or query serial data format

    private:
        bool _isInCommandMode;
        char _escapeCharacter;
        uint16_t _escapeTimeout;
        uint16_t _escapeTimeoutBuffer;
        uint16_t _autoFramingPeriod;
        uint16_t _commandTimeout;
        Stream *_serial;
        bool _isSending;
		short _hardwareResetPin;
		uint16_t _hardwareResetDelay;

        void _init (Stream *serial);
        void _init (Stream *serial, bool isAlreadyInCommandMode);
};
