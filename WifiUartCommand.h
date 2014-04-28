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
#include "WifiUartResponse.h"

#define WIFIU_OP_NONE ""
#define WIFIU_OP_COMMAND "="
#define WIFIU_OP_SYNC "=!"
#define WIFIU_OP_QUERY "=?"

// AT+ commands
// Admin commands (WifiUart.h)
#define WIFIU_ATP_ATM "ATM" // Set or query work mode of module
#define WIFIU_ATP_E "E" // Switch serial character to show back
#define WIFIU_ATP_CMDM "CMDM" // Set or query default command mode of system
#define WIFIU_ATP_ENTM "ENTM" // Set module into transparent transmission mode
#define WIFIU_ATP_ESPC "ESPC" // Set or query escape character
#define WIFIU_ATP_ESPT "ESPT" // Set or query escape time
#define WIFIU_ATP_RSTF "RSTF" // Return to manufacturer setting
#define WIFIU_ATP_UART "UART" // Set or query serial data format
#define WIFIU_ATP_Z "Z" // Reset
// Network based commands (WifiUartNework.h)
#define WIFIU_ATP_BSSID "BSSID" // Set or query bssid address of target AP
#define WIFIU_ATP_CHL "CHL" // Set or query target wirless channel number
#define WIFIU_ATP_CHLL "CHLL" // Set or query wireless channel list
#define WIFIU_ATP_ENCRY "ENCRY" // Set or query security mode of wireless network
#define WIFIU_ATP_KEY "KEY" // Set or query key of wireless network
#define WIFIU_ATP_LKSTT "LKSTT" // Query network connection state
#define WIFIU_ATP_NIP "NIP" // Set or query local ip address
#define WIFIU_ATP_QMAC "QMAC" // Query physical address
#define WIFIU_ATP_SSID "SSID" // Set or query SSID
#define WIFIU_ATP_WARM "WARM" // Set or query wireless roam
#define WIFIU_ATP_WATC "WATC" // Set or query whether to create AdHoc network automatically
#define WIFIU_ATP_WBGR "WBGR" // Set or query wireless BG mode and maximum send rate
#define WIFIU_ATP_WJOIN "WJOIN" // Join or create network
#define WIFIU_ATP_WLEAV "WLEAV" // Disconnect network
#define WIFIU_ATP_WPRT "WPRT" // Set or query wireless mode
#define WIFIU_ATP_WSCAN "WSCAN" // Scan
// Transparent Mode Commands (WifiUartTransparent.h)
#define WIFIU_ATP_ATLT "ATLT" // Set or query data length of automatic framing
#define WIFIU_ATP_ATPT "ATPT" // Set or query period of automatic framing
#define WIFIU_ATP_ATRM "ATRM" // Set or query connection message of socket module creates automatically in auto-work mode
#define WIFIU_ATP_WARC "WARC" // Set or query automatic retry times in auto-work mode
// Socket communication (WifiUartSocket.h)
#define WIFIU_ATP_SKCLS "SKCLS" // Close socket connection
#define WIFIU_ATP_SKCT "SKCT" // Create socket connection
#define WIFIU_ATP_SKRCV "SKRCV" // Receive data through socket connection
#define WIFIU_ATP_SKSDF "SKSDF" // Set default sending socket connection
#define WIFIU_ATP_SKSND "SKSND" // send data through socket connection
#define WIFIU_ATP_SKSTT "SKSTT" // query socket connection state
// Misc functions (WifiUartMisc.h)
#define WIFIU_ATP_NULL "" // Null command
#define WIFIU_ATP_ENTS "ENTS" // Enter sleeping mode
#define WIFIU_ATP_IOC "IOC" // GPIO Control
#define WIFIU_ATP_IOM "IOM" // Set or query GPIO mode
#define WIFIU_ATP_PASS "PASS" // Set or query system password
#define WIFIU_ATP_PMTF "PMFT" // Update parameters in memory to flash
#define WIFIU_ATP_QVER "QVER" // Query version
#define WIFIU_ATP_WEBS "WEBS" // Set or query web service

class WifiUart;
class WifiUartResponse;

class WifiUartCommand {
    public:
        WifiUartCommand (WifiUart *wifi, char *cmd);
        WifiUartCommand (WifiUart *wifi, char *cmd, char *op, char *extra);
        ~WifiUartCommand ();

        WifiUartResponse send ();

    private:
        WifiUart *_wifi;
        char *_command;
        char *_operation;
        char *_extra;

        void _init(WifiUart *wifi, char *cmd);
        void _init(WifiUart *wifi, char *cmd, char *op, char *extra);
};
