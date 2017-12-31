const express = require('express');
const app = express();
const server = require('http')
	.Server(app);
const io = require('socket.io')(server);
const path = require('path');

let currentSocket = undefined;

let timeoutHandler = undefined;

let teta = 0;

const startFakeScan = () => {
	const packet = [];
	let packetSize = 32;
	while(packetSize >0) {
		packet.push({
			distance: 2000,
			angle: teta
		});
		teta++;
		if(teta>=360) teta=0;
		packetSize--;
	}
	if (currentSocket != undefined) currentSocket.emit('scan', packet);
	timeoutHandler = setTimeout(startFakeScan, 1000);
}

const stopFakeScan = () => {
	if(timeoutHandler) clearTimeout(timeoutHandler);
	teta=0;
}

app.use('/', express.static(path.join(__dirname, 'html_root')));

io.on('connection', function(socket) {
	currentSocket = socket;
	socket.on('error', console.log);
	socket.on('start', startFakeScan);
	socket.on('stop', stopFakeScan);
});

server.listen(8080);

const exitHandler = () => {
	console.log("Clean exit");
	process.exit();
}

//catches ctrl+c event
process.on('SIGINT', exitHandler.bind(null, {}));
//catches uncaught exceptions
process.on('uncaughtException', exitHandler.bind(null, {}));
