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
#include <WifiUartNetwork.h>
#include <WifiUartTransparent.h>
#include <WifiUartMisc.h>

#define WIFIU_HELPER_RESPONSE_MODIFIED 1
#define WIFIU_HELPER_RESPONSE_OK 0
#define WIFIU_HELPER_RESPONSE_ERR -1

class WifiUartHelper {
    public:
        WifiUartHelper ();
        ~WifiUartHelper ();

        // Network
        // setWirelessMode, setSSID, setSecurity
        static int initWireless (
            WifiUart *wifi,
            int mode,
            char *ssid,
            int encryptionMode,
            char *key,
            int keyFormat,
            int wepIndex);
        // setNetworkSettingsToDHCP
        static int initNetworkDHCP (WifiUart *wifi);
        // setNetworkSettingsToStaticIP
        static int initNetworkStaticIP (WifiUart *wifi, char *ip, char *netmask, char *gateway, char *dns);

        // Transparent
        // setClient
        static int initClient(WifiUart *wifi, char *serverName, int port, int protocol);
        // setServer
        static int initServer(WifiUart *wifi, int port, int protocol, int timeout);

        // Misc
        // setWebServiceStatus, setSystemPassword
        static int initMisc(WifiUart *wifi, bool enableWebService, int port, char *password);

        // Debug
        static void printWirelessSettings (WifiUart *wifi, Stream *outSerial);
        static void printNetworkSettings (WifiUart *wifi, Stream *outSerial);
        static void printTransparentSettings (WifiUart *wifi, Stream *outSerial);
        static void printWebServiveSettings (WifiUart *wifi, Stream *outSerial);
};

