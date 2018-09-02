const StepperMotors = require('./StepperMotors.js')

const done = () => {
	console.log("done");
}

const gpioPinNumberPulse = 17;
const gpioPinNumberDirA = 27;
const gpioPinNumberDirB = 22;

const steppers = new StepperMotors(gpioPinNumberPulse, gpioPinNumberDirA, gpioPinNumberDirB, done, 200);

steppers.forward(500);