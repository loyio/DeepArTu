#include <stdarg.h>

const char* ssid = "CMCC_Inequality";
const char* password = "Chebyshev";
Serial.println("AT+CWJAP=\"CMCC_Inequality\",\"Chebyshev\"");  //connect wifi

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  Serial.
}
void rx_empty(void) 
{
    while(Serial.available() > 0) {
        Serial.read();
    }
}
uint32_t recv(uint8_t *buffer, uint32_t buffer_size, uint32_t timeout)
{
    return recvPkg(buffer, buffer_size, NULL, timeout, NULL);
}
uint32_t recvPkg(uint8_t *buffer, uint32_t buffer_size, uint32_t *data_len, uint32_t timeout, uint8_t *coming_mux_id)
{
    String data;
    char a;
    int32_t index_PIPDcomma = -1;
    int32_t index_colon = -1; /* : */
    int32_t index_comma = -1; /* , */
    int32_t len = -1;
    int8_t id = -1;
    bool has_data = false;
    uint32_t ret;
    unsigned long start;
    uint32_t i;
    
    if (buffer == NULL) {
        return 0;
    }
    
    start = millis();
    while (millis() - start < timeout) {
        if(Serial.available() > 0) {
            a = Serial.read();
            data += a;
        }
        
        index_PIPDcomma = data.indexOf("+IPD,");
        if (index_PIPDcomma != -1) {
            index_colon = data.indexOf(':', index_PIPDcomma + 5);
            if (index_colon != -1) {
                index_comma = data.indexOf(',', index_PIPDcomma + 5);
                /* +IPD,id,len:data */
                if (index_comma != -1 && index_comma < index_colon) { 
                    id = data.substring(index_PIPDcomma + 5, index_comma).toInt();
                    if (id < 0 || id > 4) {
                        return 0;
                    }
                    len = data.substring(index_comma + 1, index_colon).toInt();
                    if (len <= 0) {
                        return 0;
                    }
                } else { /* +IPD,len:data */
                    len = data.substring(index_PIPDcomma + 5, index_colon).toInt();
                    if (len <= 0) {
                        return 0;
                    }
                }
                has_data = true;
                break;
            }
        }
    }
    
    if (has_data) {
        i = 0;
        ret = len > buffer_size ? buffer_size : len;
        start = millis();
        while (millis() - start < 2000) {
            while(Serial.available() > 0 && i < ret) {
                a = Serial.read();
                buffer[i++] = a;
            }
            if (i == ret) {
                rx_empty();
                if (data_len) {
                    *data_len = len;    
                }
                if (index_comma != -1 && coming_mux_id) {
                    *coming_mux_id = id;
                }
                return ret;
            }
        }
    }
    return 0;
}

void loop() {
  // put your main code here, to run repeatedly:
    String control;
    uint8_t buffer[128] = {0};
    uint32_t len = recv(buffer,sizeof(buffer),10000);
    if (len > 0) {
        for(uint32_t i = 0; i < len; i++) {
            control += (char)buffer[i];
        }
        if(control == "up")
          digitalWrite(LED_BUILTIN, HIGH);
        else if(control = "down")
          digitalWrite(LED_BUILTIN, LOW);
    }
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