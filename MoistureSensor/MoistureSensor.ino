/* MoistureSensor.ino
 * Test sketch to try out the moisture sensor.
 */

#define debugSerial SerialUSB

void setup() {
  // put your setup code here, to run once:
  debugSerial.begin(115200);
  //pinMode(A0, INPUT);

    while ((!SerialUSB) && (millis() < 10000)) {
    // Wait for SerialUSB or start after 10 seconds
  }

  debugSerial.println(F("Ready"));
}

void loop() {
  float moisture = -100;

  // run 10 times quickly and take the maximum (for stability)
  for (int i=0; i<10; i++) {
    float temp = getMoister();
    // save value if it's higher than is already recorded
    if (temp > moisture) {
      moisture = temp;
    }
    delay(10);
  }

  debugSerial.print(moisture, 0);
  debugSerial.println(" steps");
}

int getMoist()
{
  int sensorPin = A0;
  return analogRead(sensorPin);
}

int16_t getMoister()
{
  int sensorPin = A0;
  return (1023-analogRead(sensorPin))/10.23;
}

float getMoistest()
{
  int sensorPin = A0;
  int adc = 1023-analogRead(sensorPin);
  return (float)(adc)*-1 / 1023;
}
