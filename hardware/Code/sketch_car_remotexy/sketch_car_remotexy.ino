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
uint8_t RemoteXY_CONF[] =   // 120 bytes
  { 255,11,0,11,0,113,0,16,31,0,7,53,10,20,20,5,2,26,2,1,
  0,64,14,12,12,2,31,70,0,1,0,64,42,12,12,2,31,66,0,1,
  0,49,29,12,12,2,31,76,0,1,0,77,29,12,12,2,31,82,0,1,
  0,21,6,12,12,2,31,82,82,0,1,0,6,6,12,12,2,31,82,76,
  0,3,5,17,35,8,22,2,26,1,0,49,11,12,12,2,31,70,76,0,
  1,0,78,11,12,12,2,31,70,82,0,67,4,10,27,20,5,2,26,11 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  int16_t edit_speed;  // 32767.. +32767 
  uint8_t btn_forward; // =1 if button pressed, else =0 
  uint8_t btn_back; // =1 if button pressed, else =0 
  uint8_t btn_left; // =1 if button pressed, else =0 
  uint8_t btn_right; // =1 if button pressed, else =0 
  uint8_t btn_rotate_right; // =1 if button pressed, else =0 
  uint8_t btn_rotate_left; // =1 if button pressed, else =0 
  uint8_t select_speed; // =0 if select position A, =1 if position B, =2 if position C, ... 
  uint8_t btn_front_left; // =1 if button pressed, else =0 
  uint8_t btn_front_right; // =1 if button pressed, else =0 

    // output variables
  char txt_speed[11];  // string UTF8 end zero 

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
  int speed = 0;
  if(RemoteXY.connect_flag == 1){
    if(RemoteXY.btn_forward==1){
      active = 1;
    }else if(RemoteXY.btn_back==1){
      active = 2;
    }else if(RemoteXY.btn_left==1){
      active = 3;
    }else if(RemoteXY.btn_right==1){
      active = 4;
    }else if(RemoteXY.btn_rotate_right==1){
      active = 5;
    }else if(RemoteXY.btn_rotate_left==1){
      active = 6;
    }else if(RemoteXY.btn_front_left==1){
      active = 7;
    }else if(RemoteXY.btn_front_right==1){
      active = 8;
    }
    speed = RemoteXY.select_speed*60;
    sprintf(RemoteXY.txt_speed, "%d", speed);
  }else{
    active = 0;
    speed = 0;
  }
  
  analogWrite(motorSpeed, speed);
  carMove(active);
}


// 0: stop 1: Forward 2: Backward 3: Left 4: Right 5: Rotate Right 6 : Rotate Left 7: front left 8: front right
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
  }else if(active == 3){
    tiresMove(1, 2);
    tiresMove(2, 2);
    tiresMove(3, 1);
    tiresMove(4, 1);
  }else if(active == 4){
    tiresMove(1, 1);
    tiresMove(2, 1);
    tiresMove(3, 2);
    tiresMove(4, 2);
  }else if(active == 5){
    tiresMove(1, 1);
    tiresMove(2, 2);
    tiresMove(3, 1);
    tiresMove(4, 2);
  }else if(active == 6){
    tiresMove(1, 2);
    tiresMove(2, 1);
    tiresMove(3, 2);
    tiresMove(4, 1);
  }else if(active == 7){
    tiresMove(1, 2);
    tiresMove(2, 0);
    tiresMove(3, 0);
    tiresMove(4, 1);
  }else if(active == 8){
    tiresMove(1, 1);
    tiresMove(2, 0);
    tiresMove(3, 0);
    tiresMove(4, 2);
  }
}

// tire 1: Left Front 2: Right Rear 3: Left Rear 4: Right Front  direction 0: stop  1: forward 2: backward
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
