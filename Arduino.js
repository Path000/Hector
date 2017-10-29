const EventEmitter = require('events').EventEmitter;
const SerialPort = require('serialport');

const Readline = SerialPort.parsers.Readline;

class Arduino extends EventEmitter {
	constructor(path) {
		super();
		this.serialIsOpened = false;
		this.parser = new Readline({ delimiter: '\n' });
		this.port = new SerialPort(path, {
			baudRate: 115200,
			autoOpen: true
		});
		this.port.on('open', () => {
			console.log('Port :', path, 'is openned');
			this.serialIsOpened = true;
			this.port.pipe(this.parser);

			this.port.on('error', (err)=>{
				console.log('Error : ', err.message);
			});
			
			this.port.on('close', ()=>{
				console.log('Port closed.');
			});
			

			this.parser.on('data', (data)=>{
				this.parse(data, (command, args )=> {
					this.emit(command, args);
				});
			});
		});
	}

	parse(data, next) {
		let splitedData = data.trim().split(':');
		let command = splitedData.shift();
		next(command, {args: splitedData});
	}

	writeSerial(message, next) {
		if(this.serialIsOpened) {
			this.port.write(message+"\n", 'ascii', next);
		}
		else {
			let err = new Error('Serial port not openned');
			next(err);
		}
	}
}

module.exports = Arduino;