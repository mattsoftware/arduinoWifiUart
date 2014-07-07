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

#include <ArduinoUnit.h>
#include <WifiUart.h>
#include <WifiUartHelper.h>
#include <WifiUartMock.h>

test (initNetworkStaticIP) {
    FakeStreamBuffer stream = FakeStreamBuffer();
    WifiUart wifi = WifiUartMock(&stream);
    // NIP, static ip, ip, mask, gateway, dns
    stream.nextBytes("+OK=1,\"10.10.10.1\",\"255.255.0.0\",\"10.10.10.254\",\"10.10.10.250\"\r\n\r\n");
    assertEqual(WIFIU_HELPER_RESPONSE_OK, WifiUartHelper::initNetworkStaticIP(&wifi, "10.10.10.1", "255.255.0.0", "10.10.10.254", "10.10.10.250"));
    assertEqual("AT+NIP\r", stream.bytesWritten());
}
test (initNetworkStaticIPModified) {
    FakeStreamBuffer stream = FakeStreamBuffer();
    WifiUart wifi = WifiUartMock(&stream);
    // NIP, dhcp (expect static ip), ip, mask, gateway, dns
    stream.nextBytes("+OK=0,\"10.10.10.1\",\"255.255.0.0\",\"10.10.10.254\",\"10.10.10.250\"\r\n\r\n");
    stream.nextBytes("+OK\r\n\r\n");
    assertEqual(WIFIU_HELPER_RESPONSE_MODIFIED, WifiUartHelper::initNetworkStaticIP(&wifi, "10.10.10.1", "255.255.0.0", "10.10.10.254", "10.10.10.250"));
    assertEqual("AT+NIP\rAT+NIP=!1,10.10.10.1,255.255.0.0,10.10.10.254,10.10.10.250\r", stream.bytesWritten());
}

void setup() {
    Serial.begin(115200);
    Serial.println("");
}

void loop() {
    Test::run();
}

