const express = require('express');
const app = express();
const server = require('http').Server(app);
const io = require('socket.io')(server);
const path = require('path');

const Arduino = require('./Arduino.js');
let arduinoTourelle = undefined;
let arduinoPropulsion = undefined;

const arduinoUSB0 = new Arduino('/dev/ttyUSB0');
const arduinoUSB1 = new Arduino('/dev/ttyUSB1');

let arduinoTourelleReady = false;
let arduinoPropulsionReady = false;

let currentSocket = undefined;

arduinoUSB0.on('READY', ()=>{
	console.log('arduinoUSB0 ready');
	arduinoUSB0.writeSerial('HELLO', console.log);
});
arduinoUSB1.on('READY', ()=>{
	console.log('arduinoUSB1 ready');
	arduinoUSB1.writeSerial('HELLO', console.log);
});

const initTourelle = (arduinoHandler)=> {
	arduinoTourelle = arduinoHandler;
	arduinoTourelleReady = true;

	arduinoTourelle.writeSerial('START', console.log);

	arduinoTourelle.on('MEAS', (data)=>{
			
		let msg = {};
		msg.angle = data.args[0];
		msg.distA = data.args[1];
		msg.distB = data.args[2];
		msg.distC = data.args[3];
		msg.distD = data.args[4];
		if(currentSocket != undefined) {
			currentSocket.emit('meas', msg);
		}
	});
}
const initPropulsion = (arduinoHandler)=> {
	arduinoPropulsion = arduinoHandler;
	arduinoPropulsionReady = true;

	arduinoPropulsion.on('FINISHED', (data)=>{
		console.log('MOVE FINISHED', data.args);
	});

	// TODO

	arduinoPropulsion.writeSerial('MOVE:1:6000:0.09:15:80');
}

arduinoUSB0.on('TOURELLE', ()=>{
	console.log('Arduino TOURELLE is on USB0');
	initTourelle(arduinoUSB0);
});
arduinoUSB0.on('PROPULSION', ()=>{
	console.log('Arduino PROPULSION is on USB0');
	initPropulsion(arduinoUSB0);
});
arduinoUSB1.on('TOURELLE', ()=>{
	console.log('Arduino TOURELLE is on USB1');
	initTourelle(arduinoUSB1);
});
arduinoUSB1.on('PROPULSION', ()=>{
	console.log('Arduino PROPULSION is on USB1');
	initPropulsion(arduinoUSB1);
});


app.use('/', express.static(path.join(__dirname, 'html_root')));

io.on('connection', function (socket) {
	currentSocket = socket;
	socket.on('error', console.log)
});

server.listen(8080);

const exitHandler = (options, err)=> {
  
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
