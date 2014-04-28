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

#include <WifiUartResponse.h>
#include <ArduinoUnit.h>

test (constructorWifi) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream);
  WifiUartResponse response = WifiUartResponse(&wifi);
  assertEqual(WIFIU_ERR_INCOMPLETE, response.getCode());
  assertEqual(0, response.getResponseLength());
  assertEqual(0, strcmp("", response.getResponse()));
}

//test (deconstructor) {}

test (receiveOk) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream);
  WifiUartResponse response = WifiUartResponse(&wifi);
  stream.nextBytes("+OK\r\n\r\n");
  response.receive();
  assertEqual(WIFIU_OK, response.getCode());
  assertEqual(0, response.getResponseLength());
  assertEqual((String)"", (String)response.getResponse());
}
test (receiveErr) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  stream.nextBytes("+ERR=-10\r\n\r\n");
  WifiUart wifi = WifiUart(&stream);
  WifiUartResponse response = WifiUartResponse(&wifi);
  response.receive();
  assertEqual(WIFIU_ERR_JOIN, response.getCode());
  assertEqual(3, response.getResponseLength());
  assertEqual((String)"-10", (String)response.getResponse());
}
test (receiveWithExtra) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  stream.nextBytes("+OK=one,two\r\n\r\n");
  WifiUart wifi = WifiUart(&stream);
  WifiUartResponse response = WifiUartResponse(&wifi);
  response.receive();
  assertEqual(WIFIU_OK, response.getCode());
  assertEqual(7, response.getResponseLength());
  assertEqual((String)"one,two", (String)response.getResponse());
}

//test (getCode) {}
//test (getResponse) {}
//test (getResponseLength) {}

test (getResponseField) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  stream.nextBytes("+OK=one,two,three\r\n\r\n");
  WifiUart wifi = WifiUart(&stream);
  WifiUartResponse response = WifiUartResponse(&wifi);
  response.receive();
  assertEqual(WIFIU_OK, response.getCode());
  char tempString[response.getResponseLength()+1];
  response.getResponseField(0, tempString, response.getResponseLength());
  assertEqual((String)"one", (String)tempString);
  response.getResponseField(1, tempString, response.getResponseLength());
  assertEqual((String)"two", (String)tempString);
  response.getResponseField(2, tempString, response.getResponseLength());
  assertEqual((String)"three", (String)tempString);
  response.getResponseField(3, tempString, response.getResponseLength());
  assertEqual((String)"", (String)tempString);
  response.getResponseField(4, tempString, response.getResponseLength());
  assertEqual((String)"", (String)tempString);
}

void setup() {
  Serial.begin(115200);
  Serial.println("");
}

void loop() {
  Test::run();
}
