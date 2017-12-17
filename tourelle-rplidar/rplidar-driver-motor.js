const modulePigpio = require('pigpio');

class RPLidarDriverMotor {

	constructor(gpioPin) {
		const Gpio = modulePigpio.Gpio;
		this.motorPWM = new Gpio(gpioPin);
		this.motorPWM.mode(Gpio.OUTPUT);
	}

	start(motorPWMDuty) {
		const duty = 1000000 * motorPWMDuty / 100;
		this.motorPWM.hardwarePwmWrite(25000, duty);
	}

	stop() {
		this.motorPWM.hardwarePwmWrite(0, 0);
	}
}
module.exports = RPLidarDriverMotor;