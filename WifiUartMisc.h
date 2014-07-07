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

#define WIFIU_MISC_RESPONSE_OK 0
#define WIFIU_MISC_RESPONSE_ERR -1
#define WIFIU_MISC_PASSLENGTH 6
struct WifiUartMiscPasswordResponse {
    int code;
    char password[WIFIU_MISC_PASSLENGTH+1];
};
struct WifiUartMiscWebServiceResponse {
    int code;
    bool isEnabled;
    int port;
};

class WifiUartMisc {
    public:
        WifiUartMisc ();
        ~WifiUartMisc ();

        static struct WifiUartMiscPasswordResponse getSystemPassword (WifiUart *wifi); // PASS
        static bool setSystemPassword (WifiUart *wifi, char *newPass);

        static struct WifiUartMiscWebServiceResponse getWebServiceStatus (WifiUart *wifi); // WEBS
        static bool setWebServiceStatus(WifiUart *wifi, bool status, int port);

 // #define WIFIU_ATP_NULL "" // Null command
 // #define WIFIU_ATP_ENTS "ENTS" // Enter sleeping mode
 // #define WIFIU_ATP_IOC "IOC" // GPIO Control
 // #define WIFIU_ATP_IOM "IOM" // Set or query GPIO mode
 // #define WIFIU_ATP_PMTF "PMFT" // Update parameters in memory to flash
 // #define WIFIU_ATP_QVER "QVER" // Query version

};
