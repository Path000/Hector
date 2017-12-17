const EventEmitter = require('events')
	.EventEmitter;

const REQUESTS = {
	STOP: Buffer.from([0xA5, 0x25]),
	GET_HEALTH: Buffer.from([0xA5, 0x52]),
	GET_INFO: Buffer.from([0xA5, 0x50]),
	EXPRESS_SCAN: Buffer.from([0xA5, 0x82, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x22]),
};

class RPLidarDriverRequest extends EventEmitter {

	constructor(port, response) {
		super();
		this.port = port;
		this.response = response;
	}

	getLidarHealth(next) {
		this.response.setHealthParser();
		this.emit('debug', 'getLidarHealth requested');
		this.port.write(REQUESTS.GET_HEALTH, next);
	}

	getLidarInfo(next) {
		this.response.setInfoParser();
		this.emit('debug', 'getLidarInfo requested');
		this.port.write(REQUESTS.GET_INFO, next);
	}

	startExpressScan(next) {
		this.response.setExpressScanParser(t);
		this.emit('debug', 'startExpressScan requested');
		this.port.write(REQUESTS.EXPRESS_SCAN, next);
	}

	stopScan(next) {
		this.response.unsetParser();
		this.emit('debug', 'stopScan requested');
		this.port.write(REQUESTS.STOP, next);
	}
}
module.exports = RPLidarDriverRequest;