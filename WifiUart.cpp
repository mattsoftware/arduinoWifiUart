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

#include "WifiUart.h"

WifiUart::WifiUart (Stream *serial) {
    _init(serial);
}
WifiUart::WifiUart (Stream *serial, bool isAlreadyInCommandMode) {
    _init(serial, isAlreadyInCommandMode);
}

WifiUart::~WifiUart () {
}

void WifiUart::setEscapeCharacter (char character) {
    _escapeCharacter = character;
}

void WifiUart::setEscapeTimeout (uint16_t milliseconds) {
    _escapeTimeout = milliseconds;
}

void WifiUart::setEscapeTimeoutBuffer (uint16_t milliseconds) {
    _escapeTimeoutBuffer = milliseconds;
}

void WifiUart::setAutoFramingPeriod (uint16_t milliseconds) {
    _autoFramingPeriod = milliseconds;
}

uint16_t WifiUart::getCommandTimeout() {
    return _commandTimeout;
}

void WifiUart::setCommandTimeout (uint16_t milliseconds) {
    _commandTimeout = milliseconds;
}

Stream* WifiUart::getSerial() {
    return _serial;
}

bool WifiUart::isInCommandMode() {
    return _isInCommandMode;
}

bool WifiUart::getIsSending() {
    return _isSending;
}

void WifiUart::setIsSending() {
    _isSending = true;
}

void WifiUart::clearIsSending() {
    _isSending = false;
}

bool WifiUart::setHardwareResetPin(short pin, bool init) {
	if (_hardwareResetPin > -1) {
		return false;
	}
	_hardwareResetPin = pin;
	if (init) {
		pinMode(_hardwareResetPin, OUTPUT);
		digitalWrite(_hardwareResetPin, HIGH);
	}
	return true;
}

bool WifiUart::enterCommandMode() {
    if (_isInCommandMode) {
        return false;
    }
    delay (_escapeTimeout + _escapeTimeoutBuffer);
    for (int x = 0; x < 3; x++) {
        _serial->write(_escapeCharacter);
    }
    WifiUartResponse response = WifiUartResponse(this);
    // Experience shows waiting a further 2 seconds really helps!
    delay(_autoFramingPeriod + 2000);
    // If we are already in command mode, this should fix the module getting
    // into a state of limbo
    _serial->write("\r");
    response.receive();
    if (response.getCode() == WIFIU_OK
            || response.getCode() == WIFIU_ERR_FORMAT) {
        _isInCommandMode = true;
        return true;
    }
    return false;
}

bool WifiUart::exitCommandMode() {
    if (!_isInCommandMode) {
        return false;
    }
    return enterManualMode();
}

bool WifiUart::enterManualMode() {
    WifiUartCommand cmd = WifiUartCommand(this, WIFIU_ATP_ENTM);
    WifiUartResponse res = cmd.send();
    if (res.getCode() == WIFIU_OK) {
        _isInCommandMode = false;
        return true;
    }
    return false;
}

bool WifiUart::factoryReset() {
    WifiUartCommand cmd = WifiUartCommand(this, WIFIU_ATP_RSTF);
    WifiUartResponse res = cmd.send();
    if (res.getCode() == WIFIU_OK) {
        return true;
    }
    return false;
}

bool WifiUart::reset() {
    WifiUartCommand cmd = WifiUartCommand(this, WIFIU_ATP_Z);
    WifiUartResponse res = cmd.send();
    if (res.getCode() == WIFIU_OK) {
        // Doing a reset will put it out of command mode as well
        _isInCommandMode = false;
        return true;
    }
    return false;
}

bool WifiUart::hardwareReset() {
	if (_hardwareResetPin < 0) {
		return false;
	}
	digitalWrite(_hardwareResetPin, LOW);
	delay(_hardwareResetDelay);
	digitalWrite(_hardwareResetPin, HIGH);
	delay(_hardwareResetDelay);
	_isInCommandMode = false;
	return true;
}

void WifiUart::_init (Stream *serial) {
    _init(serial, false);
}

void WifiUart::_init (Stream *serial, bool isAlreadyInCommandMode) {
    _escapeCharacter = '+';
    _escapeTimeout = 2000;
    _escapeTimeoutBuffer = 200;
    _autoFramingPeriod = 500;
    _commandTimeout = WIFIU_RESPONSE_DEFAULT_TIMEOUT;
    _serial = serial;
    _isInCommandMode = isAlreadyInCommandMode;
    _isSending = false;
	_hardwareResetPin = -1;
	_hardwareResetDelay = 500;
}
