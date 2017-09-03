/* ExplorerCayenne.ino
 *  
 * Uploads temperature and moisture sensor data to TTN
 * using the Cayenne data payload format.
 * 
 * Requires The Things Network Library: https://github.com/TheThingsNetwork/arduino-device-lib
 * (CaynneLPP.h is included with this library)
 *  
 * @DefProc
 */

#include <TheThingsNetwork.h>
#include <CayenneLPP.h>

// Set your AppEUI and AppKey
const char *appEui = "";
const char *appKey = "";
const int MOIST_SENSOR = A0;
 
#define debugSerial SerialUSB
#define hwSerial    Serial
#define btSerial    Serial1
#define loraSerial  Serial2

// Use either TTN_FP_EU868 or TTN_FP_US915
#define freqPlan TTN_FP_EU868

TheThingsNetwork ttn(loraSerial, debugSerial, freqPlan);
CayenneLPP lpp(12); // can set cayenne payload up to 51 bytes, but only need 4 for one reading

void setup()
{
  debugSerial.begin(115200);
  //hwSerial.begin(57600);  
  //btSerial.begin(57600);  
  loraSerial.begin(57600);

  while ((!SerialUSB) && (millis() < 10000)) {
    // Wait for SerialUSB or start after 10 seconds
  }
  
  debugSerial.println(F("ExplorerCayenne"));

  // use the onboard MCP9700AT analog output temperature sensor (on pin A6)
  pinMode(TEMP_SENSOR, INPUT);
  pinMode(MOIST_SENSOR, INPUT);

  debugSerial.println("-- STATUS");
  ttn.showStatus();

  debugSerial.println("-- JOIN");
  ttn.join(appEui, appKey);

}

void loop()
{
  // Vout from MCP9700AT is 10 mV/⁰C - 0.5V
  // -10 to +125⁰C, ±1⁰C accuracy
  float degC = -10.0;
  float moisture = -10.0;
  
  // run 10 times quickly and take the maximum (for stability)
  for (int i=0; i<10; i++) {
    float temp = getTemperature();
    // save value if it's higher than is already recorded
    if (temp > degC) {
      degC = temp;
    }
    delay(10);
  }

  // run 10 times quickly and take the maximum (for stability)
  for (int i=0; i<10; i++) {
    float temp = (float)getMoist();
    // save value if it's higher than is already recorded
    if (temp > moisture) {
      moisture = temp/100;
    }
    delay(10);
  }
  
  debugSerial.print(degC, 6);
  debugSerial.println(" ⁰C");

  debugSerial.print(moisture, 0);
  debugSerial.println("%");

  // clear any readings currently in the cayenne payload
  lpp.reset();
  // add "temperature_1"
  lpp.addTemperature(1, degC);
  lpp.addAnalogInput(2, moisture);

  // Send it off
  ttn.sendBytes(lpp.getBuffer(), lpp.getSize());

  // wait one minute before sending again
  delay(60000);
}

float getTemperature()
{
  //10mV per C, 0C is 500mV
  float mVolts = (float)analogRead(TEMP_SENSOR) * 3300.0 / 1023.0;
  float temp = (mVolts - 500.0) / 10.0;
  return (temp);
}

int16_t getMoist()
{
  int sensorPin = A0;
  return (1023-analogRead(sensorPin))/10.23;
}
