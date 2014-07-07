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

#include <WifiUart.h>
#include <WifiUartTransparent.h>
#include <ArduinoUnit.h>

// test (constructor) {}
// test (deconstructor) {}

test (getConnectionDetailsServer) {
    FakeStreamBuffer stream = FakeStreamBuffer();
    WifiUart wifi = WifiUart(&stream, true);
    // ATRM, tcp, server, timeout, port
    stream.nextBytes("+OK=0,1,42,66\r\n\r\n");
    WifiUartTransDetailsResponse response = WifiUartTransparent::getConnectionDetails(&wifi);
    assertEqual(WIFIU_TRANS_RESPONSE_OK, response.code);
    assertFalse(response.isClient);
    assertEqual("", (String)response.serverName); // NA for server mode
    assertEqual(66, response.port);
    assertEqual(WIFIU_TRANS_PROTO_TCP, response.protocol);
    assertEqual(42, response.connectionTimeout);
    assertEqual("AT+ATRM\r", stream.bytesWritten());
}
test (getConnectionDetailsClient) {
    FakeStreamBuffer stream = FakeStreamBuffer();
    WifiUart wifi = WifiUart(&stream, true);
    // protocol, client, server name, port
    stream.nextBytes("+OK=1,0,\"192.168.1.1\",78\r\n\r\n");
    WifiUartTransDetailsResponse response = WifiUartTransparent::getConnectionDetails(&wifi);
    assertEqual(WIFIU_TRANS_RESPONSE_OK, response.code);
    assertTrue(response.isClient);
    assertEqual("192.168.1.1", (String)response.serverName);
    assertEqual(78, response.port);
    assertEqual(WIFIU_TRANS_PROTO_UDP, response.protocol);
    assertEqual(-1, response.connectionTimeout); // NA for client mode
    assertEqual("AT+ATRM\r", stream.bytesWritten());
}

test (setClientWifiCharInt) {
    FakeStreamBuffer stream = FakeStreamBuffer();
    WifiUart wifi = WifiUart(&stream, true);
    stream.nextBytes("+OK\r\n\r\n");
    assertTrue(WifiUartTransparent::setClient(&wifi, "192.168.12.55", 22));
    assertEqual("AT+ATRM=!0,0,192.168.12.55,22\r", stream.bytesWritten());
}

test (setClientWifiCharIntInt) {
    FakeStreamBuffer stream = FakeStreamBuffer();
    WifiUart wifi = WifiUart(&stream, true);
    stream.nextBytes("+OK\r\n\r\n");
    assertTrue(WifiUartTransparent::setClient(&wifi, "192.168.0.9", 8080, WIFIU_TRANS_PROTO_UDP));
    assertEqual("AT+ATRM=!1,0,192.168.0.9,8080\r", stream.bytesWritten());
}

// test (setServerWifiInt) {}
test (setServerWifiIntIntInt) {
    FakeStreamBuffer stream = FakeStreamBuffer();
    WifiUart wifi = WifiUart(&stream, true);
    stream.nextBytes("+OK\r\n\r\n");
    assertTrue(WifiUartTransparent::setServer(&wifi, 80, WIFIU_TRANS_PROTO_TCP, 100));
    assertEqual("AT+ATRM=!0,1,100,80\r", stream.bytesWritten());
}


void setup() {
  Serial.begin(115200);
  Serial.println("");
}

void loop() {
  Test::run();
}

