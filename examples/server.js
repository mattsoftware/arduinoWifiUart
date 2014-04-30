#!/usr/bin/env node

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

var net = require('net');

var server = net.createServer(function (socket) {
	console.log("Client Connected");
	socket.on('end', function () {
		console.log("Client Disconnected");
	});
	var currentState = false;
	var receiveChar = function (c) {
		if (c === "T") {
			currentState = !currentState;
			var command = currentState ? '1' : '0';
			process.stdout.write("\nSending command: " + command + ": ");
			socket.write(command);
		} else {
			process.stdout.write(c);
		}
	};
	socket.on('data', function(data) {
		for (var x = 0; x < data.length; x++) {
			receiveChar(data.toString().charAt(x));
		}
	});
});

server.listen(7777);
