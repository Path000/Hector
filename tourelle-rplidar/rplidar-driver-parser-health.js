const DESCRIPTOR_SIZE = 7;

const PAYLOAD_SIZE = 3;

const DESCRIPTOR = Buffer.from([0xA5, 0x5A, 0x03, 0x00, 0x00, 0x00, 0x06]);

const HEALTH_STATUSES = ['Good', 'Warning', 'Error'];

class RPLidarDriverParserHealth {

	constructor(response, port) {
		this.response = response;
		this.port = port;
	}

	parse() {
		this.response.emit('debug', 'Parsing Health Data');

		const bytesToConsume = DESCRIPTOR_SIZE + PAYLOAD_SIZE;

		if (this.port.getBufferSize() < bytesToConsume) return;

		const buffer = this.port.consumeBuffer(bytesToConsume);

		if (DESCRIPTOR.equals(buffer.slice(0, DESCRIPTOR_SIZE))) {
			const payload = buffer.slice(DESCRIPTOR_SIZE);
			const byte0 = payload[0];
			const parsedData = {
				statusCode: byte0,
				status: HEALTH_STATUSES[byte0],
				errorCode: (payload[2] << 8) | payload[1]
			};
			this.response.emit('health', parsedData);
		}
		this.port.resetBuffer();
		this.response.unsetParser();
	}
}
module.exports = RPLidarDriverParserHealth;