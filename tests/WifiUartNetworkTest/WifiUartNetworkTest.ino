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

test (getWirelessModeInfrastructure) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream, true);
  // WPRT, infrastructure mode
  stream.nextBytes("+OK=0\r\n\r\n");
  WifiUartNetModeResponse response = WifiUartNetwork::getWirelessMode(&wifi);
  assertEqual(WIFIU_NET_RESPONSE_OK, response.code);
  assertEqual(WIFIU_NET_MODE_INFRASTRUCTURE, response.mode);
  assertEqual("AT+WPRT\r", stream.bytesWritten());
  stream.reset();
}
test (getWirelessModeAdHoc) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream, true);
  // WPRT, adhoc mode
  stream.nextBytes("+OK=1\r\n\r\n");
  WifiUartNetModeResponse response = WifiUartNetwork::getWirelessMode(&wifi);
  assertEqual(WIFIU_NET_RESPONSE_OK, response.code);
  assertEqual(WIFIU_NET_MODE_ADHOC, response.mode);
  assertEqual("AT+WPRT\r", stream.bytesWritten());
  stream.reset();
}
test (setWirelessModeInfrastructure) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream, true);
  stream.nextBytes("+OK\r\n\r\n");
  assertTrue(WifiUartNetwork::setWirelessMode(&wifi, WIFIU_NET_MODE_INFRASTRUCTURE));
  assertEqual("AT+WPRT=!0\r", stream.bytesWritten());
  stream.reset();
}
test (setWirelessModeAdHoc) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream, true);
  stream.nextBytes("+OK\r\n\r\n");
  assertTrue(WifiUartNetwork::setWirelessMode(&wifi, WIFIU_NET_MODE_ADHOC));
  assertEqual("AT+WPRT=!1\r", stream.bytesWritten());
  stream.reset();
}

test (getSSID) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream, true);
  // SSID, ssid
  stream.nextBytes("+OK=\"thirtytwocharacterspossible\"\r\n\r\n");
  WifiUartNetSSIDResponse response = WifiUartNetwork::getSSID(&wifi);
  assertEqual(WIFIU_NET_RESPONSE_OK, response.code);
  assertEqual((String)"thirtytwocharacterspossible", (String)response.ssid);
  assertEqual("AT+SSID\r", stream.bytesWritten());
  stream.reset();
}
test (setSSID) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream, true);
  stream.nextBytes("+OK\r\n\r\n");
  assertTrue(WifiUartNetwork::setSSID(&wifi, "somekindofssid"));
  assertEqual("AT+SSID=!somekindofssid\r", stream.bytesWritten());
  stream.reset();
}

test (getSecurityWifi) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  // ENCRY, wpa2
  stream.nextBytes("+OK=5\r\n\r\n");
  // KEY, ascii, wep index, password
  stream.nextBytes("+OK=1,0,\"password\"\r\n\r\n");
  WifiUart wifi = WifiUart(&stream, true);
  WifiUartNetSecurityResponse response = WifiUartNetwork::getSecurity(&wifi);
  assertEqual(WIFIU_NET_RESPONSE_OK, response.code);
  assertEqual(WIFIU_NET_ENC_WPA2, response.encryptionMode);
  assertEqual(WIFIU_NET_KEY_FORMAT_ASCII, response.keyFormat);
  assertEqual("password", (const char *)response.key);
  assertEqual(0, response.wepIndex);
  assertEqual("AT+ENCRY\rAT+KEY\r", stream.bytesWritten());
}

test (getSecurityWifiBool){
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream, true);
  // ENCRY, aes
  stream.nextBytes("+OK=6\r\n\r\n");
  WifiUartNetSecurityResponse response = WifiUartNetwork::getSecurity(&wifi, false);
  assertEqual(WIFIU_NET_RESPONSE_OK, response.code);
  assertEqual(WIFIU_NET_ENC_WPA2_AES, response.encryptionMode);
  assertEqual(-1, response.keyFormat);
  assertEqual("", (const char *)response.key);
  assertEqual(-1, response.wepIndex);
  assertEqual("AT+ENCRY\r", stream.bytesWritten());
}

// test (setSecurityWifiIntChar) {}
// test (setSecurityWifiIntCharInt) {}

test (setSecurityWifiIntCharIntInt) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream, true);
  stream.nextBytes("+OK\r\n\r\n");
  stream.nextBytes("+OK\r\n\r\n");
  assertTrue(WifiUartNetwork::setSecurity(&wifi, WIFIU_NET_ENC_WPA, "12345678", WIFIU_NET_KEY_FORMAT_ASCII, 0));
  assertEqual("AT+ENCRY=!3\rAT+KEY=!1,0,12345678\r", stream.bytesWritten());
}

void setup() {
  Serial.begin(115200);
  Serial.println("");
}

void loop() {
  Test::run();
}
