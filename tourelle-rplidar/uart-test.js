const SerialPort = require('serialport');

const port = new SerialPort('/dev/ttyAMA0', {
	autoOpen: false,
	baudRate: 115200
});

const send = () => {
	port.write('Je teste le port série 1 2 3 4 5', () => {
		port.drain((err) => {
			if (err) {
				console.log(err);
				return;
			}
			console.log('drained');
		});
	});
	setTimeout(send, 1000);
}

port.on('open', () => {
	console.log('Opened');
	send();
});

port.on('data', (chunk) => {
	console.log(chunk);
	console.log(chunk.toString());
});

port.open((err) => {
	if (err) {
		console.log(err);
		return;
	}
});