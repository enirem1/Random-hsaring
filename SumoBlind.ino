// GENESIS BOARD MINI SUMO ROBOT PROGRAM
//FOR 3 OPPONENT SENSOR, 2 EDGE SENSOR
// JSUMO 01/2020
///////////////////////////////////////

//MOTOR CONTROL
int RPwm = 11;
int RDir = 13;

int LPwm = 3;
int LDir = 12;

//LED & BUZZER
int Buzzer = 9;
int ArduLed = 8;

//EDGE & CONTRAST SENSORS
const int Redge = A1;
const int Ledge = A0;

//TRIMPOTS
int SPD = A7;
int TRN = A6;
bool lastLine = true;
//OPPONENT SENSORS
int LSens = A4;
int RSens = A2;
int MSens = A3;

// DIPSWITCH & BUTTON
int Button = 10;  // Can be used as start pin too.
int DS1 = 5;
int DS2 = 6;
int DS3 = 7;

//VALUES
int Speed = 10;
int MaxSpeed = 70;   // Idle Speed while no sensor giving data.
int TurnSpeed = 30;  // Left and Right Forward Turning Speed
int EdgeTurn = 150;  // Turning Time variable when minisumo sees white line
int Duration;        // Turning Time at minisumo starting.
int LastValue = 5;   // Last Value Variable for remembering last Opponent sensor sense.

void setup() {
  pinMode(LSens, INPUT);  // Left Opponent Sensor Input
  pinMode(RSens, INPUT);  // Right Opponent Sensor Input
  pinMode(MSens, INPUT);  // Middle Opponent Sensor Input

  pinMode(Buzzer, OUTPUT);   // Buzzer Declared as Output
  pinMode(ArduLed, OUTPUT);  // Buzzer Declared as Output
  pinMode(Button, INPUT);    // Buzzer Declared as Output

  pinMode(RPwm, OUTPUT);  // Four PWM Channel Declared as Output
  pinMode(RDir, OUTPUT);
  pinMode(LPwm, OUTPUT);
  pinMode(LDir, OUTPUT);

  digitalWrite(Buzzer, LOW);   // Buzzer Pin Made Low for Silence 🙂
  digitalWrite(ArduLed, LOW);  // Arduino Mode Led Made Low
  digitalWrite(DS1, HIGH);     // 3 Dipswitch Pin Pullups Made
  digitalWrite(DS2, HIGH);
  digitalWrite(DS3, HIGH);
  randomSeed(analogRead(7));
  digitalWrite(LSens, HIGH);  // Opponent Sensor Pullups Made
  digitalWrite(RSens, HIGH);

  digitalWrite(MSens, HIGH);
  Serial.begin(9600);
}
bool mDelay(int time) {
  int start = millis();
  while (millis() - start <= time) {
    lastLine = false;
    return true;
  }
  return false;
}

//Motor Control Function
void Set_Motor(float Lval, float Rval, int timex) {
  Lval = Lval * 2.5;
  Rval = Rval * 2.5;
  if (Lval >= 0) {
    analogWrite(LPwm, Lval);
    digitalWrite(LDir, LOW);
  } else {
    Lval = abs(Lval);
    digitalWrite(LDir, HIGH);
    analogWrite(LPwm, Lval);
  }
  if (Rval >= 0) {
    analogWrite(RPwm, Rval);
    digitalWrite(RDir, HIGH);
  } else {
    Rval = abs(Rval);
    digitalWrite(RDir, LOW);
    analogWrite(RPwm, Rval);
  }
  //If you want to see Speed Values please uncomment bottom line.
  // Serial.print(Rval); Serial.print("-"); Serial.println(Lval);
  delay(timex);
}


void OpDetect() {
  int start = millis();
  while (1) {
    if (analogRead(Ledge) < 300 && analogRead(Redge) > 300) {
      int rndN = random(75, 150);
      Set_Motor(-100, -100, 100);  // Backward for 55 mS.
      Set_Motor(-100, 100, rndN);
      start = millis();  // Left Backward, Right Forward, Turning Time Based on ETRN Trimpot
      LastValue = 5;
    } else if (analogRead(Ledge) > 300 && analogRead(Redge) < 300) {
      digitalWrite(Buzzer, LOW);
      int rndN = random(75, 150);
      digitalWrite(ArduLed, HIGH);
      Set_Motor(-100, -100, 100);  // Backward for 55 mS.
      Set_Motor(100, -100, rndN);  // Right Backward, Left Forward, Turning Time Based on ETRN Trimpot
      LastValue = 5;
      start = millis();
    } else if (analogRead(Ledge) < 300 && analogRead(Redge) < 300) {
      digitalWrite(Buzzer, LOW);
      digitalWrite(ArduLed, HIGH);
      Set_Motor(-100, -100, 100);
      int rndN = random(100, 150);  // Backward for 55 mS.
      Set_Motor(100, -100, rndN);   // Right Backward, Left Forward, Turning Time Based on ETRN Trimpot
      LastValue = 5;
      start = millis();
    }


      /// Edge Sensor Control Routine ///



      Speed = (analogRead(SPD) / 10.3);  // This raw speed value comes with 0 to 1023 so we divide to 10.3 After that it is 0 to 99 integer.
      Speed = 100 - Speed;
      // This is used for reversing trimpot dial direction at board.
      Speed = 20;
      if (millis() - start <= 1500) {
        Serial.println("SLOW");
        Set_Motor(Speed, Speed, 1);
      } else {
         Serial.println("Fast");
        Set_Motor(MaxSpeed, MaxSpeed, 1);
      }
    }
  }

void loop() {
  digitalWrite(RPwm, LOW);
  digitalWrite(LPwm, LOW);
  tone(Buzzer, 18, 100);  // Pin, Frequency, Duration
  //////////////////////////////////////////////
  // This function below, used for stopping robot while no button is pushed or Microstart is not triggered.
  while (digitalRead(Button) == 0) {
    Serial.println("Button Press Waited");
    Set_Motor(0, 0, 1);
    /// Sensor Control While Waiting The Button Press ///
    if (digitalRead(MSens) == LOW || digitalRead(RSens) == LOW || digitalRead(LSens) == LOW || analogRead(Redge) < 100 || analogRead(Ledge) < 100) {
      digitalWrite(ArduLed, HIGH);
    } else {
      digitalWrite(ArduLed, LOW);
    }
  }
  ///////////////////////////////////////////////
  if (digitalRead(Button) == 1) {

    Duration = (analogRead(TRN) / 5);  // Duration variable based on TRN (A6) trimpot
    Duration = 205 - Duration;         //

    Serial.println("5 Sec Routine Started");

    for (int i = 0; i < 5; i++) {
      digitalWrite(Buzzer, HIGH);
      digitalWrite(ArduLed, HIGH);
      delay(100);
      digitalWrite(Buzzer, LOW);
      digitalWrite(ArduLed, LOW);
      delay(900);
    }


    Serial.print("OK");
    digitalWrite(Buzzer, LOW);
    EdgeTurn = (analogRead(TRN) / 5);  // Raw value comes with 0 to 1023, we divide it for 0 t0 205 mS turning time.
    EdgeTurn = 205 - EdgeTurn;         //
  }

  //Main Loop


    OpDetect();
  
}