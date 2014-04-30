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

#define WIFIU_TRANS_RESPONSE_OK 0
#define WIFIU_TRANS_RESPONSE_ERR -1

#define WIFIU_TRANS_SERVERLENGTH 15 // This can be bumped up if needed, minimum 15
#define WIFIU_TRANS_DEFAULT_TIMEOUT 120

#define WIFIU_TRANS_PROTO_TCP 0
#define WIFIU_TRANS_PROTO_UDP 1
#define WIFIU_TRANS_CS_CLIENT 0
#define WIFIU_TRANS_CS_SERVER 1
struct WifiUartTransDetailsResponse {
    int code;
    bool isClient;
    // serverName only if isClient == true
    char serverName[WIFIU_TRANS_SERVERLENGTH+1];
    int port;
    int protocol;
    // connectionTimeout only if isClient == false
    int connectionTimeout;
};

class WifiUartTransparent {
    public:
        WifiUartTransparent ();
        ~WifiUartTransparent ();

        static struct WifiUartTransDetailsResponse getConnectionDetails (WifiUart *wifi); // ATRM
        static bool setClient(WifiUart *wifi, char *serverName, int port);
        static bool setClient(WifiUart *wifi, char *serverName, int port, int protocol);
        static bool setServer(WifiUart *wifi, int port);
        static bool setServer(WifiUart *wifi, int port, int protocol, int timeout);

//#define WIFIU_ATP_ATLT "ATLT" // Set or query data length of automatic framing
//#define WIFIU_ATP_ATPT "ATPT" // Set or query period of automatic framing
//#define WIFIU_ATP_WARC "WARC" // Set or query automatic retry times in auto-work mode
};
