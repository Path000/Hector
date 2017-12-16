const EventEmitter = require('events')
	.EventEmitter;
const SerialPort = require('serialport');
const modulePigpio = require('pigpio');

const START_FLAG = 0xA5;
const START_FLAG2 = 0x5A;
const REQUESTS = {
	STOP: Buffer.from([START_FLAG, 0x25]),
	GET_HEALTH: Buffer.from([START_FLAG, 0x52]),
	GET_INFO: Buffer.from([START_FLAG, 0x50]),
	//SCAN: Buffer.from([START_FLAG, 0x20]),
	EXPRESS_SCAN: Buffer.from([START_FLAG, 0x82, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22]),
};
const RESPONSES = {
	GET_HEALTH: {
		descriptor: Buffer.from([START_FLAG, START_FLAG2, 0x03, 0x00, 0x00, 0x00, 0x06]),
		payloadLength: 3
	},
	GET_INFO: {
		descriptor: Buffer.from([START_FLAG, START_FLAG2, 0x14, 0x00, 0x00, 0x00, 0x04]),
		payloadLength: 20
	},
	EXPRESS_SCAN: {
		descriptor: Buffer.from([START_FLAG, START_FLAG2, 0x54, 0x00, 0x00, 0x40, 0x82]),
		payloadLength: 84
	}
};

const HEALTH_STATUSES = ['Good', 'Warning', 'Error'];

class RPLidarDriver extends EventEmitter {

	constructor(path, motorPWMDuty) {
		super();
		this.path = path;
		this.motorPWMDuty = motorPWMDuty;
		const Gpio = modulePigpio.Gpio;
		this.motorPWM = new Gpio(18);
		this.motorPWM.mode(Gpio.OUTPUT);
	}

	init(next) {
		this.port = new SerialPort(this.path, {
			autoOpen: false,
			baudRate: 115200
		});

		this.port.on('error', (err) => {
			this.emit('error', err);
		});

		this.port.on('open', () => {
			this.emit('open', {});
		});

		this.port.on('data', (chunk) => {
			this.dataBuffer = Buffer.concat([this.dataBuffer, chunk]);
			this.parse();
		});

		this.port.on('close', () => {
			this.emit('close', {});
		});

		this.port.open((err) => {
			if (err) {
				this.emit('error', err);
				return;
			}
			this.resetParser();
			next();
		});
	}

	parse() {
		// I choose to not use SerialPort parsers
		// Response format is given by request.
		// No parser set means no request that awaits response were emited
		// I trust my hardware.
		if (this.responseParser == undefined) return;
		this.responseParser();
	}

	resetParser() {
		this.dataBuffer = Buffer.alloc(0);
		this.responseParser = undefined;
		//this.emit('ready', {});
	}

	setParser(method) {
		this.dataBuffer = Buffer.alloc(0);
		this.responseParser = method;
	}

	parseHealthData() {
		console.log('Parsing Info Data');
		if (this.dataBuffer.length < 7 + RESPONSES.GET_HEALTH.payloadLength) return;

		if (0 == RESPONSES.GET_HEALTH.descriptor.compare(this.dataBuffer, 0, 7)) {
			const byte0 = this.dataBuffer[7];
			const byte1 = this.dataBuffer[8];
			const byte2 = this.dataBuffer[9];
			const parsedData = {
				statusCode: byte0,
				status: HEALTH_STATUSES[byte0],
				errorCode: (byte2 << 8) | byte1
			};
			this.emit('health', parsedData);
		}
		this.resetParser();
	}

	parseInfoData() {
		console.log('Parsing Info Data');
		if (this.dataBuffer.length < 7 + RESPONSES.GET_INFO.payloadLength) return;

		if (0 == RESPONSES.GET_INFO.descriptor.compare(this.dataBuffer, 0, 7)) {
			const parsedData = {
				model: this.dataBuffer[7],
				firmware_minor: this.dataBuffer[8],
				firmware_major: this.dataBuffer[9],
				hardware: this.dataBuffer[10],
				serial_number: this.dataBuffer.slice(11, 27)
			};
			this.emit('info', parsedData);
		}
		this.resetParser();
	}

	parseExpressScan() {
		console.log('Parsing Express Scan Data');
		if (this.dataBuffer.length < 7) return;
		if (0 == RESPONSES.EXPRESS_SCAN.descriptor.compare(this.dataBuffer, 0, 7)) {
			this.dataBuffer = this.dataBuffer.slice(7);
			this.setParser(this.parseExpressScanPayload);
			this.parse();
		}
	}

	parseExpressScanPayload() {
		if (this.dataBuffer.length < RESPONSES.EXPRESS_SCAN.payloadLength) return;

		console.log(this.dataBuffer.slice(0, RESPONSES.EXPRESS_SCAN.payloadLength));

		let packet = {};

		let byte0 = this.dataBuffer[0];
		let sync1 = (byte0 & 0xF0) >>> 4;
		let chkSumLSB = byte0 & 0x0F;

		let byte1 = this.dataBuffer[1];
		let sync2 = (byte1 & 0xF0) >>> 4;
		let chkSumMSB = byte1 & 0x0F;

		if (sync1 != 0xA || sync2 != 0x5) {
			console.log('Wrong sync values');
			return;
		}

		packet.chkSum = (chkSumMSB << 4) | chkSumLSB;

		let startAngleQ6LSB = this.dataBuffer[2];

		let byte3 = this.dataBuffer[3];
		let startAngleQ6MSB = byte3 & 0b01111111;

		packet.startFlag = (byte3 & 0b10000000) >> 7;

		packet.startAngle = ((startAngleQ6MSB << 8) | startAngleQ6LSB) / 64;

		packet.cabin = [];

		for (let index = 0; index < 16; index++) {
			let buffCabin = this.dataBuffer.slice(index * 5 + 4, index * 5 + 9);
			let cabin = {};

			packet.cabin.push(buffCabin);
		}

		console.log(packet);

		this.dataBuffer = this.dataBuffer.slice(RESPONSES.EXPRESS_SCAN.payloadLength);
		setTimeout(this.parse, 0);
	}

	writePort(data, next) {
		this.port.write(data, () => {
			this.port.drain((err) => {
				next(err);
			});
		});
	}

	resetLidar(next) {
		this.resetParser();
		this.writePort(REQUESTS.RESET, (err) => {
			if (err) {
				this.emit('error', err);
				return;
			}
			console.log('lidar reseted');
			next();
		});
	}

	getLidarHealth(next) {
		this.setParser(this.parseHealthData);
		console.log('getLidarHealth requested');
		this.writePort(REQUESTS.GET_HEALTH, next);
	}

	getLidarInfo(next) {
		this.setParser(this.parseInfoData);
		console.log('getLidarInfo requested');
		this.writePort(REQUESTS.GET_INFO, next);
	}

	startExpressScan(next) {
		this.setParser(this.parseExpressScan);
		console.log('startExpressScan requested');
		setTimeout(() => {
			this.writePort(REQUESTS.EXPRESS_SCAN, next);
		}, 10);
	}

	stopScan(next) {
		setTimeout(() => {

			this.writePort(REQUESTS.STOP, (err) => {
				console.log('lidar stoped');
				if (err) {
					this.emit('error', err);
					return;
				}
				this.resetParser();
				next();
			});
		}, 10);
	}

	startMotor(next) {
		let duty = 1000000 * this.motorPWMDuty / 100;
		this.motorPWM.hardwarePwmWrite(25000, duty);
		next();
	}

	stopMotor(next) {
		this.motorPWM.hardwarePwmWrite(0, 0);
		next();
	}
}

module.exports = RPLidarDriver;