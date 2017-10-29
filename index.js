const express = require('express');
const app = express();
const server = require('http').Server(app);
const io = require('socket.io')(server);
const path = require('path');

const Arduino = require('./Arduino.js');
const arduinoTourelle = new Arduino('/dev/ttyUSB0');

let arduinoTourelleReady = false;

let currentSocket = undefined;

arduinoTourelle.on('READY', ()=>{
	console.log('Tourelle ready');
	arduinoTourelle.writeSerial('HELLO', console.log);
});

arduinoTourelle.on('TOURELLE', ()=>{
	arduinoTourelleReady = true;
	arduinoTourelle.writeSerial('START', console.log);
});

arduinoTourelle.on('MEAS', (data)=>{
	if(currentSocket != undefined) {
		let msg = {};
		msg.angle = data.args[0];
		msg.distA = data.args[1];
		msg.distB = data.args[2];
		msg.distC = data.args[3];
		msg.distD = data.args[4];
		currentSocket.emit('meas', msg);
	}
});


app.use('/', express.static(path.join(__dirname, 'html_root')));

io.on('connection', function (socket) {
	currentSocket = socket;
	socket.on('error', console.log)
});

server.listen(8080);

let exitHandler = (options, err)=> {
  
	if (err) console.log(err.message);

	if (options.exit) {
		arduinoTourelle.writeSerial('STOP', ()=>{
			process.exit();
		});
	}
}

//catches ctrl+c event
process.on('SIGINT', exitHandler.bind(null, {exit:true}));
//catches uncaught exceptions
process.on('uncaughtException', exitHandler.bind(null, {exit:true}));
