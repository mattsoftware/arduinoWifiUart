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

#define WIFIU_NET_RESPONSE_OK 0
#define WIFIU_NET_RESPONSE_ERR -1

#define WIFIU_NET_MODE_INFRASTRUCTURE 0
#define WIFIU_NET_MODE_ADHOC 1

#define WIFIU_NET_ENC_OPEN 0
#define WIFIU_NET_ENC_WEP64 1
#define WIFIU_NET_ENC_WEP128 2
#define WIFIU_NET_ENC_WPA 3
#define WIFIU_NET_ENC_WPA_AES 4
#define WIFIU_NET_ENC_WPA2 5
#define WIFIU_NET_ENC_WPA2_AES 6
// NOT SUPPORTED #define WIFIU_ENC_NET_WAPI

#define WIFIU_NET_KEY_FORMAT_HEX 0
#define WIFIU_NET_KEY_FORMAT_ASCII 1
#define WIFIU_NET_ATRM_TCP 0
#define WIFIU_NET_ATRM_UDP 1
#define WIFIU_NET_ATRM_CLIENT 0
#define WIFIU_NET_ATRM_SERVER 1

struct WifiUartNetModeResponse {
	int code;
	int mode;
};
#define WIFIU_NET_SSIDRESPONSE_SSIDLENGTH 32
struct WifiUartNetSSIDResponse {
	int code;
	// 1 to 32 characters
	char ssid[WIFIU_NET_SSIDRESPONSE_SSIDLENGTH+1];
};
#define WIFIU_NET_SECURITYRESPONSE_KEYLENGTH 64
struct WifiUartNetSecurityResponse {
	int code;
	int encryptionMode;
	//  If mode = WEP64... HEX = 10 chars, ASCII = 5 chars
	//  If mode = WEP128... HEX = 26, ASCII = 13
	//  If mode = WPA*... HEX = 64, ASCII = 8~63
	char key[WIFIU_NET_SECURITYRESPONSE_KEYLENGTH+1];
	int keyFormat; // WIFIU_NET_KEY_FORMAT_*
	int wepIndex; // number between 0 and 4 inclusive
};
#define WIFIU_NET_IPLENGTH 15
#define WIFIU_NET_MACLENGTH 12
#define WIFIU_NET_STATERESPONSE_ISCONNECTED_YES 1
struct WifiUartNetStateResponse {
	int code;
	bool isConnected;
	char ip[WIFIU_NET_IPLENGTH + 2];
	char netmask[WIFIU_NET_IPLENGTH + 1];
	char gateway[WIFIU_NET_IPLENGTH + 1];
	char dns[WIFIU_NET_IPLENGTH + 1];
	char physical[WIFIU_NET_MACLENGTH + 1];
};
struct WifiUartNetSettingsResponse {
	int code;
	bool staticIp;
	char ip[WIFIU_NET_IPLENGTH + 1];
	char netmask[WIFIU_NET_IPLENGTH + 1];
	char gateway[WIFIU_NET_IPLENGTH + 1];
	char dns[WIFIU_NET_IPLENGTH + 1];
};

class WifiUartNetwork {
    public:
        WifiUartNetwork ();
        ~WifiUartNetwork ();

		static struct WifiUartNetModeResponse getWirelessMode (WifiUart *wifi); // WPRT
		static bool setWirelessMode(WifiUart *wifi, int mode);

		static struct WifiUartNetSSIDResponse getSSID (WifiUart *wifi); // SSID
		static bool setSSID (WifiUart *wifi, char* ssid);

		static struct WifiUartNetSecurityResponse getSecurity (WifiUart *wifi); // ENCRY, KEY
		static struct WifiUartNetSecurityResponse getSecurity (WifiUart *wifi, bool fetchKey);
		static bool setSecurity(WifiUart *wifi, int encryptionMode, char *key);
		static bool setSecurity(WifiUart *wifi, int encryptionMode, char *key, int keyFormat);
		static bool setSecurity(WifiUart *wifi, int encryptionMode, char *key, int keyFormat, int wepIndex);

		static struct WifiUartNetStateResponse getConnectionState(WifiUart *wifi); // LKSTT, QMAC
		static struct WifiUartNetStateResponse getConnectionState(WifiUart *wifi, bool fetchPhysical);

		static struct WifiUartNetSettingsResponse getNetworkSettings (WifiUart *wifi); // NIP
		static bool setNetworkSettingsToDHCP (WifiUart *wifi);
		static bool setNetworkSettingsToStaticIP (WifiUart *wifi, char *ip, char *netmask, char *gateway, char *dns);

//#define WIFIU_ATP_BSSID "BSSID" // Set or query bssid address of target AP
//#define WIFIU_ATP_CHL "CHL" // Set or query target wirless channel number
//#define WIFIU_ATP_CHLL "CHLL" // Set or query wireless channel list
//#define WIFIU_ATP_WARM "WARM" // Set or query wireless roam
//#define WIFIU_ATP_WATC "WATC" // Set or query whether to create AdHoc network automatically
//#define WIFIU_ATP_WBGR "WBGR" // Set or query wireless BG mode and maximum send rate
//#define WIFIU_ATP_WJOIN "WJOIN" // Join or create network
//#define WIFIU_ATP_WLEAV "WLEAV" // Disconnect network
//#define WIFIU_ATP_WSCAN "WSCAN" // Scan
};
