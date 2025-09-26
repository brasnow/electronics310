/*
  how fast can we turn the voltage pot before we see voltage skew between the first and Nth measurement?
  23sept20 BR

  hardware: LM317 output -> voltage divider 10k/(10k+22k) -> A2
*/

#define VoutPin A2
#define NUMSAMPLES 10

volatile int Vout[NUMSAMPLES][3], i, j;

void setup()
{
  Serial.begin(9600);
}   // setup

void loop()
{
  // get data fast
  for (j = 0; j < 3; j++)
    for (i = 0; i < NUMSAMPLES; i++)
      Vout[i][j] = analogRead(VoutPin);

  for (j = 0; j < 3; j++)
  {   // print the results
    Serial.print(j + 1);
    Serial.print(": ");
    for (i = 0; i < NUMSAMPLES; i++)
    {
      Serial.print(Vout[i][j]);
      Serial.print("\t");
    }
    Serial.print("\n");
  }
  delay(50);
}   // loop
