//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__ESP8266_HARDSERIAL_POINT

#include <RemoteXY.h>

// RemoteXY connection settings 
#define REMOTEXY_SERIAL Serial
#define REMOTEXY_SERIAL_SPEED 115200
#define REMOTEXY_WIFI_SSID "RemoteXY"
#define REMOTEXY_WIFI_PASSWORD "12345678"
#define REMOTEXY_SERVER_PORT 6377


// RemoteXY configurate  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =   // 50 bytes
  { 255,4,0,0,0,43,0,16,31,0,5,0,62,20,30,30,2,26,31,2,
  1,39,5,22,11,2,26,31,31,70,79,82,87,65,82,68,0,66,65,67,
  75,0,4,0,15,26,7,18,2,26 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  int8_t joystick_1_x; // from -100 to 100  
  int8_t joystick_1_y; // from -100 to 100  
  uint8_t switch_1; // =1 if switch ON and =0 if OFF 
  int8_t slider_1; // =0..100 slider position 

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////



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
  RemoteXY_Init (); 
  pinMode(leftMotor1Pin1, OUTPUT);
  pinMode(leftMotor1Pin2, OUTPUT);
  pinMode(leftMotor2Pin1, OUTPUT);
  pinMode(leftMotor2Pin2, OUTPUT);
  pinMode(rightMotor1Pin1, OUTPUT);
  pinMode(rightMotor1Pin2, OUTPUT);
  pinMode(rightMotor2Pin1, OUTPUT);
  pinMode(rightMotor2Pin2, OUTPUT);
  pinMode(motorSpeed, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  //Serial.begin(9600);
}

void loop() {
  RemoteXY_Handler ();
  int active = 0;
  active = (RemoteXY.switch_1==0)?2:1;
  int speed = 0;
  speed = RemoteXY.slider_1;
  int carSpeed = speed*2;
  analogWrite(motorSpeed, carSpeed);
  carMove(active);
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
