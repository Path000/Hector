const express = require('express');
const app = express();
const server = require('http')
	.Server(app);
const io = require('socket.io')(server);
const path = require('path');

const lidar = require('./tourelle-rplidar');

const emitter = lidar.getScanEmitter();

emitter.on('scan', (data) => {
	if (currentSocket != undefined) currentSocket.emit('scan', data);
});

let currentSocket = undefined;

app.use('/', express.static(path.join(__dirname, 'html_root')));

io.on('connection', function(socket) {
	currentSocket = socket;
	socket.on('error', console.log);
	socket.on('start', lidar.start);
	socket.on('stop', lidar.stop);
});


server.listen(8080);

const exitHandler = () => {
	console.log("Clean exit");
	lidar.stop();
	process.exit();
}

//catches ctrl+c event
process.on('SIGINT', exitHandler.bind(null, {}));
//catches uncaught exceptions
process.on('uncaughtException', exitHandler.bind(null, {}));
