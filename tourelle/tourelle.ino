#include "Stop.h"
#include "MyStepper.h"
#include "Sensors.h"

#define STATE_FORWARD 1
#define STATE_BACKWARD 2
#define STATE_STOP 3

#define SEPARATOR ":"

#define STEP_SIZE 10

int state;
bool doReadRange;
long position;

MyStepper stepper;
Sensors sensors;
Stop stopSensor;

void printPositionAndRanges() {
  uint16_t rangeA = sensors.readRangeA();
  uint16_t rangeB = sensors.readRangeB();
  uint16_t rangeC = sensors.readRangeC();
  uint16_t rangeD = sensors.readRangeD();
  Serial.print("MEAS");
  Serial.print(SEPARATOR);
  Serial.print(position), DEC;
  Serial.print(SEPARATOR);
  Serial.print(rangeA, DEC);
  Serial.print(SEPARATOR);
  Serial.print(rangeB, DEC);
  Serial.print(SEPARATOR);
  Serial.print(rangeC, DEC);
  Serial.print(SEPARATOR);
  Serial.print(rangeD, DEC);
  Serial.println("");
  Serial.flush();
}

/*
   SETUP
*/
void setup() {
  Serial.begin(115200);
  Serial.setTimeout(1000);

  state = STATE_STOP;
  doReadRange = false;
  stepper.init();
  sensors.init();
  // L'ouverture du port USB par le PI reset l'arduino.
  // On prévient quand on est up à nouveau.
  Serial.println("READY");
}

/*
   LOOP
*/
void loop() {
  // Read command

  String command = "";
  if (Serial.available() > 0)  {

    command = Serial.readStringUntil('\n');

    if (command == "HELLO") {
      Serial.println("TOURELLE");
    }

    if (command == "START") {
      state = STATE_BACKWARD;
    }

    if (command == "STOP") {
      state = STATE_STOP;
    }
  }

  // Action

  switch (state) {

    case STATE_STOP :

      doReadRange = false; // Pas de lecture des distances lors de la reprise en backward
      break;

    case STATE_BACKWARD : // recule jusqu'à la butée.

      if (stopSensor.isReached()) { // Si en butée
        position = 0;
        doReadRange = true;
        state = STATE_FORWARD;
        break;
      }

      stepper.move(-1 * STEP_SIZE);
      position -= STEP_SIZE;
      if (doReadRange) { // Pas de lecture au démarrage lors de la recherche de la butée
        printPositionAndRanges();
      }
      break;

    case STATE_FORWARD : // On avance de 90° (ou 800 pas)

      stepper.move(STEP_SIZE);
      position += STEP_SIZE;
      printPositionAndRanges();

      if (position >= 800) { // Détection arrivée en fin de course
        position = 800;
        state = STATE_BACKWARD;
      }
      break;
  }
}



