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

#include "WifiUartCommand.h"

WifiUartCommand::WifiUartCommand (WifiUart *wifi, char *cmd) {
    _init(wifi, cmd);
}
WifiUartCommand::WifiUartCommand (WifiUart *wifi, char *cmd, char *op, char *extra) {
    _init(wifi, cmd, op, extra);
}
WifiUartCommand::~WifiUartCommand () {
}

WifiUartResponse WifiUartCommand::send () {
    WifiUartResponse response = WifiUartResponse(_wifi);
    if (_wifi->getIsSending()) {
        // TODO: Set code to is sending or something more appropriate
        return response;
    }
    else if (!_wifi->isInCommandMode()) {
        // TODO: Set code to not in command mode or something more appropriate
        return response;
    }
    _wifi->setIsSending();
    Stream *serial = _wifi->getSerial();
    serial->write("AT+");
    serial->write(_command);
    serial->write(_operation);
    serial->write(_extra);
    serial->write("\r");
    response.receive();
    return response;
}

void WifiUartCommand::_init(WifiUart *wifi, char *cmd) {
    _init(wifi, cmd, WIFIU_OP_NONE, "");
}

void WifiUartCommand::_init(WifiUart *wifi, char *cmd, char *op, char *extra) {
    _wifi = wifi;
    _command = cmd;
    _operation = op;
    _extra = extra;
}
