const exitHandler = () => {
	console.log("Clean exit");
	process.exit();
}

//catches ctrl+c event
process.on('SIGINT', exitHandler.bind(null, {}));
//catches uncaught exceptions
process.on('uncaughtException', exitHandler.bind(null, {}));

// ===============================================================================

const SerialPort = require('serialport');

let port = new SerialPort("/dev/tty.usbmodem1411", {
	autoOpen: false,
	baudRate: 115200
});

port.on('error', (err) => {
	console.log("Error on port")
	console.log(err);
});

port.on('open', () => {
	console.log("Port opened");
});

port.on('close', () => {
	console.log("Port closed");
});

port.on('data', (chunk) => {
	let data = Buffer.from(chunk, 'binary');
	if (data.startwith('SCAN:')) {
		if (currentSocket) {
			let jsonData = {};
			data = data.slice(5);
			let sepPos = data.indexOf(':');
			jsonData.x = data.slice(0, sepPos)
				.toString();
			jsonData.y = data.slice(sepPos + 1)
				.toString();

			currentSocket.emit('scan', jsonData);
		}
	} else {
		console.log(data.toString());
	}
});

port.open((err) => {
	if (err) {
		console.log("Error on open port")
		console.log(err);
		return;
	}
});

// ===============================================================================

const express = require('express');
const app = express();
const server = require('http')
	.Server(app);
const io = require('socket.io')(server);
const path = require('path');

let currentSocket = undefined;

app.use('/', express.static(path.join(__dirname, 'html_root')));

io.on('connection', function(socket) {
	currentSocket = socket;
	socket.on('error', console.log);
	socket.on('start', lidar.start);
	socket.on('stop', lidar.stop);
});

server.listen(8080);