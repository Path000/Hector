#include "Robot.h"

void Robot::init() {

  Serial.begin(115200); // RaspberryPI
  Serial.setTimeout(500);
  while (!Serial); // wait for serial port to connect. Needed for native USB
  _command.init(&Serial);

  _moteurA.init(PIN_DIR_A, PIN_PWM_A, "A");
  _moteurB.init(PIN_DIR_B, PIN_PWM_B, "B");
  _moteurC.init(PIN_DIR_C, PIN_PWM_C, "C");
  _moteurD.init(PIN_DIR_D, PIN_PWM_D, "D");

/*
  // Test moteurs

  digitalWrite(PIN_DIR_A, HIGH);
  analogWrite(PIN_PWM_A, 20);
  delay(500);
  digitalWrite(PIN_DIR_A, LOW);
  delay(500);

  digitalWrite(PIN_DIR_B, HIGH);
  analogWrite(PIN_PWM_B, 20);
  delay(500);
  digitalWrite(PIN_DIR_B, LOW);
  delay(500);

  digitalWrite(PIN_DIR_C, HIGH);
  analogWrite(PIN_PWM_C, 20);
  delay(500);
  digitalWrite(PIN_DIR_C, LOW);
  delay(500);

  digitalWrite(PIN_DIR_D, HIGH);
  analogWrite(PIN_PWM_D, 20);
  delay(500);
  digitalWrite(PIN_DIR_D, LOW);
  delay(500);

  stopMotors();
*/
  _sensor.init();
  
  _ecran.init();
  
  Serial1.begin(115200);
  Serial1.setTimeout(1000);
  _compteur1.init(&Serial1);
  if(!Serial1) {
    Serial.print("Ooops, Serial1 port was not available !");
    while (1);
  }
  
  Serial2.begin(115200);
  Serial2.setTimeout(1000);
  _compteur2.init(&Serial2);
  if(!Serial2) {
    Serial.print("Ooops, Serial2 port was not available !");
    while (1);
  }
}

void Robot::stopMotors() {
  _moteurA.stop();
  _moteurB.stop();
  _moteurC.stop();
  _moteurD.stop();
}

// STOP :            MOVE:-1:0
// rotate :          MOVE:-1:-1
// rotate :          MOVE:-1:1
// strafe forward :  MOVE:0:0
// strafe left :     MOVE:90:0
// starfe + rotate : MOVE:0:1

void Robot::computeMove(int strafeDirection, int rotationDirection) { // TODO : put that computation in raspberry

  double wA = 0.0;
  double wB = 0.0;
  double wC = 0.0;
  double wD = 0.0;

  // cmd arg rotation : direction (int) -1 | 0 | 1
  double angularSpeed = ROTATION_COEF * (double)rotationDirection * (double)(DEMI_LARGEUR + DEMI_LONGUEUR);

  wA = angularSpeed;
  wB = angularSpeed;
  wC = angularSpeed;
  wD = angularSpeed;

  // cmd arg strafe : (int) angle en degrés -1 means no strafe
  if(strafeDirection >= 0) {
    double strafeX = STRAFE_COEF * cos(strafeDirection * (double)PI / (double)180);
    double strafeY = STRAFE_COEF * sin(strafeDirection * (double)PI / (double)180);
    wA = -strafeY + strafeX + wA;
    wB = strafeY + strafeX + wB;
    wC = strafeY - strafeX + wC;
    wD = -strafeY - strafeX + wD;
  }

  _moteurA.setCommand((int)wA);
  _moteurB.setCommand((int)wB);
  _moteurC.setCommand((int)wC);
  _moteurD.setCommand((int)wD);
}

void Robot::setPID(float Kp, float Ki, float Kd) {
  _moteurA.setPID(Kp, Ki, Kd);
  _moteurB.setPID(Kp, Ki, Kd);
  _moteurC.setPID(Kp, Ki, Kd);
  _moteurD.setPID(Kp, Ki, Kd);
}
