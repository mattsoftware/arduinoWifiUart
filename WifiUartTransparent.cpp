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

#include "WifiUartTransparent.h"

WifiUartTransparent::WifiUartTransparent () {
}

WifiUartTransparent::~WifiUartTransparent () {
}

struct WifiUartTransDetailsResponse WifiUartTransparent::getConnectionDetails (WifiUart *wifi) {
    WifiUartCommand cmd = WifiUartCommand(wifi, WIFIU_ATP_ATRM);
    WifiUartResponse res = cmd.send();
	WifiUartTransDetailsResponse ret = { WIFIU_TRANS_RESPONSE_ERR, false, "", -1, -1, -1 };
    if (res.getCode() == WIFIU_OK) {
		ret.code = WIFIU_TRANS_RESPONSE_OK;

		char tmpString[res.getResponseLength()];
		res.getResponseField(0, tmpString, res.getResponseLength());
        ret.protocol = atoi(tmpString);
		res.getResponseField(1, tmpString, res.getResponseLength());
        ret.isClient = (atoi(tmpString) == WIFIU_TRANS_CS_CLIENT);
		res.getResponseField(2, tmpString, res.getResponseLength());
        if (ret.isClient) {
    		strncpy(ret.serverName, tmpString, WIFIU_TRANS_SERVERLENGTH);
        }
        else {
            ret.connectionTimeout = atoi(tmpString);
        }
		res.getResponseField(3, tmpString, res.getResponseLength());
        ret.port = atoi(tmpString);
    }
    return ret;
}

bool WifiUartTransparent::setClient(WifiUart *wifi, char *serverName, int port) {
    return setClient(wifi, serverName, port, WIFIU_TRANS_PROTO_TCP);
}

bool WifiUartTransparent::setClient(WifiUart *wifi, char *serverName, int port, int protocol) {
	bool ret = false;
	char extra[1 + 1 + 1 + 1 + strlen(serverName) + 1 + 6 + 1];
    char tmpInt[13];
	extra[0] = '\0';
    // protocol
	itoa(protocol, tmpInt, 10);
	strncat(extra, tmpInt, 1);
	strncat(extra, ",", 1);
    // client/server
    itoa(WIFIU_TRANS_CS_CLIENT, tmpInt, 10);
	strncat(extra, tmpInt, 1);
	strncat(extra, ",", 1);
    // host
	strncat(extra, serverName, strlen(serverName));
	strncat(extra, ",", 1);
    // port
    itoa(port, tmpInt, 10);
	strncat(extra, tmpInt, 6);

	WifiUartCommand cmd = WifiUartCommand(wifi, WIFIU_ATP_ATRM, WIFIU_OP_SYNC, extra);
	WifiUartResponse res = cmd.send();
	return (res.getCode() == WIFIU_OK);
}

bool WifiUartTransparent::setServer(WifiUart *wifi, int port) {
    return setServer(wifi, port, WIFIU_TRANS_PROTO_TCP, WIFIU_TRANS_DEFAULT_TIMEOUT);
}

bool WifiUartTransparent::setServer(WifiUart *wifi, int port, int protocol, int timeout) {
	bool ret = false;
	char extra[1 + 1 + 1 + 1 + 6 + 1 + 6 + 1];
    char tmpInt[13];
	extra[0] = '\0';
    // protocol
	itoa(protocol, tmpInt, 10);
	strncat(extra, tmpInt, 1);
	strncat(extra, ",", 1);
    // client/server
    itoa(WIFIU_TRANS_CS_SERVER, tmpInt, 10);
	strncat(extra, tmpInt, 1);
	strncat(extra, ",", 1);
    // timeout
    itoa(timeout, tmpInt, 10);
	strncat(extra, tmpInt, 6);
	strncat(extra, ",", 1);
    // port
    itoa(port, tmpInt, 10);
	strncat(extra, tmpInt, 6);

	WifiUartCommand cmd = WifiUartCommand(wifi, WIFIU_ATP_ATRM, WIFIU_OP_SYNC, extra);
	WifiUartResponse res = cmd.send();
	return (res.getCode() == WIFIU_OK);
}
