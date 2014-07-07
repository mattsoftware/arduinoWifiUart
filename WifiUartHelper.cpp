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

#include "WifiUartHelper.h"

WifiUartHelper::WifiUartHelper () {
}
WifiUartHelper::~WifiUartHelper () {
}

// Network
int WifiUartHelper::initWireless (
        WifiUart *wifi,
        int mode,
        char *ssid,
        int encryptionMode,
        char *key,
        int keyFormat,
        int wepIndex) {
    int ret = WIFIU_HELPER_RESPONSE_OK; // assume unchanged

    WifiUartNetModeResponse modeResponse = WifiUartNetwork::getWirelessMode(wifi);
    if (modeResponse.code != WIFIU_NET_RESPONSE_OK) {
        return WIFIU_HELPER_RESPONSE_ERR;
    }
    if (modeResponse.mode != mode) {
        // The mode requires changing
        if (!WifiUartNetwork::setWirelessMode(wifi, mode)) {
            return WIFIU_HELPER_RESPONSE_ERR;
        }
        ret = WIFIU_HELPER_RESPONSE_MODIFIED;
    }

    WifiUartNetSSIDResponse ssidResponse = WifiUartNetwork::getSSID(wifi);
    if (ssidResponse.code != WIFIU_NET_RESPONSE_OK) {
        return WIFIU_HELPER_RESPONSE_ERR;
    }
    if (strncmp(ssidResponse.ssid, ssid, WIFIU_NET_SSIDRESPONSE_SSIDLENGTH) != 0) {
        // The ssid requires changing
        if (!WifiUartNetwork::setSSID(wifi, ssid)) {
            return WIFIU_HELPER_RESPONSE_ERR;
        }
        ret = WIFIU_HELPER_RESPONSE_MODIFIED;
    }

    WifiUartNetSecurityResponse securityResponse = WifiUartNetwork::getSecurity(wifi, true);
    if (securityResponse.code != WIFIU_NET_RESPONSE_OK) {
        return WIFIU_HELPER_RESPONSE_ERR;
    }
    if (securityResponse.encryptionMode != encryptionMode
            || strncmp(securityResponse.key, key, WIFIU_NET_SECURITYRESPONSE_KEYLENGTH) != 0
            || securityResponse.keyFormat != keyFormat
            || securityResponse.wepIndex != wepIndex
            ) {
        if (!WifiUartNetwork::setSecurity(wifi, encryptionMode, key, keyFormat, wepIndex)) {
            return WIFIU_HELPER_RESPONSE_ERR;
        }
        ret = WIFIU_HELPER_RESPONSE_MODIFIED;
    }

    return ret;
}

int WifiUartHelper::initNetworkDHCP (WifiUart *wifi) {
    int ret = WIFIU_HELPER_RESPONSE_OK; // assume unchanged

    WifiUartNetSettingsResponse settingsResponse = WifiUartNetwork::getNetworkSettings(wifi);
    if (settingsResponse.code != WIFIU_NET_RESPONSE_OK) {
        return WIFIU_HELPER_RESPONSE_ERR;
    }
    if (settingsResponse.staticIp) {
        // Currently static ip
        if (!WifiUartNetwork::setNetworkSettingsToDHCP(wifi)) {
            return WIFIU_HELPER_RESPONSE_ERR;
        }
        ret = WIFIU_HELPER_RESPONSE_MODIFIED;
    }
    return ret;
}

int WifiUartHelper::initNetworkStaticIP (
        WifiUart *wifi,
        char *ip,
        char *netmask,
        char *gateway,
        char *dns) {
    int ret = WIFIU_HELPER_RESPONSE_OK; // assume unchanged

    WifiUartNetSettingsResponse settingsResponse = WifiUartNetwork::getNetworkSettings(wifi);
    if (settingsResponse.code == WIFIU_NET_RESPONSE_ERR) {
        return WIFIU_HELPER_RESPONSE_ERR;
    }
    if (!settingsResponse.staticIp
            || strncmp(settingsResponse.ip, ip, WIFIU_NET_IPLENGTH) != 0
            || strncmp(settingsResponse.netmask, netmask, WIFIU_NET_IPLENGTH) != 0
            || strncmp(settingsResponse.gateway, gateway, WIFIU_NET_IPLENGTH) != 0
            || strncmp(settingsResponse.dns, dns, WIFIU_NET_IPLENGTH) != 0) {
        // Settings need to be changed
        if (!WifiUartNetwork::setNetworkSettingsToStaticIP(wifi, ip, netmask, gateway, dns)) {
            return WIFIU_HELPER_RESPONSE_ERR;
        }
        ret = WIFIU_HELPER_RESPONSE_MODIFIED;
    }
    return ret;
}

// Transparent
int WifiUartHelper::initClient(WifiUart *wifi, char *serverName, int port, int protocol) {
    int ret = WIFIU_HELPER_RESPONSE_OK; // assume unchanged

    WifiUartTransDetailsResponse transResponse = WifiUartTransparent::getConnectionDetails(wifi);
    if (transResponse.code != WIFIU_TRANS_RESPONSE_OK) {
        return WIFIU_HELPER_RESPONSE_ERR;
    }
    if (!transResponse.isClient
            || strncmp(transResponse.serverName, serverName, WIFIU_TRANS_SERVERLENGTH) != 0
            || transResponse.port != port
            || transResponse.protocol != protocol) {
        // Settings need to be changed
        if (!WifiUartTransparent::setClient(wifi, serverName, port, protocol)) {
            return WIFIU_HELPER_RESPONSE_ERR;
        }
        ret = WIFIU_HELPER_RESPONSE_MODIFIED;
    }
    return ret;
}

int WifiUartHelper::initServer(WifiUart *wifi, int port, int protocol, int timeout) {
    int ret = WIFIU_HELPER_RESPONSE_OK; // assume unchanged

    WifiUartTransDetailsResponse transResponse = WifiUartTransparent::getConnectionDetails(wifi);
    if (transResponse.code != WIFIU_TRANS_RESPONSE_OK) {
        return WIFIU_HELPER_RESPONSE_ERR;
    }
    if (transResponse.isClient
            || transResponse.port != port
            || transResponse.protocol != protocol
            || transResponse.connectionTimeout != timeout) {
        // Settings need to be changed
        if (!WifiUartTransparent::setServer(wifi, port, protocol, timeout)) {
            return WIFIU_HELPER_RESPONSE_ERR;
        }
        ret = WIFIU_HELPER_RESPONSE_MODIFIED;
    }
    return ret;
}

int WifiUartHelper::initMisc(WifiUart *wifi, bool enableWebService, int port, char *password) {
    int ret = WIFIU_HELPER_RESPONSE_OK; // assume unchanged

    WifiUartMiscWebServiceResponse webResponse = WifiUartMisc::getWebServiceStatus(wifi);
    if (webResponse.code != WIFIU_MISC_RESPONSE_OK) {
        return WIFIU_HELPER_RESPONSE_ERR;
    }
    if (webResponse.isEnabled != enableWebService
            || webResponse.port != port) {
        // Settings need to be changed
        if (!WifiUartMisc::setWebServiceStatus(wifi, enableWebService, port)) {
            return WIFIU_HELPER_RESPONSE_ERR;
        }
        ret = WIFIU_HELPER_RESPONSE_MODIFIED;
    }

    WifiUartMiscPasswordResponse passResponse = WifiUartMisc::getSystemPassword(wifi);
    if (passResponse.code != WIFIU_MISC_RESPONSE_OK) {
        return WIFIU_HELPER_RESPONSE_ERR;
    }
    if (strncmp(passResponse.password, password, WIFIU_MISC_PASSLENGTH) != 0) {
        // Settings need to be changed
        if (!WifiUartMisc::setSystemPassword(wifi, password)) {
            return WIFIU_HELPER_RESPONSE_ERR;
        }
        ret = WIFIU_HELPER_RESPONSE_MODIFIED;
    }

    return ret;
}

// Debug
void WifiUartHelper::printWirelessSettings (WifiUart *wifi, Stream *outSerial) {
    // TODO:
    outSerial->write("Wireless Settings\n");
}
void WifiUartHelper::printNetworkSettings (WifiUart *wifi, Stream *outSerial) {
    // TODO:
    outSerial->write("Network Settings\n");
}
void WifiUartHelper::printTransparentSettings (WifiUart *wifi, Stream *outSerial) {
    // TODO:
    outSerial->write("Transparent Settings\n");
}
void WifiUartHelper::printWebServiveSettings (WifiUart *wifi, Stream *outSerial) {
    // TODO:
    outSerial->write("Web Service Settings\n");
}

