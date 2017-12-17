const RPLidarDriverSerialport = require('./rplidar-driver-serialport.js');
const RPLidarDriverMotor = require('./rplidar-driver-motor.js')
const RPLidarDriverRequest = require('./rplidar-driver-request.js')
const RPLidarDriverResponse = require('./rplidar-driver-response.js')

const lidarPort = new RPLidarDriverSerialport('/dev/ttyAMA0');
const lidarMotor = new RPLidarDriverMotor(18);
const lidarResponse = new RPLidarDriverResponse(lidarPort);
const lidarRequest = new RPLidarDriverRequest(lidarPort, lidarResponse);

lidarPort.on('error', (err) => {
	exitFunction({
		exit: true
	}, err);
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

lidarPort.open(() => {
	setTimeout(() => {
		lidarRequest.getLidarHealth((err) => {
			console.log('health asked finished');
			console.log(err);
			exitFunction({
				exit: true
			}, err);
		});
	}, 10);
});

lidarResponse.on('debug', (data) => {
	console.log(data);
});

lidarResponse.on('health', (data) => {
	console.log(data);
	if (data.statusCode != 0) {
		console.log('Health is not good. Stopping ...');
		exitFunction({
			exit: true
		});
	}
	setTimeout(() => {
		lidarRequest.getLidarInfo((err) => {
			exitFunction({
				exit: true
			}, err);
		});
	}, 10);
});

lidarResponse.on('info', (data) => {
	console.log(data);

	setTimeout(() => {
		lidarMotor.start(60);

		setTimeout(() => {
			lidarRequest.startExpressScan((err) => {
				exitFunction({
					exit: true
				}, err);
			});
		}, 3000);

	}, 10);
});

lidarResponse.on('scan', (data) => {
	console.log(data);
});

const exitHandler = (options, err) => {
	exitFunction(options, err);
}

const exitFunction = (options, err) => {
	//clean all
	if (lidarMotor) lidarMotor.stop();
	if (lidarRequest) lidarRequest.stopScan();
	if (lidarPort) lidarPort.close();
	if (options.cleanup) console.log('clean');
	if (err) console.log(err.stack);
	if (options.exit) process.exit();

}

//do something when app is closing
process.on('exit', exitHandler.bind(null, {
	cleanup: true
}));
//catches ctrl+c event
process.on('SIGINT', exitHandler.bind(null, {
	exit: true
}));
//catches uncaught exceptions
process.on('uncaughtException', exitHandler.bind(null, {
	exit: true
}));