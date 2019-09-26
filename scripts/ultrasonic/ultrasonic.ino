// -----------------------------------------------------------------------------
// Code cho thi nghiem khao sat cam bien HS-SR04
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
// KHAI BAO HANG SO

#define DEBUG 0
#define BAUDRATE 115200
#define TRIGPIN 6
#define ECHOPIN 5
#define BUTTONPIN 2
#define BUFFERSIZE 256
#define BUFFERBYTESIZE BUFFERSIZE*2

#if DEBUG
#define dprint(message) Serial.print(message)
#endif

// -----------------------------------------------------------------------------
// KHAI BAO BIEN GLOBAL

volatile uint16_t distanceBuffer[BUFFERSIZE];
volatile uint16_t bufferCounter;

// -----------------------------------------------------------------------------
// CAC HAM (FUNCTIONS)

uint16_t distance() {
  digitalWrite(TRIGPIN, 0);
  delayMicroseconds(2);
  digitalWrite(TRIGPIN, 1);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, 0);

  return (uint16_t) pulseIn(ECHOPIN, HIGH);
}

void measureManyTimes(int n) {
  for(int i = 0; i < n; i++)
    distanceBuffer[i] = distance();
}

// -----------------------------------------------------------------------------
// THAN CHUONG TRINH

void setup() {
  // ---------------------------------------------------------------------------
  // Khoi tao giao tiep serial voi may tinh
  
  Serial.begin(BAUDRATE);

  // ---------------------------------------------------------------------------
  // Khoi tao cac chan
  
  pinMode(TRIGPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);
  pinMode(BUTTONPIN, INPUT);

  // ---------------------------------------------------------------------------
  // Set du lieu ve 0
  
  memset((void *) distanceBuffer, 0, sizeof(distanceBuffer));
  bufferCounter = 0;
}

void loop() {
  // ---------------------------------------------------------------------------
  // Do 1 lan moi khi bam nut
  
  bool b = digitalRead(BUTTONPIN);
  if(b) { // Measure distance everytime button is pushed:
    distanceBuffer[bufferCounter++] = distance();
    /*
    Serial.println(bufferCounter);
    for(int i = 0; i < bufferCounter; i++) {
      Serial.print(distanceBuffer[i]);
      Serial.print(" ");
    }
    Serial.println();*/
    delay(500);
  }

  // ---------------------------------------------------------------------------
  // Giao tiep voi may tinh
  
  if(Serial.available() > 0) {
    char message = Serial.read(); // Doc tin hieu tu may tinh.
    switch(message) {
      case 'r': // Send all data stored in buffer at once, then reset.
      case 'R': // Gui tat ca du lieu do duoc ve may tinh roi reset.
        Serial.write(bufferCounter);
        Serial.write((uint8_t *) distanceBuffer, bufferCounter*2);
        bufferCounter = 0;
        break;
      case 'm': // Measure once and send.
        // Do mot lan:
        Serial.write((uint8_t *) distance(), 2);
        break;
      case 'M': // Fill buffer with measured distance and send all.
        // Do 256 lan:
        measureManyTimes(BUFFERSIZE);
        Serial.write((uint8_t *) distanceBuffer, BUFFERBYTESIZE);
        break;
    }
  }
}

// -----------------------------------------------------------------------------
