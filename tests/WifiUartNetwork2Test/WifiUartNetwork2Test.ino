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
#include <WifiUartNetwork.h>
#include <ArduinoUnit.h>

// test (getConnectionStateWifi) {}

test (getConnectionStateWifiBool) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  stream.nextBytes("+OK=1,\"192.168.1.1\",\"255.255.0.0\",\"192.168.1.2\",\"192.168.0.3\"\r\n\r\n");
  stream.nextBytes("+OK=1234567890ab\r\n\r\n");
  WifiUart wifi = WifiUart(&stream, true);
  WifiUartNetStateResponse response = WifiUartNetwork::getConnectionState(&wifi);
  assertTrue(response.isConnected);
  assertEqual("192.168.1.1", (String)response.ip);
  assertEqual("255.255.0.0", (String)response.netmask);
  assertEqual("192.168.1.2", (String)response.gateway);
  assertEqual("192.168.0.3", (String)response.dns);
  assertEqual("1234567890ab", (String)response.physical);
  assertEqual("AT+LKSTT\rAT+QMAC\r", stream.bytesWritten());
}

test (getNetworkSettings) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  // NIP, static ip, ip, mask, gateway, dns
  stream.nextBytes("+OK=0,\"192.168.1.1\",\"255.255.0.0\",\"192.168.1.2\",\"192.168.0.3\"\r\n\r\n");
  WifiUart wifi = WifiUart(&stream, true);
  WifiUartNetSettingsResponse response = WifiUartNetwork::getNetworkSettings(&wifi);
  assertFalse(response.staticIp);
  assertEqual("192.168.1.1", (String)response.ip);
  assertEqual("255.255.0.0", (String)response.netmask);
  assertEqual("192.168.1.2", (String)response.gateway);
  assertEqual("192.168.0.3", (String)response.dns);
  assertEqual("AT+NIP\r", stream.bytesWritten());
}

test (setNetworkSettingsToDHCP) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream, true);
  stream.nextBytes("+OK\r\n\r\n");
  assertTrue(WifiUartNetwork::setNetworkSettingsToDHCP(&wifi));
  assertEqual("AT+NIP=!0\r", stream.bytesWritten());
}

test (setNetworkSettingsToStaticIP) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream, true);
  stream.nextBytes("+OK\r\n\r\n");
  assertTrue(WifiUartNetwork::setNetworkSettingsToStaticIP(&wifi, "192.168.0.1", "255.255.255.0", "192.168.0.2", "192.168.0.3"));
  assertEqual("AT+NIP=!1,192.168.0.1,255.255.255.0,192.168.0.2,192.168.0.3\r", stream.bytesWritten());
}

void setup() {
  Serial.begin(115200);
  Serial.println("");
}

void loop() {
  Test::run();
}
