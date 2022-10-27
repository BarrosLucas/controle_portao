#include <Ultrasonic.h>
 
#define pin_trigger 9
#define pin_echo 10

#define pin_button 2

Serial mySerial; //Funcionalidade serial
PrintWriter output; //Funcionalidade de escrita em arquivos

int enB = 3;
int in3 = 5;
int in4 = 4;

float distance_close= 40;   //5
float distance_1    = 27;  //15
float distance_2    = 18;  //25
float distance_open = 12;  //35

int speed_very_fast = 80;
int speed_fast      = 80; //250
int speed_medium    = 80; //150
int speed_slow      = 80; //100
int speed_very_slow = 80;  //70

enum states{
  OPEN,             //0
  CLOSE,            //1
  OPENING_1,        //2
  OPENING_2,        //3
  OPENING_FINISH,   //4
  CLOSING_1,        //5
  CLOSING_2,        //6
  CLOSING_FINISH,   //7
  TO_OPEN_1,        //8
  OPEN_1,           //9
  TO_OPEN_2,        //10
  OPEN_2,           //11
  TO_CLOSE_1,       //12
  CLOSE_1,          //13
  TO_CLOSE_2,       //14
  CLOSE_2           //15
};                           

int current_state = OPEN;

Ultrasonic ultrasonic(pin_trigger, pin_echo);
 
void setup(){
  Serial.begin(9600);

  String portName = Serial.list()[0]; //configura a porta serial
  mySerial = new Serial(this, portName, 9600);


  /**
    RISING botao: 0 -> 1
    FALLING botao: 1 -> 0
  */
  attachInterrupt(digitalPinToInterrupt(pin_button), changeState, RISING);
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}


//Forward:  opening
//Backward: closing
void moving_gate(){
  switch(current_state){
      case OPENING_1:
        moveToForward(speed_fast);
        if(getGateDistance() <= distance_1){
          current_state = OPENING_2;
        }
        break;
      case OPENING_2:
        moveToForward(speed_medium);
        if(getGateDistance() <= distance_2){
          current_state = OPENING_FINISH;
        }
        break;
      case OPENING_FINISH:
        moveToForward(speed_slow);
        if(getGateDistance() <= distance_open){
          current_state = OPEN;
        }
        break;
      case CLOSING_1:
        moveToBackward(speed_fast);
        if(getGateDistance() >= distance_2){
          current_state = CLOSING_2;
        }
        break;
      case CLOSING_2:
        moveToBackward(speed_medium);
        if(getGateDistance() >= distance_1){
          current_state = CLOSING_FINISH;
        }
        break;
       case CLOSING_FINISH: 
        moveToBackward(speed_slow);
        if(getGateDistance() >= distance_close){
          current_state = CLOSE;
        }
        break;
      case TO_OPEN_1:
        moveToForward(speed_very_slow);
        if(getGateDistance() <= distance_1){
          current_state = OPEN_1;
        }
        break;
      case TO_OPEN_2:
        moveToForward(speed_very_slow);
        if(getGateDistance() <= distance_2){
          current_state = OPEN_2;
        }
        break;
      case TO_CLOSE_1:
        moveToBackward(speed_very_slow);
        if(getGateDistance() >= distance_2){
          current_state = CLOSE_1;
        }
        break;
      case TO_CLOSE_2:
        moveToBackward(speed_very_slow);
        if(getGateDistance() >= distance_1){
          current_state = CLOSE_2;
        }
        break;
  }
}

/**
  Função ativada pela interrupção do botão
*/
void changeState() {
  Serial.println("Button pressed!");
  switch(current_state){
      case OPEN:
        current_state = CLOSING_1;
        break;
      case CLOSE:
        current_state = OPENING_1;
        break;
      case OPENING_1:
        current_state = TO_OPEN_1;
        break;
      case OPENING_2:
        current_state = TO_OPEN_2;
        break;
      case CLOSING_1:
        current_state = TO_CLOSE_1;
        break;
      case CLOSING_2:
        current_state = TO_CLOSE_2;
        break;
      case OPEN_1:
        impulseMotorForce(0);
        current_state = CLOSING_FINISH;
        break;
      case OPEN_2:
        impulseMotorForce(0);
        current_state = CLOSING_2;
        break;
      case CLOSE_1:
        impulseMotorForce(1);
        current_state = OPENING_FINISH;
        break;
      case CLOSE_2:
        impulseMotorForce(1);
        current_state = OPENING_2;
        break;
      default:
        break;
    }
}
 
void loop(){
  Serial.println(getGateDistance()*10);
  delay(20);
  //Serial.println("State before moving: ");
  //Serial.println(current_state);

  //Serial.println("State after set states: ");
  //Serial.println(current_state);
  moving_gate();
  //Serial.println("State after moving: ");
  //Serial.println(current_state);
}


void moveToBackward(int speed_motor){
  analogWrite(enB, speed_motor);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  delay(100);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
void moveToForward(int speed_motor){
  analogWrite(enB, speed_motor);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  delay(100);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void impulseMotorForce(int sense){
  if(!sense){
    moveToBackward(speed_very_fast);
  }else{
    moveToForward(speed_very_fast);
  }
}

float getGateDistance(){
  float cmMsec;
  long microsec = ultrasonic.timing();
  cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
  return cmMsec;
}
