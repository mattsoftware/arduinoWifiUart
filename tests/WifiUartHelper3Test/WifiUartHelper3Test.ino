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

test (initClient) {
    FakeStreamBuffer stream = FakeStreamBuffer();
    WifiUart wifi = WifiUartMock(&stream);
    // ATRM, tcp, client, server name, port
    stream.nextBytes("+OK=0,0,\"192.168.2.3\",80\r\n\r\n");
    assertEqual(WIFIU_HELPER_RESPONSE_OK, WifiUartHelper::initClient(&wifi, "192.168.2.3", 80, WIFIU_NET_ATRM_TCP));
    assertEqual("AT+ATRM\r", stream.bytesWritten());
}
test (initClientModified) {
    FakeStreamBuffer stream = FakeStreamBuffer();
    WifiUart wifi = WifiUartMock(&stream);
    // ATRM, udp (expecting tcp), client, server name, port
    stream.nextBytes("+OK=1,0,\"192.168.2.3\",80\r\n\r\n");
    stream.nextBytes("+OK\r\n\r\n");
    assertEqual(WIFIU_HELPER_RESPONSE_MODIFIED, WifiUartHelper::initClient(&wifi, "192.168.2.3", 80, WIFIU_NET_ATRM_TCP));
    assertEqual("AT+ATRM\rAT+ATRM=!0,0,192.168.2.3,80\r", stream.bytesWritten());
}

test (initServer) {
    FakeStreamBuffer stream = FakeStreamBuffer();
    WifiUart wifi = WifiUartMock(&stream);
    // ATRM, udp, server, timeout, port
    stream.nextBytes("+OK=1,1,120,8080\r\n\r\n");
    assertEqual(WIFIU_HELPER_RESPONSE_OK, WifiUartHelper::initServer(&wifi, 8080, WIFIU_NET_ATRM_UDP, WIFIU_TRANS_DEFAULT_TIMEOUT));
    assertEqual("AT+ATRM\r", stream.bytesWritten());
    stream.reset();
}
test (initServerModified) {
    FakeStreamBuffer stream = FakeStreamBuffer();
    WifiUart wifi = WifiUartMock(&stream);
    // ATRM, tcp (expecting udp), server, timeout, port
    stream.nextBytes("+OK=0,1,120,8080\r\n\r\n");
    stream.nextBytes("+OK\r\n\r\n");
    assertEqual(WIFIU_HELPER_RESPONSE_MODIFIED, WifiUartHelper::initServer(&wifi, 8080, WIFIU_NET_ATRM_UDP, WIFIU_TRANS_DEFAULT_TIMEOUT));
    assertEqual("AT+ATRM\rAT+ATRM=!1,1,120,8080\r", stream.bytesWritten());
}

test (initMisc) {
    FakeStreamBuffer stream = FakeStreamBuffer();
    WifiUart wifi = WifiUartMock(&stream);
    // WEBS, disabled, port
    stream.nextBytes("+OK=0,80\r\n\r\n");
    // PASS, passcode
    stream.nextBytes("+OK=\"123456\"\r\n\r\n");
    assertEqual(WIFIU_HELPER_RESPONSE_OK, WifiUartHelper::initMisc(&wifi, false, 80, "123456"));
    assertEqual("AT+WEBS\rAT+PASS\r", stream.bytesWritten());
}
test (initMiscModifiedWebService) {
    FakeStreamBuffer stream = FakeStreamBuffer();
    WifiUart wifi = WifiUartMock(&stream);
    // WEBS, disabled, port (expect 80)
    stream.nextBytes("+OK=0,81\r\n\r\n");
    // PASS, passcode
    stream.nextBytes("+OK=\"123456\"\r\n\r\n");
    stream.nextBytes("+OK\r\n\r\n");
    stream.nextBytes("+OK\r\n\r\n");
    assertEqual(WIFIU_HELPER_RESPONSE_MODIFIED, WifiUartHelper::initMisc(&wifi, false, 80, "123456"));
    assertEqual("AT+WEBS\rAT+WEBS=!0,80\rAT+PASS\rAT+PASS=!123456\r", stream.bytesWritten());
}
test (initMiscModifiedPasscode) {
    FakeStreamBuffer stream = FakeStreamBuffer();
    WifiUart wifi = WifiUartMock(&stream);
    // WEBS, disabled, port
    stream.nextBytes("+OK=0,81\r\n\r\n");
    // PASS, passcode (expect 123456)
    stream.nextBytes("+OK=\"123457\"\r\n\r\n");
    stream.nextBytes("+OK\r\n\r\n");
    stream.nextBytes("+OK\r\n\r\n");
    assertEqual(WIFIU_HELPER_RESPONSE_MODIFIED, WifiUartHelper::initMisc(&wifi, false, 80, "123456"));
    assertEqual("AT+WEBS\rAT+WEBS=!0,80\rAT+PASS\rAT+PASS=!123456\r", stream.bytesWritten());
}

void setup() {
    Serial.begin(115200);
    Serial.println("");
}

void loop() {
    Test::run();
}

