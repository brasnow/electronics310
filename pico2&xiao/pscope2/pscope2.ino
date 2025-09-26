/* file: pscope2.ino COMMAND SET
   T -- measure temperature from DS18B20, returns degC as ascii string
   b -- measure interleaved A0, A1 appended with time (microsecs)
   d <microseconds> -- delay <microseconds> between data acquisitions 
   p <pwmValue> write <pwmValue> integer 0-1023 to analogOutPin 

     19feb25 B.R. setup may only run on a pico during hard boot, and not  
     duringsw connection to the serial port
*/

#include <OneWire.h>
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 16  // DS18B20 data pin
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

#define NUMSAMP 5000
uint16_t pwmVal = 0;
uint16_t data[NUMSAMP + 1][2], i, microsecDelay = 0;
const int analogOutPin = 15;
int settleTime = 200;  // msec
uint32_t t0, t1;
void measureWaveformsBinary(void);

void setup() {
  Serial.ignoreFlowControl(true);  // USB UART bug
  Serial.begin(115200);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  analogReadResolution(14);
  pinMode(analogOutPin, OUTPUT);
  analogWriteResolution(10);
  analogWrite(analogOutPin, 0);
  analogRead(A0);            // start ADC and express AREF
  Serial.println("pscope");  // wait for UART to boot

  sensors.begin();
}  // setup

void loop() {
  if (Serial.available()) {
    switch (Serial.read()) {
      case 'T':  // read temperature
        {
          sensors.requestTemperatures();
          delay(750);  // max 12bit conversion time, p.3 of datasheet
          Serial.println(sensors.getTempCByIndex(0)); // ascii data
          break;
        }
      case 'p':  // set pwm value
        {
          pwmVal = Serial.parseInt();
          analogWrite(analogOutPin, pwmVal);
          delay(settleTime);
          break;
        }
      case 'b':  // readAndWriteWaveformsBinary
        {
          measureWaveformsBinary();
          break;
        }
      case 'd':  // delay (microseconds) between data acquisitions
        {
          microsecDelay = Serial.parseInt();
          break;
        }
    }
  }  // switch
  delay(1);
}  // loop

void measureWaveformsBinary(void) {  // Measure  NUMSAMP values on A0 and secondChan
  // return data and time in binary format
  t0 = micros();  // time it to compute sample rate
  for (i = 0; i < NUMSAMP; i++) {
    data[i][0] = analogRead(A0);
    delayMicroseconds(microsecDelay);
    data[i][1] = analogRead(A1);
    delayMicroseconds(microsecDelay);  // even interleave
  }
  t1 = micros();
  data[i][0] = (uint16_t)(t1 - t0);          // put dt at end of data
  data[i][1] = (uint16_t)((t1 - t0) >> 16);  // most significant 2 bytes
  Serial.write((uint8_t*)data, (NUMSAMP + 1) * 2 * sizeof(uint16_t));
}  // measureWaveformsBinary
