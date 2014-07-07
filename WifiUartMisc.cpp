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

#include "WifiUartMisc.h"

#define WIFIU_ATP_WEBS_DISABLED 0
#define WIFIU_ATP_WEBS_ENABLED 1

WifiUartMisc::WifiUartMisc () {
}
WifiUartMisc::~WifiUartMisc () {
}

struct WifiUartMiscPasswordResponse WifiUartMisc::getSystemPassword(WifiUart *wifi) {
    WifiUartCommand cmd = WifiUartCommand(wifi, WIFIU_ATP_PASS);
    WifiUartResponse res = cmd.send();
    WifiUartMiscPasswordResponse ret = { WIFIU_MISC_RESPONSE_ERR, "" };
    if (res.getCode() == WIFIU_OK) {
        ret.code = WIFIU_MISC_RESPONSE_OK;
        strncpy(ret.password, res.getResponse(), WIFIU_MISC_PASSLENGTH);
    }
    return ret;
}

bool WifiUartMisc::setSystemPassword (WifiUart *wifi, char *newPass) {
    // TODO: Make sure the password is exactly 6 characters long
    WifiUartCommand cmd = WifiUartCommand(wifi, WIFIU_ATP_PASS, WIFIU_OP_SYNC, newPass);
    WifiUartResponse res = cmd.send();
    return res.getCode() == WIFIU_OK;
}

struct WifiUartMiscWebServiceResponse WifiUartMisc::getWebServiceStatus (WifiUart *wifi) {
    WifiUartCommand cmd = WifiUartCommand(wifi, WIFIU_ATP_WEBS);
    WifiUartResponse res = cmd.send();
    WifiUartMiscWebServiceResponse ret = { WIFIU_MISC_RESPONSE_ERR, false, 0 };
    if (res.getCode() == WIFIU_OK) {
        ret.code = WIFIU_MISC_RESPONSE_OK;
        char tmpString[res.getResponseLength()];
        res.getResponseField(0, tmpString, res.getResponseLength());
        ret.isEnabled = (atoi(tmpString) == WIFIU_ATP_WEBS_ENABLED);
        res.getResponseField(1, tmpString, res.getResponseLength());
        ret.port = atoi(tmpString);
    }
    return ret;
}

bool WifiUartMisc::setWebServiceStatus(WifiUart *wifi, bool status, int port) {
    char extra[1 + 1 + 12 + 1]; // 1/0, comma, integer
    char portChar[13];
    itoa((status) ? WIFIU_ATP_WEBS_ENABLED : WIFIU_ATP_WEBS_DISABLED, extra, 10);
    itoa(port, portChar, 10);
    strncat(extra, ",", 1);
    strncat(extra, portChar, 12);
    WifiUartCommand cmd = WifiUartCommand(wifi, WIFIU_ATP_WEBS, WIFIU_OP_SYNC, extra);
    WifiUartResponse res = cmd.send();
    return res.getCode() == WIFIU_OK;
}
