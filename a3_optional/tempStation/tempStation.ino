/*
 * Name: Hwa-seung Erstling
 
  Reads temperature sensor, converts reading
  into Farhenheit and outputs value to a
  4-digit 7-segment display (two significant digits).
  Sends message-wrapped temperature to serial port
  to be read potentially from other processes.
  
 */

const int inPin = 0;    // analog pin

// 7-segments (starting at top)
// 1-decimal point
int a = 1;
int b = 2;
int c = 3;
int d = 4;
int e = 5;
int f = 6;
int g = 7;
int p = 8;

int d4 = 9;
int d3 = 10;
int d2 = 11;
int d1 = 12;

long n = 0;
// Change value for rate of temperature readings
int x = 100;
int del = 55;
long prevNum = 0;

// Stores most recent temperature reading
// Uses two digits to left and right of decimal
int digit_1;
int digit_2;
int digit_3;
int digit_4;

boolean readTempMode = false;

void setup()
{
  pinMode(d1, OUTPUT);
  pinMode(d2, OUTPUT);
  pinMode(d3, OUTPUT);
  pinMode(d4, OUTPUT);
  pinMode(a, OUTPUT);
  pinMode(b, OUTPUT);
  pinMode(c, OUTPUT);
  pinMode(d, OUTPUT);
  pinMode(e, OUTPUT);
  pinMode(f, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(p, OUTPUT);

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. 
  }
  // Send a new line initially to signal arduino startup
  Serial.println("");
}

// -------------------------
// LOOP
// -------------------------

void loop()
{
  
  // prevNum will change ~1 second
  // then read temperature
  // Change value of x (see above)
  // to change rate of temperature readings
  if ( ((n/x)%10) != prevNum ) {
    prevNum = (n/x)%10;
    readTempAndOutput();
  }
  
  clearLEDs();
  pickDigit(1);
  pickNumber(0);
  delayMicroseconds(del);
 
  clearLEDs();
  pickDigit(2);
  pickNumber(0);
  delayMicroseconds(del);
 
  clearLEDs();
  pickDigit(3);
  pickNumber(digit_1);
  delayMicroseconds(del);
 
  clearLEDs();
  pickDigit(4);
  pickNumber(digit_2);
  delayMicroseconds(del);
  
  n++;
 
  if (digitalRead(13) == LOW)
  {
    n = 0;
  }
  
}
// -------------------------
// END - LOOP
// -------------------------

void readTempAndOutput() {
  
  // Serial event received to start reading temperature!
  int value = analogRead(inPin);
  float millivolts = (value / 1024.0) * 5000;
  float celsius = millivolts / 10;  // sensor output is 10mV per degree Celsius
    
  float ftemp = (celsius * 9)/ 5 + 32;
  int fint = (int) ftemp;
    
  float fdtemp = (ftemp - fint)*100;
  int fdec = (int) fdtemp;
  
  // Extract digits of tempreature to display on 
  digit_2 = fint % 10;
  fint /= 10;
  digit_1 = fint % 10;
  digit_4 = fdec % 10;
  fdec /= 10;
  digit_3 = fdec % 10;

  if (readTempMode == true) {
    Serial.print("Celsius: ");
    Serial.print(celsius);
    Serial.print(", Fahrenheit: ");
    Serial.print( ftemp );  //  converts celsius to fahrenheit
    Serial.print(";");
    readTempMode = false;
    }  
}


// -------------------------
// SERIAL EVENT
// -------------------------

void serialEvent() {
  if (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();

    if (inChar == 'c') {
      readTempMode = true;
    }
    if (inChar == 'x') {
      readTempMode = false;
    }
  }
}

// -------------------------
// END - SERIAL EVENT
// -------------------------

void pickDigit(int x) 
{
  digitalWrite(d1, HIGH);
  digitalWrite(d2, HIGH);
  digitalWrite(d3, HIGH);
  digitalWrite(d4, HIGH);
 
  switch(x)
  {
  case 1: 
    digitalWrite(d1, LOW); 
    break;
  case 2: 
    digitalWrite(d2, LOW); 
    break;
  case 3: 
    digitalWrite(d3, LOW); 
    break;
  default: 
    digitalWrite(d4, LOW); 
    break;
  }
}
 
void pickNumber(int x)
{
  switch(x)
  {
  default: 
    zero(); 
    break;
  case 1: 
    one(); 
    break;
  case 2: 
    two(); 
    break;
  case 3: 
    three(); 
    break;
  case 4: 
    four(); 
    break;
  case 5: 
    five(); 
    break;
  case 6: 
    six(); 
    break;
  case 7: 
    seven(); 
    break;
  case 8: 
    eight(); 
    break;
  case 9: 
    nine(); 
    break;
  }
}
 
void dispDec(int x)
{
  digitalWrite(p, LOW);
}
 
void clearLEDs()
{
  digitalWrite(a, LOW);
  digitalWrite(b, LOW);
  digitalWrite(c, LOW);
  digitalWrite(d, LOW);
  digitalWrite(e, LOW);
  digitalWrite(f, LOW);
  digitalWrite(g, LOW);
  digitalWrite(p, LOW);
}
 
void zero()
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, LOW);
}
 
void one()
{
  digitalWrite(a, LOW);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, LOW);
  digitalWrite(e, LOW);
  digitalWrite(f, LOW);
  digitalWrite(g, LOW);
}
 
void two()
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, LOW);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, LOW);
  digitalWrite(g, HIGH);
}
 
void three()
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, LOW);
  digitalWrite(f, LOW);
  digitalWrite(g, HIGH);
}
 
void four()
{
  digitalWrite(a, LOW);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, LOW);
  digitalWrite(e, LOW);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
}
 
void five()
{
  digitalWrite(a, HIGH);
  digitalWrite(b, LOW);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, LOW);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
}
 
void six()
{
  digitalWrite(a, HIGH);
  digitalWrite(b, LOW);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
}
 
void seven()
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, LOW);
  digitalWrite(e, LOW);
  digitalWrite(f, LOW);
  digitalWrite(g, LOW);
}
 
void eight()
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, HIGH);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
}
 
void nine()
{
  digitalWrite(a, HIGH);
  digitalWrite(b, HIGH);
  digitalWrite(c, HIGH);
  digitalWrite(d, HIGH);
  digitalWrite(e, LOW);
  digitalWrite(f, HIGH);
  digitalWrite(g, HIGH);
}
