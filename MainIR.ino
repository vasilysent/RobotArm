//====================================// Settings (YOU CAN CHANGE ONLY THIS PART (if you don't know how the code works))
byte mot_speed_up = 100;        // speed of raising the shoulder  [0-100]              Recommended: 100
byte mot_speed_down = 75;       // speed of lowering the shoulder [0-100]              Recommended: 75
int servo_speed_up = 2;         // speed of raising the shoulder (the less the faster) Recommended: 2
int servo_speed_down = 8;       // speed of raising the shoulder (the less the faster) Recommended: 8

int mid_delay = 2500;           // delay at the upper position    [milliseconds]
int sleep_delay = 25000;        // delay between complete actions [milliseconds]

int servo_high_pos = 20;         // elbow upper position [deg]
int servo_low_pos = 145;         // elbow lower position [deg]
//====================================//
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#include <Servo.h>

//-----------------------------------//
#define PWM 5
#define IN1 6
#define IN2 7
#define BUT_LOW 10
#define BUT_HIGH 11
#define ServoPin 9
#define IRpin 12
//-----------------------------------// Arduino pins


Servo MyServo;
unsigned long mot_last_time = 0;

unsigned long servo_last_time = 0;
boolean ButLowSt = 0;
boolean ButHighSt = 0;
int i = 0;
int j = -1;
int e;
int start_pos;
int current_pos;
int servo_delay;

//-----------------------------------//
//byte mot_speed = 100;            // max motor speed
int mot_delay = 10;             // motor acceleration (the less the faster it accelerates) (20 is optimal)
int mot_min_speed = 20;         // min motor speed

int max_for_delay = 20;         // min servo speed (the less the faster) (4)
//int min_for_delay = 5;          // max servo speed (the less the faster)
//int delay_step = 3;             // servo acceleration (the less the faster it accelerates)
//-----------------------------------// Speed settings

//-----------------------------------//

//-----------------------------------// elbow upper and lower positions

int loop_delay = 2000;

boolean RaiseArm = 0;
boolean LowerArm = 0;
boolean BendElbow = 0;
boolean UnbendElbow = 0;
boolean IRstate = false;

//int for_steps = (int)( (max_for_delay - min_for_delay) / delay_step );
unsigned long loop_mot_last_time = loop_delay;
unsigned long loop_servo_last_time = loop_delay;



void setup() {
  
  MyServo.attach(ServoPin);
  pinMode(PWM, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT); 
  pinMode(BUT_LOW, INPUT_PULLUP);
  pinMode(BUT_HIGH, INPUT_PULLUP);
  pinMode(IRpin, INPUT); 
  Serial.begin(9600);


  LowerArm = true;
  RaiseArm = false;
  UnbendElbow = true;
  BendElbow = false;

  while( (LowerArm && !RaiseArm)  || UnbendElbow) {
    lower_arm();
    rotate_servo(servo_low_pos, 30);
  }

  RaiseArm = true;
  LowerArm = false;
  BendElbow = true;
  UnbendElbow = false;

}

void loop()
{ 
  if ((!IRstate) && (digitalRead(IRpin) == HIGH) ) {

    while( (RaiseArm && !LowerArm)  || BendElbow ) {
    raise_arm();
    rotate_servo(servo_high_pos, servo_speed_down);
    }
    delay(mid_delay);
    while( (LowerArm && !RaiseArm)  || UnbendElbow) {
      lower_arm();
      rotate_servo(servo_low_pos, servo_speed_up);
    }
    delay(sleep_delay);
  }

  if ((IRstate) && (digitalRead(IRpin) == LOW) ) {
    IRstate = false;
  }

}


void lower_arm()                                // lowering the arm in the loop iteration
{
  if (LowerArm) {
    if (i == 0) {
      mot_last_time = millis();
      i = mot_min_speed;
    }
    if ( ( ( (millis() - mot_last_time) % mot_delay) == 0) && (i <= mot_speed_down) )
    {
      motor(i);
      i++;
      /*Serial.print("mot speed = ");
      Serial.println(i);*/  
    }
    if (digitalRead((BUT_HIGH) != HIGH) && (i == mot_speed_down)) 
    {   
      ButHighSt = digitalRead(BUT_HIGH);
      //Serial.print("HIGH = ");
      //Serial.println(ButHighSt);
      motor(mot_speed_down);
    }
    if (digitalRead(BUT_HIGH) == HIGH) {
      motor(0);
      i = 0;
      RaiseArm = !RaiseArm;
      LowerArm = !LowerArm;
      mot_last_time = millis();
      loop_mot_last_time = millis();
    }
  }
  
}

void raise_arm(){                               // raising the arm in the loop iteration
  if (RaiseArm) {
    if (i == 0) {
        mot_last_time = millis();
        i = -mot_min_speed;
      }
      if ( ( ( (millis() - mot_last_time) % mot_delay) == 0) && (i >= -mot_speed_up) )
      {
        motor(i);
        i--;
        /*Serial.print("mot speed = ");
        Serial.println(i);*/
      }
      if (digitalRead((BUT_LOW) != HIGH) && (i == -mot_speed_up)) 
      {   
        ButLowSt = digitalRead(BUT_LOW);
        //Serial.print("LOW = ");
        //Serial.println(ButLowSt);
        motor(-mot_speed_up);
      }
      if (digitalRead(BUT_LOW) == HIGH) {
        motor(0);
        i = 0;
        RaiseArm = !RaiseArm;
        LowerArm = !LowerArm;
        mot_last_time = millis();
        loop_mot_last_time = millis();
      }
  }
  
}

void motor(int speed)                            // motor rotation function
{
  speed = map(speed, -100, 100, -255, 255); 
  if (speed >= 0)
  {
    analogWrite(PWM, speed);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  }
  else 
  {
    analogWrite(PWM, -speed);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }
}

void rotate_servo(int aim_pos, int servo_delay)                   // servo rotation in the loop iteration
{
  if (j == -1) {
    start_pos = MyServo.read();
    j = start_pos;
    e = aim_pos - start_pos;
    Serial.print("e = ");
    Serial.println(e);
  }
  if ((j != aim_pos) && ((millis() - servo_last_time) >= servo_delay))
  {
    current_pos = j + sign(e);
    MyServo.write(current_pos);
    //Serial.print("pos = ");
    //Serial.println(current_pos);
    j = current_pos;
     // calc delay for the next iteration
    Serial.print("pos = ");
    Serial.println(current_pos);
    /*if ( ( abs( aim_pos - current_pos ) > for_steps ) && ( abs(current_pos - start_pos) > for_steps ) ){
      servo_delay = min_for_delay;
    }
    else if ( (sign(e)*current_pos) >= sign(e)*(start_pos + int(e/2) ) && (servo_delay > min_for_delay)) {
      servo_delay -= delay_step;
    }
    else if ( ( abs( aim_pos - current_pos ) <= for_steps ) && (servo_delay < max_for_delay) ){
      servo_delay += delay_step;
    }
    Serial.print("del = ");
    Serial.println(servo_delay);*/
    servo_last_time = millis();
  }  
  if (j == aim_pos){
    j = -1;
    BendElbow = !BendElbow;
    UnbendElbow = !UnbendElbow;
    servo_last_time = millis();
    loop_servo_last_time = millis();
  }
}

int sign(int x){              // sigh function
  if (x > 0) {
     return 1;
  }
  if (x < 0){
     return -1;
  }
  if (x = 0){
    return 0;
  }
}
