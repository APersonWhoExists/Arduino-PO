#include <RH_ASK.h>

// Joystick stuff
#define Xpin 0
#define Ypin 1
int x_movement;
int y_movement;

// Transmitter stuff
uint8_t joystickStatus = false;
char* send_bits = new char[1] {0x00};
RH_ASK driver(2400, 11, 12, 10, true);

void setup() {
  Serial.begin(115200);

  driver.init();
}

void loop() {
  // Detecting input from the joystick
  x_movement = analogRead(Xpin);
  y_movement = analogRead(Ypin);

  // Combining x and y into one byte
  *send_bits = x_movement/64 << 4 | y_movement/64;
  Serial.println(*send_bits, HEX);

  // Sending the byte with x and y
  driver.send((uint8_t *)send_bits, 1);
  driver.waitPacketSent();
  delay(10);
}
