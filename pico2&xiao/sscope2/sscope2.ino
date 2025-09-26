/* file: sscope2.ino COMMAND SET
  r <ADC resolution> -- set ADC resolution  <= added this and next in sscope2.ino
  n <numSamples> -- return numSamples (<=NUMSAMP)) samples on A0 and A1
   b -- measure interleaved A0, A1 appended with time (microsecs)
   d <microseconds> -- delay <microseconds> between data acquisitions 
   p <pwmValue> write <pwmValue> integer 0-1023 to analogOutPin 
*/
#define NUMSAMP 5000
#define ANALOG_OUT_PIN D7 /* bottom corner */
uint16_t pwmVal = 0;
uint16_t data[NUMSAMP+1][2], i, microsecDelay=0, nchan=2;
int settleTime = 200; // msec
uint32_t t0, t1;
void measureWaveformsBinary(void);
void setup()
{
//  Serial.ignoreFlowControl(true) ; // USB UART bug
  Serial.begin(115200);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  analogReadResolution(12);
  pinMode(ANALOG_OUT_PIN, OUTPUT);
  analogWriteResolution(12);
  analogWrite(ANALOG_OUT_PIN, 0);
  Serial.println("sscope"); // wait for UART to boot
}   // setup
void loop()
{
  if (Serial.available())
  {
    switch (Serial.read())
    {
      case 'p':   // set pwm value
        {
          pwmVal = Serial.parseInt();
          analogWrite(ANALOG_OUT_PIN, pwmVal);
          delay(settleTime);
          break;
        }
       case 'b':   // readAndWriteWaveformsBinary
        {
          measureWaveformsBinary();
          break;
        }
       case 'd': // delay (microseconds) between data acquisitions
      {
        microsecDelay = Serial.parseInt();
        break;
      }
    }
  }   // switch
  delay(1);
} // loop
void measureWaveformsBinary(void)
{ // Measure  NUMSAMP values on A0 and secondChan
  // return data and time in binary format
  t0 = micros();    // time it to compute sample rate
  for (i = 0; i < NUMSAMP; i++)
  {
    data[i][0] = analogRead(A0);
    delayMicroseconds(microsecDelay);
    data[i][1] = analogRead(A1);
    delayMicroseconds(microsecDelay);   // even interleave
  }
  t1 = micros();
  data[i][0] = (uint16_t) (t1 - t0);  // put dt at end of data
  data[i][1] = (uint16_t) ((t1 - t0) >> 16); // most significant 2 bytes
  Serial.write((uint8_t*)data, (NUMSAMP + 1) * 2 * sizeof(uint16_t));
}   // measureWaveformsBinary
