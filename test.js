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

var fs = require('fs');
var Q = require('q');
var serialport = require('serialport');
var childProcess = require('child_process');
var SerialPort = serialport.SerialPort;
var board = process.argv[2];
var dev = process.argv[3];
var baud = process.argv[4];
var testFileSpec = process.argv[5];
var debug = process.argv[6] && true || false;

var usage = function () {
    process.stdout.write("Usage: tests.js <board> <port> <baud> <test file spec> [debug]\n");
    process.stdout.write("       Where: <board> is the board type (e.g. uno, pro)\n");
    process.stdout.write("              <test file spec> can be an ino file, an ino directory, or a directory containing ino directories\n");
    process.stdout.write("              [debug] optional parameter to set debug mode\n");
    process.exit(2);
}
if (!testFileSpec) {
	usage();
}

var collectInoFilesToTest = function (baseDir) {
	var inos = [];
	baseDir = fs.realpathSync(baseDir);

	var baseDirStat = fs.statSync(baseDir);
	if (baseDirStat.isFile()) {
		inos.push(baseDir);
	} else if (baseDirStat.isDirectory()) {
        var baseDirName = baseDir.substring(baseDir.lastIndexOf('/')+1);
        if (fs.existsSync(baseDir + '/' + baseDirName + '.ino')) {
            inos.push(baseDir + '/' + baseDirName + '.ino');
        } else {
			var dirs = fs.readdirSync(baseDir);
			for (var x in dirs) {
				if (dirs.hasOwnProperty(x)) {
					if (dirs[x] !== '.' && dirs[x] !== '..') {
						inos.push(baseDir + '/' + dirs[x] + '/' + dirs[x] + '.ino');
					}
				}
			}
		}
	}

	return inos;
};
var inos = collectInoFilesToTest(testFileSpec);

function runTests (dev, baud, callback) {
	var serial = new SerialPort(dev, {
		baudrate: baud,
		parser: serialport.parsers.readline("\r\n")
	});
	serial.on('open', function () {
		var testRunLines = [];

		serial.on('data', function (line) {
			if (/^Test .* failed\.$/.test(line)) {
				process.stdout.write('F');
			} else if (/^Test .* skipped\.$/.test(line)) {
				process.stdout.write('S');
			} else {
				process.stdout.write('.');
			}
			if (/^Test .*\.$/.test(line) || /^Assertion failed:/.test(line)) {
				testRunLines.push(line);
			}
			var parseSummary = /^Test summary: ([0-9]+) passed, ([0-9]+) failed, and ([0-9]+) skipped, out of ([0-9]+) test\(s\).$/;
			var summaryResult = parseSummary.exec(line);
			if (summaryResult) {
				serial.flush(function (err) {
					serial.drain(function (err) {
						serial.close(function (err) {
							process.stdout.write('\n');
							callback(err, {
								output: testRunLines,
								passed: summaryResult[1],
								failed: summaryResult[2],
								skipped: summaryResult[3],
								total: summaryResult[4]
							});
						});
					});
				});
			}
		});
	});
	serial.on('error', function (err) {
		callback(err, null);
	});
	serial.on('exit', function () {
		callback(null, 'exited');
	});
};

var arduinoCommand = function (arduinoOptions, callback) {
	var output = '';
	var errOutput = '';
	var boardParam = [arduinoOptions.package, arduinoOptions.architecture, arduinoOptions.board].join(':');
	if (arduinoOptions.parameters.length > 0) {
		boardParam = [boardParam, arduinoOptions.parameters].join(':');
	}
	var cmdParams = [
		'--' + arduinoOptions.command,
		'--board', boardParam,
		'--port', arduinoOptions.port,
		arduinoOptions.ino
	];
    if (debug) {
        console.log("Running arduino command: " + arduinoOptions.binary + " " + cmdParams.join(" "));
    }
	var cmd = childProcess.spawn(arduinoOptions.binary, cmdParams);
	cmd.on('error', function (err) {
    if (debug) {
        console.log("Arduino command failed");
    }
		callback(err, null);
	});
	cmd.stdout.on('data', function(data) {
		output += data;
	});
	cmd.stderr.on('data', function(data) {
		errOutput += data;
	});
	cmd.on('close', function(code){
		if (code !== 0) {
			var error = new Error(arduinoOptions.command + ' exited with code ' + code);
			error.code = code;
			error.commandLine = arduinoOptions.binary + ' ' + cmdParams.join(' ');
			error.output = output;
			error.errorOutput = errOutput;
			callback(error, null);
		} else {
			callback(null, output);
		}
	});
};

var arduinoVerify = function (ino, arduinoOptions, callback) {
	var verifyArduinoOptions = JSON.parse(JSON.stringify(arduinoOptions));
	verifyArduinoOptions.command = 'verify';
	verifyArduinoOptions.ino = ino;
	arduinoCommand(verifyArduinoOptions, callback);
};

var arduinoUpload = function (ino, arduinoOptions, callback) {
	var uploadArduinoOptions = JSON.parse(JSON.stringify(arduinoOptions));
	uploadArduinoOptions.command = 'upload';
	uploadArduinoOptions.ino = ino;
	arduinoCommand(uploadArduinoOptions, callback);
}

var arduinoOptions = {
	binary: '/Applications/Arduino.app/Contents/MacOS/JavaApplicationStub',
	package: 'arduino',
	architecture: 'avr',
    board: board,
	parameters: '',
	port: dev
};

var fullTestRun = function (ino, arduinoOptions, baud, previousResults) {
	if (!previousResults) {
		previousResults = [];
	}
	console.log("Running tests for " + ino);
	var ret = Q.defer();
	//Q.fcall(function() { return Q.nfcall(arduinoVerify, ino, arduinoOptions); })
	//.then(function () { return Q.nfcall(arduinoUpload, ino, arduinoOptions); })
	Q.fcall(function() { return Q.nfcall(arduinoUpload, ino, arduinoOptions); })
	.then(function () { return Q.nfcall(runTests, arduinoOptions.port, baud); })
	.then(function (result) {
		result.filename = ino;
		previousResults.push(result);
		ret.resolve(previousResults);
	})
	.fail(function (err) {
		ret.reject(err);
	});
	return ret.promise;
};
var fullTestRunPromise = function (ino, arduinoOptions, baud) {
	return function (result) {
		return fullTestRun(ino, arduinoOptions, baud, result);
	};
}

var promise = Q(null);
for (var ino in inos) {
	if (inos.hasOwnProperty(ino)) {
		promise = promise.then(fullTestRunPromise(inos[ino], arduinoOptions, baud));
	}
}

var summariseResult = function (result) {
	//console.log(result);
	var summary = {
		passed: 0,
		failed: 0,
		skipped: 0,
		total: 0,
		failedTests: [],
	};
	for (var r in result) {
		if (result.hasOwnProperty(r)) {
			summary.passed += +result[r].passed;
			summary.failed += +result[r].failed;
			summary.skipped += +result[r].skipped;
			summary.total += +result[r].total;
			if (+result[r].failed > 0) {
				for (var l in result[r].output) {
					if (result[r].output.hasOwnProperty(l)) {
						var line = result[r].output[l];
						if (/^Assertion failed:/.test(line)) {
							summary.failedTests.push({
								filename: result[r].filename,
								line: line
							});
						}
					}
				}
			}
		}
	}
	return summary;
};

promise.then(function (result) {
	//console.log(result);
	var summarised = summariseResult(result);
	console.log(summarised);
})
.fail(function(err) {
	console.log("ERROR: " + err + " (" + err.code + ")");
	if (err.commandLine) {
		console.log(err.commandLine.trim());
		console.log("-----");
		console.log(err.errorOutput.trim());
		console.log("-----");
	}
});
