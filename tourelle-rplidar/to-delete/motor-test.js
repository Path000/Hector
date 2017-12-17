const modulePigpio = require('pigpio');

Gpio = modulePigpio.Gpio;

const motorPWM = new Gpio(18);
motorPWM.mode(Gpio.OUTPUT);

// duty = 1 000 000*60/100; -> 600 000
motorPWM.hardwarePwmWrite(25000, 600000);

const again = () => {
	setTimeout(again, 5000);
}

again();

const exitHandler = (options, err) => {
	exitFunction(options, err);
}

const exitFunction = (options, err) => {
	//clean all
	if (motorPWM) motorPWM.hardwarePwmWrite(0, 0);
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