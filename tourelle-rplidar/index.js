const RPLidarDriver = require('./rplidar-driver.js');

const lidar = new RPLidarDriver('/dev/ttyAMA0', 60);

lidar.on('health', (data) => {
	console.log(data);
	if (data.statusCode != 0) {
		console.log('Health is not good. Stopping ...');
		exitFunction({
			exit: true
		});
	}
	setTimeout(() => {
		lidar.getLidarInfo(() => {});
	}, 10);
});

lidar.on('info', (data) => {
	console.log(data);
	setTimeout(() => {
		lidar.startMotor(() => {
			setTimeout(() => {
				lidar.startExpressScan(() => {});
			}, 3000)
		});

	}, 10);
});

lidar.on('error', (data) => {
	console.log(data);
});

lidar.on('open', () => {
	console.log('Port oppened');
});

lidar.on('close', () => {
	console.log('Port closed');
});

lidar.on('ready', () => {
	console.log('Driver ready');
});

lidar.init(() => {
	setTimeout(() => {
		lidar.getLidarHealth(() => {});
	}, 10);
});

const exitHandler = (options, err) => {
	exitFunction(options, err);
}

const exitFunction = (options, err) => {
	//clean all
	if (lidar) {
		lidar.stopMotor(() => {});
		lidar.stopScan(() => {});
	}
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