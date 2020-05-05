// Used libraries are ServoTimer2 for control of the servo and RH_ASK for the radio receiver.
// ServoTimer2 is used instead of Servo.h because ServoTimer2 does not use Timer1. RH_ASK also uses Timer1 and this will result in conflict otherwise.
#include <ServoTimer2.h>
#include <RH_ASK.h>

// Servo stuff
#define servoPin 9
ServoTimer2 servo;

// Stepper stuff
#define IN1 4
#define IN2 5
#define IN3 6
#define IN4 7
int Steps = 0;
bool Direction = true;
unsigned long last_time;
unsigned long currentMillis;
//long time;

// Receiver stuff
RH_ASK driver(2400, 11, 12, 10, true);
int x_movement = 8;  // The x and y movement get sent in one byte, both range from 0 to 15 so 8 is the "neutral" state.
int y_movement = 8;  // This also means that there are only 16 different positions for the servo, however this is plenty for just steering

void setup() {
  Serial.begin(115200);

  // Servo stuff
  servo.attach(servoPin);

  // Stepper stuff
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Receiver stuff
  driver.init();
}

void loop() {
  // Detecting input from receiver
  uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];
  uint8_t buflen = sizeof(buf);
  if (driver.recv(buf, &buflen))
  {
    // Separating x and y
    x_movement = ((buf[0] & 0xF0) >> 4);
    y_movement = (buf[0] & 0x0F);
    Serial.println(buf[0], HEX);
  }

  // Moving the servo according to x
  servo.write(x_movement * 32 + 1000);

  // Moving the stepper according to y
  currentMillis = micros();
  if (currentMillis - last_time >= 1000) {
    if (y_movement > 8) { // Forward or backward
      Direction = true;
    } else {
      Direction = false;
    }
    if (y_movement != 8) { // Only move if the joystick is not in the "neutral" state
      stepper(1);
      //time = time + micros() - last_time;
      last_time = micros();
    }
  }
  
  delay(1);
}

void stepper(int xw) { // Moving the stepper
  for (int x = 0; x < xw; x++) {
    switch (Steps) {
      case 0:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        break;
      case 1:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, HIGH);
        break;
      case 2:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        break;
      case 3:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        break;
      case 4:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
      case 5:
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
      case 6:
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
      case 7:
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        break;
      default:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
    }
    SetDirection();
  }
}
void SetDirection() { // Updating steps
  if (Direction == 1) {
    Steps++;
  }
  if (Direction == 0) {
    Steps--;
  }
  if (Steps > 7) {
    Steps = 0;
  }
  if (Steps < 0) {
    Steps = 7;
  }
}
