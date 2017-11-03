
// Serial message parser
#define SEPARATOR ':'
#define MAX_ARGS 5

// PINS
#define PIN_ENCA_CHANA 2 // interrupt 0
#define PIN_ENCB_CHANA 3 // interrupt 1
#define PIN_ENCA_CHANB 4 // hardcoded in interrupt
#define PIN_ENCB_CHANB 5 // hardcoded in interrupt
#define PIN_DIRA 8
#define PIN_DIRB 9
#define PIN_PWMA 10
#define PIN_PWMB 11

int state;

#define STATE_STOPED 1
#define STATE_MOVING 2

// Directions
#define MOVE_FORWARD 1
#define MOVE_BACKWARD 2
#define TURN_LEFT 3
#define TURN_RIGHT 4


#define SAMPLE_DELAY 100 // ms

// varaibles de calcul de mouvement
long instantSpeedA = 0L;
long instantSpeedB = 0L;
long distanceTraveledA = 0L;
long distanceTraveledB = 0L;
unsigned long lastSampleTime = 0L;

// Compteurs des roues codeuses
volatile long counterA = 0L;
volatile long counterB = 0L;

// paramètres donnés
long distanceToTravel = 0L;
float acceleration = 0;
byte maxSpeed = 0;
byte minSpeed = 0;

/*
    Wiring
    Pololu - 100:1 Metal Gearmotor 37Dx73L mm with 64 CPR Encoder
    https://www.pololu.com/product/2826

    Red : motor power (connects to one motor terminal)
    Black : motor power (connects to the other motor terminal)
    Green : encoder GND
    Blue : encoder Vcc (3.5 – 20 V)
    Yellow : encoder A output
    White : encoder B output
*/

/*
   Données :
   6533 ticks par tour.
   Périmètre des roues : 219.91 mm
   0,033661638719009 mm/tick
   arduino long : (4 bytes), from -2,147,483,648 to 2,147,483,647.
   soit 63796170618,018622864075955 mm max en comptant les ticks
*/

// moteur gauche
// pin channel A : 2 (interrupt)
// pin channel B : 4
void interruptOnRaisingA() {
  (PIND & B00010000) ? counterA++ : counterA--;
}
// moteur droit
// pin channel A : 3 (interrupt)
// pin channel B : 7
void interruptOnRaisingB() {
  (PIND & B00100000) ? counterB-- : counterB++;
}


/*
   SETUP
*/
void setup() {
  Serial.begin(115200);
  Serial.setTimeout(1000);

  pinMode(PIN_ENCA_CHANA, INPUT);
  pinMode(PIN_ENCB_CHANA, INPUT);
  pinMode(PIN_ENCA_CHANB, INPUT);
  pinMode(PIN_ENCB_CHANB, INPUT);
  pinMode(PIN_DIRA, OUTPUT);
  pinMode(PIN_DIRB, OUTPUT);
  pinMode(PIN_PWMA, OUTPUT);
  pinMode(PIN_PWMB, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(PIN_ENCA_CHANA), interruptOnRaisingA, RISING);
  attachInterrupt(digitalPinToInterrupt(PIN_ENCB_CHANA), interruptOnRaisingB, RISING);

  doMotorStop();

  // L'ouverture du port USB par le PI reset l'arduino.
  // On prévient quand on est up à nouveau.
  Serial.println("READY");
}

void doMotorStop() {
  state = STATE_STOPED;
  analogWrite(PIN_PWMA, 0);
  analogWrite(PIN_PWMB, 0);
}

void updateMotorControl() {

  // controle de la vitesse A :
  // accélération au démarrage.
  // décélération à l'arrivée.
  // les 2 moteurs doivent aller à la même vitesse.
  // => La vitesse de B est ajustée fonction de la différence entre A et B

  if (distanceTraveledA >= distanceToTravel) {
    // On est arrivé
    Serial.print("FINISHED");
    Serial.print(SEPARATOR);
    Serial.print(distanceTraveledA); // donner la distance réelle parcourue.
    Serial.print(SEPARATOR);
    Serial.print(distanceTraveledB); // donner la distance réelle parcourue.
    Serial.println("");
    doMotorStop();
    return;
  }

  // On construit une courbe en triangle pointe en haut à mi-parcours.
  long theoricalControl = 0L;
  if(distanceTraveledA < distanceToTravel / 2) {
    // On accélère jusqu'à mi parcours
    theoricalControl = distanceTraveledA * acceleration;
  }
  else {
    // on décélère à mi parcours
    theoricalControl = (distanceToTravel - distanceTraveledA) * acceleration;
  }

  // On écrête A
  byte controlA = constrain(theoricalControl, minSpeed, maxSpeed);
  // Contrôle de B proportionnel
  // setPoint : instantSpeedA
  // input : instantSpeedB
  long errorB = instantSpeedA - instantSpeedB;
  // Contrôle de B intégral
  long errorCumulB = distanceTraveledA - distanceTraveledB;
  // rapport constaté sur le moteur B entre pwm et ticks
  int theoricalControlB = controlA + int((errorB + errorCumulB) * 0.9);
  // On écrête aussi B
  byte controlB = constrain(theoricalControlB, 0, 255);
 
  analogWrite(PIN_PWMA, controlA);
  analogWrite(PIN_PWMB, controlB);

/*
  Serial.print(distanceTraveledA, DEC);
  Serial.print(", ");
  Serial.print(distanceTraveledB, DEC);
  Serial.print(", ");
  Serial.print(errorB, DEC);
  Serial.print(", ");
  Serial.print(errorCumulB, DEC);
  Serial.print(", ");
  Serial.print(controlA, DEC);
  Serial.print(", ");
  Serial.print(controlB, DEC);
  Serial.println();
*/
}


/*
MOVE:1:6000:0.09:15:80
*/

/*
  Format : COMMAND:value[:value[:value[...]]]
  Command MOVE:dir:dist:acceleration:minSpeed:maxSpeed
  Où dir : (avance, gauche, recule, droite)
  dist : ticks
  acceleration : pente accélérarion et décération
  minSpeed (0-255)
  maxSpeed (0-255)
*/

void parseAndDispatch(String dataFromPI) {
  // get Command
  int dataLength = dataFromPI.length();
  int firstSep = dataFromPI.indexOf(SEPARATOR);
  if (firstSep + 1 >= dataLength) return;
  String cmd = "";
  String arrayArgs[MAX_ARGS];
  if (firstSep == -1) { // pas de param
    cmd = dataFromPI;
  }
  else {
    cmd = dataFromPI.substring(0, firstSep);
    // get args
    int index = 0;
    while (firstSep + 1 < dataLength && index < MAX_ARGS) {
      int secondSep = dataFromPI.indexOf(SEPARATOR, firstSep + 1);
      // pour le dernier argument
      if (secondSep == -1) {
        secondSep = dataLength;
      }
      String arg = dataFromPI.substring(firstSep + 1, secondSep);
      firstSep = secondSep;
      arrayArgs[index] = arg;
      index++;
    }
  }
  if (cmd == "MOVE") {
    state = STATE_MOVING;
    counterA = 0L;
    counterB = 0L;
    distanceTraveledA = 0L;
    distanceTraveledB = 0L;
    lastSampleTime = 0L;
    distanceToTravel = arrayArgs[1].toInt();
    acceleration = arrayArgs[2].toFloat();
    minSpeed = arrayArgs[3].toInt();
    maxSpeed = arrayArgs[4].toInt();
    int direction = arrayArgs[0].toInt();
    switch (direction) {
      case MOVE_FORWARD :
        digitalWrite(PIN_DIRA, LOW);
        digitalWrite(PIN_DIRB, LOW);
        break;
      case MOVE_BACKWARD :
        digitalWrite(PIN_DIRA, HIGH);
        digitalWrite(PIN_DIRB, HIGH);
        break;
      case TURN_LEFT :
        digitalWrite(PIN_DIRA, LOW);
        digitalWrite(PIN_DIRB, HIGH);
        break;
      case TURN_RIGHT :
        digitalWrite(PIN_DIRA, HIGH);
        digitalWrite(PIN_DIRB, LOW);
        break;
    }
  }
  if (cmd == "STOP") {
    doMotorStop();
  }
  if (cmd == "HELLO") {
    Serial.println("PROPULSION");
  }
}

/*
   LOOP
*/
void loop() {
  if (Serial.available() > 0) {
    parseAndDispatch(Serial.readStringUntil('\n'));
  }

  switch (state) {

    case STATE_STOPED :
      // rien à faire
      break;

    case STATE_MOVING :
      unsigned long now = millis();
      unsigned long deltaTime = now - lastSampleTime;
      if(deltaTime >= SAMPLE_DELAY) {
        lastSampleTime = now;
        distanceTraveledA += abs(counterA);
        distanceTraveledB += abs(counterB);
        instantSpeedA = abs(counterA);
        instantSpeedB = abs(counterB);
        counterA = 0L;
        counterB = 0L;
        updateMotorControl();
      }
      break;
  }
}
