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
#include <WifiUart.h>

#define WIFIU_SOCKET_RESPONSE_OK 0
#define WIFIU_SOCKET_RESPONSE_ERR -1

//struct WifiUartNetSSIDResponse {
//	int code;
//	char* ssid;
//};

class WifiUartNetwork {
    public:
        WifiUartSocket ();
        ~WifiUartSocket ();

//		static struct WifiUartNetModeResponse getWirelessMode (WifiUart *wifi); // WPRT
//		static bool setWirelessMode(int mode);

//#define WIFIU_ATP_ATLT "ATLT" // Set or query data length of automatic framing
//#define WIFIU_ATP_ATPT "ATPT" // Set or query period of automatic framing
//#define WIFIU_ATP_ATRM "ATRM" // Set or query connection message of socket module creates automatically in auto-work mode
//#define WIFIU_ATP_WARC "WARC" // Set or query automatic retry times in auto-work mode
};
