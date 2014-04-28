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

#include "WifiUartResponse.h"

#define WIFIU_RESPONSE_TERMINATOR "\r\n\r\n"

#define WIFIU_RESPONSE_STATE_INIT 1 // serial initialisation
#define WIFIU_RESPONSE_STATE_WAIT 2 // waiting for response
#define WIFIU_RESPONSE_STATE_DATA 3 // waiting for data
#define WIFIU_RESPONSE_STATE_END 4 // waiting for end
#define WIFIU_RESPONSE_STATE_ERRCODE 5 // waiting for error code
#define WIFIU_RESPONSE_STATE_COMPLETE 6 // completed

WifiUartResponse::WifiUartResponse (WifiUart *wifi) {
    _wifi = wifi;
    _code = WIFIU_ERR_INCOMPLETE;
    _response = "";
    _length = 0;
    _cleanReceiveBuffer();
    _receiveState = WIFIU_RESPONSE_STATE_INIT;
    return;
}

WifiUartResponse::~WifiUartResponse () {
}

bool WifiUartResponse::receive() {
    unsigned long expire = millis() + _wifi->getCommandTimeout();
    Stream *serial = _wifi->getSerial();
    while (millis() < expire) {
        while (serial->available() && _receiveState != WIFIU_RESPONSE_STATE_COMPLETE) {
            _receiveChar(serial->read());
        }
        if (_receiveState == WIFIU_RESPONSE_STATE_COMPLETE) {
            break;
        }
    }
    _wifi->clearIsSending();
    if (_receiveState != WIFIU_RESPONSE_STATE_COMPLETE) {
        _code = WIFIU_ERR_TIMEDOUT;
        return false;
    }
    return _code == WIFIU_OK;
}

int WifiUartResponse::getCode() {
    return _code;
}
char* WifiUartResponse::getResponse() {
    return _response;
}
uint16_t WifiUartResponse::getResponseLength() {
    return _length;
}

void WifiUartResponse::_receiveChar(char c) {
    switch (_receiveState) {
        case WIFIU_RESPONSE_STATE_INIT:
            if (c == '+') {
                _receiveState = WIFIU_RESPONSE_STATE_WAIT;
            }
            break;
        case WIFIU_RESPONSE_STATE_WAIT:
            _receiveBuffer[_receiveBufferLength++] = c;
            if (_receiveBufferLength == 2
                    && strcmp(_receiveBuffer, "OK") == 0) {
                _receiveState = WIFIU_RESPONSE_STATE_DATA;
                _code = WIFIU_OK;
                _cleanReceiveBuffer();
            }
            else if (_receiveBufferLength == 3
                    && strcmp(_receiveBuffer, "ERR") == 0) {
                _receiveState = WIFIU_RESPONSE_STATE_ERRCODE;
                _code = WIFIU_ERR_UNKNOWN;
                _cleanReceiveBuffer();
            }
            else if (_receiveBufferLength >= 3) {
                // We didn't get an OK or an ERR
                _receiveState = WIFIU_RESPONSE_STATE_COMPLETE;
                _code = WIFIU_ERR_UNKNOWN;
                _cleanReceiveBuffer();
            }
            break;
        case WIFIU_RESPONSE_STATE_DATA:
        case WIFIU_RESPONSE_STATE_END:
        case WIFIU_RESPONSE_STATE_ERRCODE:
            if (c == '=' && _receiveBufferLength == 0) {
            }
			else if (c == '"') {
			}
            else {
                _receiveBuffer[_receiveBufferLength++] = c;
                if (strstr(_receiveBuffer, WIFIU_RESPONSE_TERMINATOR) != NULL) {
                    _receiveState = WIFIU_RESPONSE_STATE_COMPLETE;
                    if (_code == WIFIU_ERR_UNKNOWN) {
                        _code = atoi(_receiveBuffer);
                    }
                    // zero out the trailing cr/lf/cr/lf
                    int x = _receiveBufferLength - 4;
                    while (_receiveBufferLength > x) {
                        _receiveBuffer[--_receiveBufferLength] = 0;
                    }
                    _response = _receiveBuffer;
                    _length = _receiveBufferLength;
                }
                else if (_receiveBufferLength >= WIFIU_RESPONSE_BUFFER_LENGTH) {
                    _receiveState = WIFIU_RESPONSE_STATE_COMPLETE;
                }
            }
            break;
        case WIFIU_RESPONSE_STATE_COMPLETE:
            break;
        default:
            _receiveState = WIFIU_RESPONSE_STATE_COMPLETE;
    }
}

void WifiUartResponse::_cleanReceiveBuffer() {
    for (int x = 0; x < WIFIU_RESPONSE_BUFFER_LENGTH; x++) {
        _receiveBuffer[x] = '\0';
    }
    _receiveBufferLength = 0;
}

void WifiUartResponse::getResponseField(int index, char *buffer, int length) {
	return getResponseField(index, buffer, length, ',');
}
void WifiUartResponse::getResponseField(int index, char *buffer, int length, char separator) {
	char currentLength = 0;
	int currentIndex = 0;
	for (int x = 0; x < length; x++) {
		if (_response[x] == separator) {
			currentIndex++;
			if (currentIndex > index) {
				break;
			}
		}
		else if (_response[x] == '\0') {
			break;
		}
		else if (index == currentIndex) {
			buffer[currentLength++] = _response[x];
		}
	}
	buffer[currentLength] = '\0';
	return;
}
