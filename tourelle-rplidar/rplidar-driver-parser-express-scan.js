const DESCRIPTOR_SIZE = 7;

const HEADER_SIZE = 4;

const CABINS_SIZE = 80;

const DESCRIPTOR = Buffer.from([0xA5, 0x5A, 0x54, 0x00, 0x00, 0x40, 0x82]);

class RPLidarDriverParserExpressScan {
	constructor(response, port) {
		this.response = response;
		this.port = port;
		this.firstPacket = true;
		this.response.emit('debug', 'RPLidarDriverParserExpressScan constructed');
	}

	parse() {
		if (this.firstPacket) {
			this.firstPacket = false;
			this.parseDescriptorAndFirstHeader();
			return;
		}
		this.parseCabinsAndNextHeader();
	}

	parseDescriptorAndFirstHeader() {
		this.response.emit('debug', 'Parsing Express Scan Data');

		const bytesToConsume = DESCRIPTOR_SIZE + HEADER_SIZE;

		if (this.port.getBufferSize() < bytesToConsume) return;

		const buffer = this.port.consumeBuffer(bytesToConsume);

		if (DESCRIPTOR.equals(buffer.slice(0, DESCRIPTOR_SIZE))) {

			this.currentHeader = this.parseHeader(buffer.slice(DESCRIPTOR_SIZE));

			setTimeout(this.parse, 0);
		}
	}

	parseCabinsAndNextHeader() {

		if (this.currentHeader.sync1 != 0xA || this.currentHeader.sync2 != 0x5) {
			this.emit('error', new Error(`Wrong sync values ${this.currentHeader.sync1} ${this.currentHeader.sync2}`));
			return;
		}

		if (this.currentHeader.startFlag != 0) {
			this.emit('debug', 'Start flag was true !');
		}

		if (this.port.getBufferSize() < CABINS_SIZE + HEADER_SIZE) return;

		this.emit('debug', `Buffer length : ${this.dataBuffer.length}`);

		const bufferCabins = this.port.consumeBuffer(CABINS_SIZE);
		const bufferNextHeader = this.port.consumeBuffer(HEADER_SIZE)

		if (this.computeCheckSum(this.currentHeader.buffer, bufferCabins) != this.currentHeader.checkSum) {
			this.emit('error', new Error('Wrong check sum.'));
			return;
		}

		const nextHeader = this.parseHeader(bufferNextHeader);

		const scans = [];

		for (let index = 0; index < 16; index++) {

			const buffCabin = bufferCabins.slice(index * 5, index * 5 + 5);

			const cabin = this.parseCabin(buffCabin);

			const scan1 = {}
			scan1.distance = cabin.distance1;
			scan1.angle = this.computeAngle(index * 2, this.currentHeader.startAngle, nextHeader.startAngle, cabin.deltaAngle1);
			scans.push(scan1);

			const scan2 = {}
			scan2.distance = cabin.distance2;
			scan2.angle = this.computeAngle(index * 2 + 1, this.currentHeader.startAngle, nextHeader.startAngle, cabin.deltaAngle2);
			scans.push(scan2);
		}

		this.emit('debug', this.currentHeader);
		this.emit('scan', scans);

		this.currentHeader = nextHeader;
		setTimeout(this.parse, 0);
	}

	computeCheckSum(headerBuffer, cabinBuffer) {

		let xorBuffer = cabinBuffer[0];
		for (let index = 1; index < 80; index++) {
			xorBuffer = xorBuffer ^ cabinBuffer[index];
		}
		for (const value of headerBuffer) {
			xorBuffer = xorBuffer ^ value;
		}
		return xorBuffer;
	}

	angleDiff(angle1, angle2) {
		if (angle1 <= angle2) return (angle2 - angle1);
		return (360 + angle2 - angle1);
	}

	computeAngle(k, angle1, angle2, delta) {
		return angle1 - delta + (this.angleDiff(angle1, angle2) * k / 32);
	}

	parseHeader(buffer) {
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

	parseCabin(buffer) {
		const cabin = {};

		const byte0 = buffer[0];
		const distance1LSB = (byte0 & 0b11111100) >>> 2;
		const deltaAngle1MSB = byte0 & 0b00000011;

		const distance1MSB = buffer[1];
		cabin.distance1 = (distance1MSB << 6) | distance1LSB;

		const byte2 = buffer[2];
		const distance2LSB = (byte2 & 0b11111100) >>> 2;
		const deltaAngle2MSB = byte2 & 0b00000011;

		const distance2MSB = buffer[3];
		cabin.distance2 = (distance2MSB << 6) | distance2LSB;

		const byte4 = buffer[4];
		const deltaAngle2LSB = (byte4 & 0xF0) >>> 4;
		const deltaAngle1LSB = byte4 & 0x0F;

		cabin.deltaAngle1 = (deltaAngle1MSB << 4) | deltaAngle1LSB;
		cabin.deltaAngle2 = (deltaAngle2MSB << 4) | deltaAngle2LSB;

		return cabin;
	}
}
module.exports = RPLidarDriverParserExpressScan;