const RPLidarDriverSerialport = require('./rplidar-driver-serialport.js');
const RPLidarDriverMotor = require('./rplidar-driver-motor.js')
const RPLidarDriverRequest = require('./rplidar-driver-request.js')
const RPLidarDriverResponse = require('./rplidar-driver-response.js')

const lidarPort = new RPLidarDriverSerialport('/dev/ttyAMA0');
const lidarMotor = new RPLidarDriverMotor(18);
const lidarResponse = new RPLidarDriverResponse(lidarPort);
const lidarRequest = new RPLidarDriverRequest(lidarPort, lidarResponse);

lidarPort.on('error', (err) => {
	console.log(err);
	throw err;
});

lidarPort.on('open', () => {
	console.log('Port oppened');
});

lidarPort.on('close', () => {
	console.log('Port closed');
});

lidarRequest.on('debug', (data) => {
	console.log(data);
});

lidarResponse.on('debug', (data) => {
	console.log(data);
});

lidarResponse.on('health', (data) => {
	console.log(data);
	if (data.statusCode != 0) {
		const msg = 'Health is not good. Stopping ...';
		console.log(msg);
		throw new Error(msg);
	}
	setTimeout(() => {
		lidarRequest.getLidarInfo((err) => {
			if (err) {
				console.log(err);
				throw err;
			}
		});
	}, 10);
});

lidarResponse.on('info', (data) => {
	console.log(data);

	setTimeout(() => {
		lidarMotor.start(60);

		setTimeout(() => {
			lidarRequest.startExpressScan((err) => {
				if (err) {
					console.log(err);
					throw err;
				}
			});
		}, 3000);

	}, 10);
});

const getScanEmitter = () => {
	return lidarResponse;
}

const start = () => {
	lidarPort.open(() => {
		setTimeout(() => {
			lidarRequest.getLidarHealth((err) => {
				if (err) exitFunction({
					exit: true
				}, err);
			});
		}, 10);
	});
}

const stop = (next) => {
	//clean all
	if (lidarMotor) lidarMotor.stop();
	if (lidarRequest) lidarRequest.stopScan(() => {
		if (lidarPort) lidarPort.close();
		if (next)next();
	});

}

module.exports.stop = stop;
module.exports.start = start;
module.exports.getScanEmitter = getScanEmitter;