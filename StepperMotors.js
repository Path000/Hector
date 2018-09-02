const Gpio = require('pigpio')
	.Gpio;

/*
	This class drives the 2 stepper motors.
	Both share the same pulse pin.
	Each one have their own direction pin

	Doc : https://github.com/fivdi/pigpio/blob/master/doc/gpio.md
*/

class StepperMotors {

	constructor(gpioPinNumberPulse, gpioPinNumberDirA, gpioPinNumberDirB, stepsDoneCB, stepsPerRevolution) {
		this._gpioPulse = new Gpio(gpioPinNumberPulse, {
			mode: Gpio.OUTPUT
		});
		this._gpioDirA = new Gpio(gpioPinNumberDirA, {
			mode: Gpio.OUTPUT
		});
		this._gpioDirB = new Gpio(gpioPinNumberDirB, {
			mode: Gpio.OUTPUT
		});

		/*
			full step : 200 / revolution
			1/16 step : 3200 / revolution
		*/

		this._stepsPerRevolution = stepsPerRevolution;
		this._maxPeriod = 10000 / stepsPerRevolution; // 1 rev in 10s
		this._minPeriod = 5000 / stepsPerRevolution; // 1 rev in 5s

		// dela t between min and max period for 2 revolutions
		// constant accel in 2 revolutions
		this._deltaT = (this._maxPeriod - this._minPeriod) / (2 * stepsPerRevolution);

		console.log("stepsPerRevolution : " + this._stepsPerRevolution);
		console.log("_maxPeriod : " + this._maxPeriod);
		console.log("_minPeriod : " + this._minPeriod);
		console.log("_deltaT : " + this._deltaT);

		this._stepToGo = 0;
		this._moveTimeoutHandler = null;
		this._stepPeriod = this._maxPeriod;
		this._stepsDoneCB = stepsDoneCB;

		this.stop();
	}

	forward(steps) {
		this._gpioDirA = 1;
		this._gpioDirB = 0;
		this._start(steps);
	}

	backward(steps) {
		this._gpioDirA = 0;
		this._gpioDirB = 1;
		this._start(steps);
	}

	left(steps) {
		this._gpioDirA = 1;
		this._gpioDirB = 1;
		this._start(steps);
	}

	right(steps) {
		this._gpioDirA = 0;
		this._gpioDirB = 0;
		this._start(steps);
	}

	_start(steps) {
		if (steps <= 0) return;
		this._stepToGo = steps;
		this._moveAStep();
	}

	_moveAStep() {
		if (this._stepToGo <= 0) {
			this.stop();
			this._stepsDoneCB();
			return;
		}

		console.log("stepToGo:" + this._stepToGo + " stepPeriod:" + this._stepPeriod);

		// pulseLen (micro seconds) should be < pulse period => period > 100us
		// trigger level to 1
		this._gpioPulse.trigger(100, 1);
		this._stepToGo--;
		this._moveTimeoutHandler = setTimeout(() => {
			this._moveAStep();
		}, this._stepPeriod);

		// compute period accell
		// at 2 revolution from the end
		if (this._stepToGo < this._stepsPerRevolution * 2) {
			// decrease speed at the end minus 2 revolutions
			if (this._stepPeriod < this._maxPeriod) {
				this._stepPeriod += this._deltaT;
			}
		} else {
			// increase speed until max
			if (this._stepPeriod > this._minPeriod) {
				this._stepPeriod -= this._deltaT;
			}
		}
	}

	stop() {
		if (this._moveTimeoutHandler != null) {
			clearTimeout(this._moveTimeoutHandler);
			this._moveTimeoutHandler = null;
		}
		this._gpioPulse.digitalWrite(0);
		this._stepToGo = 0;
		this._stepPeriod = this._maxPeriod;
	}
}
module.exports = StepperMotors;