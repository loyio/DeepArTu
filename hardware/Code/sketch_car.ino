

int leftMotor1Pin1 = 2;
int leftMotor1Pin2 = 3;
int leftMotor2Pin1 = 4;
int leftMotor2Pin2 = 5;
int rightMotor1Pin1 = 6;
int rightMotor1Pin2 = 7;
int rightMotor2Pin1 = 8;
int rightMotor2Pin2 = 9;
int motorSpeed = 10;

void setup() {
  PinMode(leftMotor1Pin1, OUTPUT);
  PinMode(leftMotor1Pin2, OUTPUT);
  PinMode(leftMotor2Pin1, OUTPUT);
  PinMode(leftMotor2Pin2, OUTPUT);
  PinMode(rightMotor1Pin1, OUTPUT);
  PinMode(rightMotor1Pin2, OUTPUT);
  PinMode(rightMotor2Pin1, OUTPUT);
  PinMode(rightMotor2Pin2, OUTPUT);
  PinMode(motorSpeed, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  analogWrite(motorSpeed, 100);
  carMove(2);
}


// 0: stop 1: Forward 2: Backward
void carMove(int active){
  if(active == 0){
    for(int i = 1; i <= 4; i++){
      tiresMove(i, 0);
    }
  }else if(active == 1){
    for(int i = 1; i <= 4; i++){
      tiresMove(i, 1);
    }
  }else if(active == 2){
    for(int i = 1; i <= 4; i++){
      tiresMove(i, 2);
    }
  }
}

// tire 1: Left Front 2: Right Front 3: Left Rear 4: Right Rear  direction 0: stop  1: forward 2: backward
void tiresMove(int tire, int direction){
  switch (tire){  
  case 1:
    tireState(leftMotor1Pin1, leftMotor1Pin2, direction);
    break;
  case 2:
    tireState(rightMotor1Pin1, rightMotor1Pin2, direction);
    break;
  case 3:
    tireState(leftMotor2Pin1, leftMotor2Pin2, direction);
    break;
  case 4:
    tireState(rightMotor2Pin1, rightMotor2Pin2, direction);
    break;
  default:
    break;
  }
}

// 0: stop  1: forward 2: backward
void tireState(int pin1, int pin2, int state){
  switch (state){
  case 0:
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
    break;
  case 1:
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, LOW);
    break;
  case 2:
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, HIGH);
    break;
  default:
    break;
  }
}

