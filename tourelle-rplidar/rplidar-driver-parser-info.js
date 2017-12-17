const DESCRIPTOR_SIZE = 7;

const PAYLOAD_SIZE = 20;

const DESCRIPTOR = Buffer.from([0xA5, 0x5A, 0x14, 0x00, 0x00, 0x00, 0x04]);

class RPLidarDriverParserInfo {

	constructor(response, port) {
		this.response = response;
		this.port = port;
		this.response.emit('debug', 'RPLidarDriverParserInfo constructed');
	}

	parse() {
		const bytesToConsume = DESCRIPTOR_SIZE + PAYLOAD_SIZE;

		if (this.port.getBufferSize() < bytesToConsume) return;

		this.response.emit('debug', 'Parsing Info Data');

		const buffer = this.port.consumeBuffer(bytesToConsume);

		if (DESCRIPTOR.equals(buffer.slice(0, DESCRIPTOR_SIZE))) {
			const payload = buffer.slice(DESCRIPTOR_SIZE);
			const parsedData = {
				model: payload[0],
				firmware_minor: payload[1],
				firmware_major: payload[2],
				hardware: payload[3],
				serial_number: payload.slice(4, 20)
			};
			this.response.emit('info', parsedData);
		}
		this.port.resetBuffer();
		this.response.unsetParser();
	}
}

module.exports = RPLidarDriverParserInfo;