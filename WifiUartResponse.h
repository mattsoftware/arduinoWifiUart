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
#include "WifiUart.h"

#define WIFIU_RESPONSE_BUFFER_LENGTH 128 // Lets hope this is enough for most cases
#define WIFIU_RESPONSE_DEFAULT_TIMEOUT 10000

#define WIFIU_OK 0
#define WIFIU_ERR_FORMAT -1 // Ineffective command format
#define WIFIU_ERR_SUPPORT -2 // not supported
#define WIFIU_ERR_OPERATION -3 // Ineffective Operation Symbol
#define WIFIU_ERR_PARAMETER -4 // Ineffective Parameter
#define WIFIU_ERR_PERMITTED -5 // Not permitted
#define WIFIU_ERR_MEMORY -6 // Memory lack
#define WIFIU_ERR_FLASH -7 // Flash error
#define WIFIU_ERR_JOIN -10 // Join Failed
#define WIFIU_ERR_SOCKET_NA -11 // No available socket
#define WIFIU_ERR_SOCKET -12 // Ineffective Socket
#define WIFIU_ERR_SOCKET_CONNECT -13 // Socket Connect Fail
#define WIFIU_ERR_UNDEFINED -100 // Undefined
#define WIFIU_ERR_UNKNOWN -101 // Unknown
#define WIFIU_ERR_TIMEDOUT -102 // Timed out
#define WIFIU_ERR_INCOMPLETE -103 // Have not finished

class WifiUart;
class WifiUartResponse {
    public:
        WifiUartResponse (WifiUart *wifi);
        ~WifiUartResponse ();
        bool receive();

        int getCode();
        char* getResponse();
        uint16_t getResponseLength();

		void getResponseField(int index, char* buffer, int length);
		void getResponseField(int index, char* buffer, int length, char separator);

    private:
        WifiUart *_wifi;
        int _code;
        char *_response;
        uint16_t _length;
        char _receiveBuffer[WIFIU_RESPONSE_BUFFER_LENGTH];
        uint16_t _receiveBufferLength;
        int _receiveState;

        void _receiveChar(char c);
        void _cleanReceiveBuffer();
};
