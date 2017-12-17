const EventEmitter = require('events')
	.EventEmitter;

const RPLidarDriverParserHealth = require('./rplidar-driver-parser-health.js');
const RPLidarDriverParserInfo = require('./rplidar-driver-parser-info.js');
const RPLidarDriverParserExpressScan = require('./rplidar-driver-parser-express-scan.js');

/*
	emmits debug, health, info, scan
*/

class RPLidarDriverResponse extends EventEmitter {

	consructor(serialPort) {
		super();
		this.port = serialPort;
		this.unsetParser();
		this.port.on('dataAvailable', () => {
			if (this.currentParser == undefined) return;
			this.currentParser.parse();
		});
		this.emit('debug', 'RPLidarDriverResponse constructed');
	}

	unsetParser() {
		this.currentParser = undefined;
	}

	setHealthParser() {
		this.currentParser = new RPLidarDriverParserHealth(this, this.port);
	}

	setInfoParser() {
		this.currentParser = new RPLidarDriverParserInfo(this, this.port);
	}

	setExpressScanParser() {
		this.currentParser = new RPLidarDriverParserExpressScan(this, this.port);
	}
}

module.exports = RPLidarDriverResponse;