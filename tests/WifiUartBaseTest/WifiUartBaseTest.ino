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
#include <ArduinoUnit.h>

test (constructorStream) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream);
  assertFalse(wifi.isInCommandMode());
}

test (constructorStreamBool) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream, true);
  assertTrue(wifi.isInCommandMode());
}

//test (deconstructor) {}
//test (setEscapeCharacter) {}
//test (setEscapeTimeout) {}
//test (setEscapeTimeoutBuffer) {}
//test (getGetCommandTimeout) {}
//test (getSerial) {}

test (isInCommandMode) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream);
  assertFalse(wifi.isInCommandMode());

  stream.nextBytes("+OK\r\n\r\n");
  assertTrue(wifi.enterCommandMode());
  assertEqual("+++\r", stream.bytesWritten());
  assertTrue(wifi.isInCommandMode());
  stream.reset();

  stream.nextBytes("+OK\r\n\r\n");
  assertTrue(wifi.exitCommandMode());
  assertEqual("AT+ENTM\r", stream.bytesWritten());
  assertFalse(wifi.isInCommandMode());
}

//test (getIsSending) {}
//test (setIsSending) {}
//test (clearIsSending) {}

test (enterCommandModeOk) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream);
  assertFalse(wifi.isInCommandMode());

  stream.nextBytes("+OK\r\n\r\n");
  assertTrue(wifi.enterCommandMode());
  assertEqual("+++\r", stream.bytesWritten());
  assertTrue(wifi.isInCommandMode());
  assertFalse(wifi.enterCommandMode());
}
test (enterCommandModeErr) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream);
  assertFalse(wifi.isInCommandMode());

  stream.nextBytes("+ERR=-1\r\n\r\n");
  assertTrue(wifi.enterCommandMode());
  assertEqual("+++\r", stream.bytesWritten());
  assertTrue(wifi.isInCommandMode());
}
test (enterCommandModeFatal) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream);
  assertFalse(wifi.isInCommandMode());

  stream.nextBytes("+ERR=-10\r\n\r\n");
  assertFalse(wifi.enterCommandMode());
  assertEqual("+++\r", stream.bytesWritten());
  assertFalse(wifi.isInCommandMode());
}

test (exitCommandModeOk) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream);
  assertFalse(wifi.isInCommandMode());

  stream.nextBytes("+OK\r\n\r\n");
  assertTrue(wifi.enterCommandMode());
  assertTrue(wifi.isInCommandMode());
  stream.reset();

  stream.nextBytes("+OK\r\n\r\n");
  assertTrue(wifi.exitCommandMode());
  assertFalse(wifi.isInCommandMode());
}
test (exitCommandModeErr) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream);
  assertFalse(wifi.isInCommandMode());

  stream.nextBytes("+OK\r\n\r\n");
  assertTrue(wifi.enterCommandMode());
  assertTrue(wifi.isInCommandMode());
  stream.reset();

  stream.nextBytes("+ERR=-1\r\n\r\n");
  assertFalse(wifi.exitCommandMode());
  assertTrue(wifi.isInCommandMode());
}

test (enterManualModeOk) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream);
  assertFalse(wifi.isInCommandMode());

  stream.nextBytes("+OK\r\n\r\n");
  assertTrue(wifi.enterCommandMode());
  assertTrue(wifi.isInCommandMode());
  stream.reset();

  stream.nextBytes("+OK\r\n\r\n");
  assertTrue(wifi.enterManualMode());
  assertFalse(wifi.isInCommandMode());
}

test (enterManualModeErr) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream);
  assertFalse(wifi.isInCommandMode());

  stream.nextBytes("+OK\r\n\r\n");
  assertTrue(wifi.enterCommandMode());
  assertTrue(wifi.isInCommandMode());
  stream.reset();

  stream.nextBytes("+ERR=-1\r\n\r\n");
  assertFalse(wifi.enterManualMode());
  assertTrue(wifi.isInCommandMode());
}

test (factoryReset) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream, true);
  assertTrue(wifi.isInCommandMode());

  stream.nextBytes("+OK\r\n\r\n");
  assertTrue(wifi.factoryReset());
  assertTrue(wifi.isInCommandMode());
  assertEqual("AT+RSTF\r", stream.bytesWritten());
}

test (reset) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream, true);
  assertTrue(wifi.isInCommandMode());

  stream.nextBytes("+OK\r\n\r\n");
  assertTrue(wifi.reset());
  assertFalse(wifi.isInCommandMode());
  assertEqual("AT+Z\r", stream.bytesWritten());
}

test (setHardwareResetPin) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream, true);
  assertTrue(wifi.isInCommandMode());
  assertTrue(wifi.setHardwareResetPin(12, true));
  assertFalse(wifi.setHardwareResetPin(12, true));
}

test (hardwareReset) {
  FakeStreamBuffer stream = FakeStreamBuffer();
  WifiUart wifi = WifiUart(&stream, true);
  assertTrue(wifi.isInCommandMode());
  assertFalse(wifi.hardwareReset());
  assertTrue(wifi.isInCommandMode());
  assertTrue(wifi.setHardwareResetPin(12, false));
  assertTrue(wifi.hardwareReset());
  assertFalse(wifi.isInCommandMode());
}


void setup() {
  Serial.begin(115200);
  Serial.println("");
}

void loop() {
  Test::run();
}
