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

#include "WifiUartNetwork.h"

WifiUartNetwork::WifiUartNetwork () {
}

WifiUartNetwork::~WifiUartNetwork () {
}

struct WifiUartNetModeResponse WifiUartNetwork::getWirelessMode (WifiUart *wifi) {
    WifiUartCommand cmd = WifiUartCommand(wifi, WIFIU_ATP_WPRT);
    WifiUartResponse res = cmd.send();
	WifiUartNetModeResponse ret = { WIFIU_NET_RESPONSE_ERR, -1 };
    if (res.getCode() == WIFIU_OK) {
		ret.code = WIFIU_NET_RESPONSE_OK;
        ret.mode = atoi(res.getResponse());
    }
    return ret;
}

bool WifiUartNetwork::setWirelessMode(WifiUart *wifi, int mode) {
	char extra[13];
	itoa(mode, extra, 10);
    WifiUartCommand cmd = WifiUartCommand(wifi, WIFIU_ATP_WPRT, WIFIU_OP_SYNC, extra);
    WifiUartResponse res = cmd.send();
    return res.getCode() == WIFIU_OK;
}

struct WifiUartNetSSIDResponse WifiUartNetwork::getSSID (WifiUart *wifi) {
    WifiUartCommand cmd = WifiUartCommand(wifi, WIFIU_ATP_SSID);
    WifiUartResponse res = cmd.send();
	WifiUartNetSSIDResponse ret = { WIFIU_NET_RESPONSE_ERR, "" };
    if (res.getCode() == WIFIU_OK) {
		ret.code = WIFIU_NET_RESPONSE_OK;
		strncpy(ret.ssid, res.getResponse(), WIFIU_NET_SSIDRESPONSE_SSIDLENGTH);
    }
    return ret;
}
bool WifiUartNetwork::setSSID (WifiUart *wifi, char* ssid) {
    WifiUartCommand cmd = WifiUartCommand(wifi, WIFIU_ATP_SSID, WIFIU_OP_SYNC, ssid);
    WifiUartResponse res = cmd.send();
    return res.getCode() == WIFIU_OK;
}

struct WifiUartNetSecurityResponse WifiUartNetwork::getSecurity (WifiUart *wifi) {
	return getSecurity(wifi, true);
}

struct WifiUartNetSecurityResponse WifiUartNetwork::getSecurity (WifiUart *wifi, bool fetchKey) {
	WifiUartNetSecurityResponse ret = { WIFIU_NET_RESPONSE_ERR, -1, "", -1, -1 };
	{
		WifiUartCommand cmd = WifiUartCommand(wifi, WIFIU_ATP_ENCRY);
		WifiUartResponse res = cmd.send();
		if (res.getCode() == WIFIU_OK) {
			ret.code = WIFIU_NET_RESPONSE_OK;
			ret.encryptionMode = atoi(res.getResponse());
		}
	};
	if (ret.code == WIFIU_NET_RESPONSE_OK && fetchKey) {
		WifiUartCommand cmd = WifiUartCommand(wifi, WIFIU_ATP_KEY);
		WifiUartResponse res = cmd.send();
		if (res.getCode() == WIFIU_OK) {
			char tmpString[res.getResponseLength()];
			res.getResponseField(0, tmpString, res.getResponseLength());
			ret.keyFormat = atoi(tmpString);
			res.getResponseField(1, tmpString, res.getResponseLength());
			ret.wepIndex = atoi(tmpString);
			res.getResponseField(2, tmpString, res.getResponseLength());
			strncpy(ret.key, tmpString, WIFIU_NET_SECURITYRESPONSE_KEYLENGTH);
		}
		else {
			ret.code = WIFIU_NET_RESPONSE_ERR;
		}
	}
    return ret;
}

bool WifiUartNetwork::setSecurity(WifiUart *wifi, int encryptionMode, char *key) {
	return setSecurity(wifi, encryptionMode, key, WIFIU_NET_KEY_FORMAT_ASCII);
}

bool WifiUartNetwork::setSecurity(WifiUart *wifi, int encryptionMode, char *key, int keyFormat) {
	return setSecurity(wifi, encryptionMode, key, keyFormat, 1);
}

bool WifiUartNetwork::setSecurity(WifiUart *wifi, int encryptionMode, char *key, int keyFormat, int wepIndex) {
	bool ret = false;
	char tmpInt[13];
	itoa(encryptionMode, tmpInt, 10);
    WifiUartCommand cmd = WifiUartCommand(wifi, WIFIU_ATP_ENCRY, WIFIU_OP_SYNC, tmpInt);
    WifiUartResponse res = cmd.send();

	if (res.getCode() == WIFIU_OK) {
		char extra[1 + 1 + 1 + 1 + strlen(key) + 1];
		extra[0] = '\0';
		itoa(keyFormat, tmpInt, 10);
		strncat(extra, tmpInt, 1);
		strncat(extra, ",", 1);
		itoa(wepIndex, tmpInt, 10);
		strncat(extra, tmpInt, 1);
		strncat(extra, ",", 1);
		strncat(extra, key, strlen(key));
		WifiUartCommand cmd2 = WifiUartCommand(wifi, WIFIU_ATP_KEY, WIFIU_OP_SYNC, extra);
		WifiUartResponse res2 = cmd2.send();
		ret = (res2.getCode() == WIFIU_OK);
	}
    return ret;
}

struct WifiUartNetStateResponse WifiUartNetwork::getConnectionState(WifiUart *wifi) {
	return getConnectionState(wifi, true);
}

struct WifiUartNetStateResponse WifiUartNetwork::getConnectionState(WifiUart *wifi, bool fetchPhysical) {
    WifiUartCommand cmd = WifiUartCommand(wifi, WIFIU_ATP_LKSTT);
	WifiUartNetStateResponse ret = { WIFIU_NET_RESPONSE_ERR, -1, "", "", "", "", "" };
	{
		WifiUartResponse res = cmd.send();
		if (res.getCode() == WIFIU_OK) {
			ret.code = WIFIU_NET_RESPONSE_OK;
			char tmpString[res.getResponseLength()];
			res.getResponseField(0, tmpString, res.getResponseLength());
			//ret.isConnected = (atoi(tmpString) == WIFIU_NET_STATERESPONSE_ISCONNECTED_YES);
			ret.isConnected = (atoi(tmpString) == 1);
			res.getResponseField(1, tmpString, res.getResponseLength());
			strncpy(ret.ip, tmpString, WIFIU_NET_IPLENGTH);
			res.getResponseField(2, tmpString, res.getResponseLength());
			strncpy(ret.netmask, tmpString, WIFIU_NET_IPLENGTH);
			res.getResponseField(3, tmpString, res.getResponseLength());
			strncpy(ret.gateway, tmpString, WIFIU_NET_IPLENGTH);
			res.getResponseField(4, tmpString, res.getResponseLength());
			strncpy(ret.dns, tmpString, WIFIU_NET_IPLENGTH);
		}
	}
	if (ret.code == WIFIU_NET_RESPONSE_OK && fetchPhysical) {
		WifiUartCommand cmd = WifiUartCommand(wifi, WIFIU_ATP_QMAC);
		WifiUartResponse res = cmd.send();
		if (res.getCode() == WIFIU_OK) {
			strncpy(ret.physical, res.getResponse(), WIFIU_NET_MACLENGTH);
		}
		else {
			ret.code = WIFIU_NET_RESPONSE_ERR;
		}

	}
    return ret;
}

struct WifiUartNetSettingsResponse WifiUartNetwork::getNetworkSettings (WifiUart *wifi) {
	WifiUartNetSettingsResponse ret = { WIFIU_NET_RESPONSE_ERR, -1, "", "", "", "" };
	WifiUartCommand cmd = WifiUartCommand(wifi, WIFIU_ATP_NIP);
	WifiUartResponse res = cmd.send();
	if (res.getCode() == WIFIU_OK) {
		ret.code = WIFIU_NET_RESPONSE_OK;

		char tmpString[res.getResponseLength()];
		res.getResponseField(0, tmpString, res.getResponseLength());
		ret.staticIp = (atoi(tmpString) == 1);
		res.getResponseField(1, tmpString, res.getResponseLength());
		strncpy(ret.ip, tmpString, WIFIU_NET_IPLENGTH);
		res.getResponseField(2, tmpString, res.getResponseLength());
		strncpy(ret.netmask, tmpString, WIFIU_NET_IPLENGTH);
		res.getResponseField(3, tmpString, res.getResponseLength());
		strncpy(ret.gateway, tmpString, WIFIU_NET_IPLENGTH);
		res.getResponseField(4, tmpString, res.getResponseLength());
		strncpy(ret.dns, tmpString, WIFIU_NET_IPLENGTH);
	}
    return ret;
}

bool WifiUartNetwork::setNetworkSettingsToDHCP (WifiUart *wifi) {
	bool ret = false;
    WifiUartCommand cmd = WifiUartCommand(wifi, WIFIU_ATP_NIP, WIFIU_OP_SYNC, "0");
    WifiUartResponse res = cmd.send();
	if (res.getCode() == WIFIU_OK) {
		ret = true;
	}
    return ret;
}

bool WifiUartNetwork::setNetworkSettingsToStaticIP (WifiUart *wifi, char *ip, char *netmask, char *gateway, char *dns) {
	char extra[1 + 1 + (WIFIU_NET_IPLENGTH + 1)*4];
	extra[0] = '1';
	extra[1] = ',';
	extra[2] = '\0';
	strncat(extra, ip, WIFIU_NET_IPLENGTH);
	strncat(extra, ",", 1);
	strncat(extra, netmask, WIFIU_NET_IPLENGTH);
	strncat(extra, ",", 1);
	strncat(extra, gateway, WIFIU_NET_IPLENGTH);
	strncat(extra, ",", 1);
	strncat(extra, dns, WIFIU_NET_IPLENGTH);
	WifiUartCommand cmd = WifiUartCommand(wifi, WIFIU_ATP_NIP, WIFIU_OP_SYNC, extra);
	WifiUartResponse res = cmd.send();
    return (res.getCode() == WIFIU_OK);
}
