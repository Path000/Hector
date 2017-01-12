let express = require('express');
let app = express();
let server = require('http').Server(app);
let io = require('socket.io')(server);
let path = require('path');

server.listen(8080);

app.use('/html', express.static(path.join(__dirname, 'html_root')));

let angle = 0;
function getMeasurement() {
	if(angle > Math.PI) {
		angle = 0;
	}
	let data = {};
	data.angle = angle
	data.distA = 50+(10*Math.random());
	data.distB = 50+(10*Math.random());
	data.distC = 50+(10*Math.random());
	data.distD = 50+(10*Math.random());
	angle = angle + (Math.PI/2/90);
	return data;
}

io.on('connection', function (socket) {
	setInterval(() => {
		socket.emit('meas', getMeasurement());
	}, 10);
});