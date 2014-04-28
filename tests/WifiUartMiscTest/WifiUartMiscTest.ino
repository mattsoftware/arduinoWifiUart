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
#include <WifiUartMisc.h>
#include <ArduinoUnit.h>

test (getSystemPasswordOk) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream, true);
  stream.nextBytes("+OK=\"123456\"\r\n\r\n");
  WifiUartMiscPasswordResponse response = WifiUartMisc::getSystemPassword(&wifi);
  assertEqual(WIFIU_MISC_RESPONSE_OK, response.code);
  assertEqual((String)"123456", (String)response.password);
  assertEqual("AT+PASS\r", stream.bytesWritten());
}
test (getSystemPasswordErr) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream, true);
  stream.nextBytes("+ERR=1\r\n\r\n");
  WifiUartMiscPasswordResponse response = WifiUartMisc::getSystemPassword(&wifi);
  assertEqual(WIFIU_MISC_RESPONSE_ERR, response.code);
  assertEqual((String)"", (String)response.password);
  assertEqual("AT+PASS\r", stream.bytesWritten());
}

test (setSystemPasswordOk) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream, true);
  stream.nextBytes("+OK\r\n\r\n");
  assertTrue(WifiUartMisc::setSystemPassword(&wifi, "mattsw"));
  assertEqual("AT+PASS=!mattsw\r", stream.bytesWritten());
}

test (setSystemPasswordErr) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream, true);
  assertFalse(WifiUartMisc::setSystemPassword(&wifi, "toolong"));
  assertFalse(WifiUartMisc::setSystemPassword(&wifi, "short"));
}
test (getWebServiceStatusOn) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream, true);
  stream.nextBytes("+OK=1,80\r\n\r\n");
  WifiUartMiscWebServiceResponse response = WifiUartMisc::getWebServiceStatus(&wifi);
  assertEqual(WIFIU_MISC_RESPONSE_OK, response.code);
  assertTrue(response.isEnabled);
  assertEqual(80, response.port);
  assertEqual("AT+WEBS\r", stream.bytesWritten());
}

test (getWebServiceStatusOff) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream, true);
  stream.nextBytes("+OK=0,81\r\n\r\n");
  WifiUartMiscWebServiceResponse response = WifiUartMisc::getWebServiceStatus(&wifi);
  assertEqual(WIFIU_MISC_RESPONSE_OK, response.code);
  assertFalse(response.isEnabled);
  assertEqual(81, response.port);
  assertEqual("AT+WEBS\r", stream.bytesWritten());
}

test (setWebServiceStatus) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream, true);
  stream.nextBytes("+OK\r\n\r\n");
  assertTrue(WifiUartMisc::setWebServiceStatus(&wifi, true, 82));
  assertEqual("AT+WEBS=!1,82\r", stream.bytesWritten());
}

void setup() {
  Serial.begin(115200);
  Serial.println("");
}

void loop() {
  Test::run();
}
