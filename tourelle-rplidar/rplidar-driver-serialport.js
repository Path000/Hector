const EventEmitter = require('events')
	.EventEmitter;

const SerialPort = require('serialport');

/*
	emmits : error, open, close, dataAvailable
*/

class RPLidarDriverSerialport extends EventEmitter {

	constructor(path) {
		super();
		this.path = path;
	}

	open(next) {
		this.port = new SerialPort(this.path, {
			autoOpen: false,
			baudRate: 115200
		});

		this.port.on('error', (err) => {
			this.emit('error', err);
		});

		this.port.on('open', () => {
			this.emit('open');
		});

		this.port.on('data', (chunk) => {
			this.appendBuffer(chunk);
			this.emit('dataAvailable');
			// I choose to not use SerialPort parsers
			// Response format is given by request.
			// No parser set means no request that awaits response were emited
			// I trust my hardware.
		});

		this.port.on('close', () => {
			this.emit('close');
		});

		this.port.open((err) => {
			if (err) {
				this.emit('error', err);
				return;
			}
			this.resetBuffer();
			next();
		});
	}

	close(next) {
		this.port.close((err) => {
			if (next) next(err);
		});
	}

	resetBuffer() {
		this.dataBuffer = Buffer.alloc(0);
	}

	appendBuffer(chunk) {
		this.dataBuffer = Buffer.concat([this.dataBuffer, chunk]);
	}

	getBufferSize() {
		return this.dataBuffer.length;
	}

	consumeBuffer(bytes) {
		const buffer = this.dataBuffer.slice(0, bytes);
		this.dataBuffer = this.dataBuffer.slice(bytes);
		return buffer;
	}

	write(data, next) {
		this.port.write(data, () => {
			this.port.drain((err) => {
				next(err);
			});
		});
	}
}

module.exports = RPLidarDriverSerialport;