/*

rover-arduino:
Parse serial commands from Raspberry Pi master to control DC motors with
Adafruit Motor FeatherWing. Also control an OLED FeatherWing.

*/

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MotorShield.h>

Adafruit_MotorShield AFMS;
Adafruit_SSD1306 display(128, 32, &Wire);

#define BUTTON_A  9
#define BUTTON_B  6
#define BUTTON_C  5

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  AFMS.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(1000);

  // Clear the buffer.
  display.clearDisplay();
  display.display();

  Serial.begin(115200);
}

// Example: All motors forward, stopped
// ffff,0,0,0,0
void parseStr(const char* command) {

#define NUM_MOTORS 4
  char direction[NUM_MOTORS];
  uint16_t speed[NUM_MOTORS];

  int result = sscanf(command, "%c%c%c%c,%u,%u,%u,%u",
                      &direction[0], &direction[1], &direction[2], &direction[3],
                      &speed[0], &speed[1], &speed[2], &speed[3]);

  if (result != 8) {
    Serial.println("NOT OK");
    return;
  } else {
    Serial.println("OK");
  }

  Adafruit_DCMotor *motor;
  for (int i = 0; i < NUM_MOTORS; ++i) {
    motor = AFMS.getMotor(i + 1);
    motor->setSpeed(speed[i]);

    switch (direction[i]) {
      case 'f':
        motor->run(FORWARD);
        break;

      case 'b':
        motor->run(BACKWARD);
        break;

      case 'r':
        motor->run(RELEASE);
        break;
    }
  }

  Serial.println("Directions: ");
  for (char c : direction) {
    Serial.print(" ");
    Serial.print(c);
  }
  Serial.println();

  Serial.println("Speeds: ");
  for (uint16_t s : speed) {
    Serial.print(" ");
    Serial.print(s);
  }
  Serial.println();

}

void loop() {

#define BUFFER_SIZE 50
  static char serialBuffer[BUFFER_SIZE];
  static uint16_t index = 0;

  while (Serial.available()) {
    char c = Serial.read();
    if (index == BUFFER_SIZE) index = 0;
    if (c == '\n') {
      serialBuffer[index++] = 0;
      parseStr(serialBuffer);
      index = 0;
    } else {
      serialBuffer[index++] = c;
    }
  }
}
