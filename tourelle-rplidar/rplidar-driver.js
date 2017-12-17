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

const angleDiff = (angle1, angle2) => {
	if (angle1 <= angle2) return (angle2 - angle1);
	return (360 + angle2 - angle1);
}

const computeAngle = (k, angle1, angle2, delta) => {
	return angle1 - delta + (angleDiff(angle1, angle2) * k / 32);
}

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
		// 7 bytes descriptor + 4 bytes first header
		if (this.dataBuffer.length < 11) return;

		if (RESPONSES.EXPRESS_SCAN.descriptor.equals(this.dataBuffer.slice(0, 7))) {

			this.currentHeader = this.parseExpressScanHeader(this.dataBuffer.slice(7, 11));
			this.dataBuffer = this.dataBuffer.slice(11);

			this.setParser(this.parseExpressScanPayload);
			this.parse();
		}
	}

	parseExpressScanHeader(buffer) {
		const head = {};

		head.buffer = buffer;

		const byte0 = buffer[0];
		head.sync1 = (byte0 & 0xF0) >>> 4;
		const chkSumLSB = byte0 & 0x0F;

		const byte1 = buffer[1];
		head.sync2 = (byte1 & 0xF0) >>> 4;
		const chkSumMSB = byte1 & 0x0F;

		head.checkSum = (chkSumMSB << 4) | chkSumLSB;

		const startAngleQ6LSB = buffer[2];

		const byte3 = buffer[3];
		const startAngleQ6MSB = byte3 & 0b01111111;

		head.startAngle = ((startAngleQ6MSB << 8) | startAngleQ6LSB) / 64;

		head.startFlag = byte3 & 0b10000000;

		return head;
	}

	parseExpressScanCabin(buffCabin) {
		const cabin = {};

		const byte0 = buffCabin[0];
		const distance1LSB = (byte0 & 0b11111100) >>> 2;
		const deltaAngle1MSB = byte0 & 0b00000011;

		const distance1MSB = buffCabin[1];
		cabin.distance1 = (distance1MSB << 6) | distance1LSB;

		const byte2 = buffCabin[2];
		const distance2LSB = (byte2 & 0b11111100) >>> 2;
		const deltaAngle2MSB = byte2 & 0b00000011;

		const distance2MSB = buffCabin[3];
		cabin.distance2 = (distance2MSB << 6) | distance2LSB;

		const byte4 = buffCabin[4];
		const deltaAngle2LSB = (byte4 & 0xF0) >>> 4;
		const deltaAngle1LSB = byte4 & 0x0F;

		cabin.deltaAngle1 = (deltaAngle1MSB << 4) | deltaAngle1LSB;
		cabin.deltaAngle2 = (deltaAngle2MSB << 4) | deltaAngle2LSB;

		return cabin;
	}

	computeCheckSum(headerBuffer, cabinBuffer) {

		let xorBuffer = cabinBuffer[0];
		for (let index = 1; index < 80; index++) {
			xorBuffer = xorBuffer ^ cabinBuffer[index];
		}
		for (const value of headerBuffer) {
			xorBuffer = xorBuffer ^ value;
		}

	}

	parseExpressScanPayload() {

		if (this.currentHeader.sync1 != 0xA || this.currentHeader.sync2 != 0x5) {
			this.emit('error', new Error(`Wrong sync values ${this.currentHeader.sync1} ${this.currentHeader.sync2}`));
			return;
		}

		if (this.currentHeader.startFlag != 0) {
			console.log('Start flag was true !');
		}

		const payloadLength = RESPONSES.EXPRESS_SCAN.payloadLength;

		if (this.dataBuffer.length < payloadLength) return;

		console.log(`Buffer length : ${this.dataBuffer.length}`);

		const packetBuffer = this.dataBuffer.slice(0, payloadLength);

		this.dataBuffer = this.dataBuffer.slice(payloadLength);

		if (this.computeCheckSum(this.currentHeader.buffer, packetBuffer) != this.currentHeader.checkSum) {
			this.emit('error', new Error(`Wrong check sum. Computed : ${xorBuffer}. Given : ${checkSum}`));
			return;
		}

		const nextHeader = this.parseExpressScanHeader(packetBuffer.slice(80, 84));

		const scans = [];

		for (let index = 0; index < 16; index++) {
			const buffCabin = packetBuffer.slice(index * 5, index * 5 + 5);
			const cabin = this.parseExpressScanCabin(buffCabin);
			const scan1 = {}
			scan1.distance = cabin.distance1;
			scan1.angle = computeAngle(index * 2, this.currentHeader.startAngle, nextHeader.startAngle, cabin.deltaAngle1);
			scans.push(scan1);
			const scan2 = {}
			scan2.distance = cabin.distance2;
			scan2.angle = computeAngle(index * 2 + 1, this.currentHeader.startAngle, nextHeader.startAngle, cabin.deltaAngle2);
			scans.push(scan2);
		}

		console.log(this.currentHeader);
		console.log(scans);

		this.currentHeader = nextHeader;
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