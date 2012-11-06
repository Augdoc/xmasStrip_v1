/*
  REQUIRED LIBRARIES
*/

// SPI is way faster than bit-banging
  #include <SPI.h>
// adafruit strip library
  #include "LPD8806.h"
// timer for calling recurring events
  #include "TimerOne.h"
// just to keep an eye on RAM usage
  #include "MemoryFree.h"
// send usage data to logger
  #include <SoftwareSerial.h>


/*
  SETUP PIN DEFINITIONS
*/

// interrupts for mode change and drum hits
const uint8_t PIN_int_0 = 2;
const uint8_t PIN_int_1 = 3;

// piezo drum inputs
const uint8_t PIN_btn_r = 0;
const uint8_t PIN_btn_g = 1;
const uint8_t PIN_btn_b = 2;
const uint8_t PIN_btn_y = 3;

// mode button lights up; just because it can
const uint8_t PIN_led_s = 6;


// initialize the LED strip library
const uint16_t NPIX = 160;
LPD8806 strip = LPD8806(NPIX);


/*
  SSTUP PROGRAM DATA
*/

// mode/prog enumeration
int8_t currentMode = 0;
int8_t currentProg = 1;

int8_t maxProgs[2] = {9, 4};
int8_t lastRandomProg = currentProg;
uint8_t currentProgTime = 60;
uint32_t lastActionTime = 0;
uint32_t lastDrumTime = 0;
uint16_t lastDrumCount = 0;


// mode changes
volatile boolean progBreak = false;

volatile uint16_t modeInterrupt = 0;
volatile uint32_t modeSpanTime = 0;
volatile uint32_t modeLastTime = 0;

volatile uint16_t drumInterrupt = 0;
volatile uint32_t drumSpanTime = 0;
volatile uint32_t drumLastTime = 0;

const uint32_t timerFreq = (4) * 1000000;


// debug
uint32_t time;
uint8_t * heapptr, * stackptr;


// serial logging
SoftwareSerial softSerial(7,8);




/*
  SETUP
*/

void setup() {
  // setup serial port for debugging
  Serial.begin(57600);
  Serial.println("INIT");

#if defined(__AVR_ATmega2560__)
  // use mega serial1 for midi
  Serial1.begin(31250);
  midiClearAll();
#endif

  // setup soft serial for data logging
  softSerial.begin(57600);
  
  
  // setup interrupt pins
  pinMode(PIN_int_0, INPUT_PULLUP);
  pinMode(PIN_int_1, INPUT_PULLUP);
  digitalWrite(PIN_int_0, HIGH);
  digitalWrite(PIN_int_1, HIGH);
  
  // setup analog inputs
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  digitalWrite(A0, HIGH);
  digitalWrite(A1, HIGH);
  digitalWrite(A2, HIGH);
  digitalWrite(A3, HIGH);
  
  // setup output pins
  pinMode(PIN_led_s, OUTPUT);
  digitalWrite(PIN_led_s, HIGH);
  
  // setup timer for automatic program rotation (uses PWM on pin 9/10)
  pinMode(10, OUTPUT);
  Timer1.initialize(timerFreq);
  Timer1.attachInterrupt(checkProgTime);
  
  // setup other program-specific data
  progSetup();
  
  // setup master "mode" interrupts
  attachDrumListener();
  attachModeListener();
  
  // initialize strip
  strip.begin();
  delay(500);
}




/*
  OPERATIONS
*/

void progSetup() {
  sparkleBarsSetup();
}


void attachDrumListener() {
  detachInterrupt(0);
  attachInterrupt(0, drumCheck, FALLING);
}

void attachModeListener() {
  detachInterrupt(1);
  attachInterrupt(1, progCycle, FALLING);
}


void drumCheck() {
  // wait at least 1 sec after manual program change
  // to ignore cross-talk on the drum/prog lines
  if (millis()-lastActionTime < 1000) {
    return;
  }
  
  // reset counter if we've exceed a very short time span
  // this is also used to eliminate cross-talk on twisted pair lines which gives false readings
  if (millis() - drumSpanTime > 10) {
    drumInterrupt = 0;
  }
  
  // start counting
  if (drumInterrupt == 0) {
    // start timing
    drumSpanTime = millis();
  }
  drumInterrupt++;
  
  // ignore additional input for a time, so we don't produce extra triggers
  if (drumInterrupt > 2  &&  millis() - drumLastTime > 500) {
    drumInterrupt = 0;
    drumLastTime = millis();
    
    Serial.println("->Drum");
    midiProgChange();
    eventLog(0x20);
    modeDrums();
  }
}


void modeDrums() {
  currentMode = 1;
  progRandom();
  setGlobalAction(0);
  progBreak = true;
}


void checkProgTime() {
  showMem();
  //return;
  //--//eventLog(0x1F);
  
  // incremement program if exceeded currentProgTime
  uint32_t runTime = floor((millis()-lastActionTime) / 1000);
  if (runTime >= currentProgTime) {
    if (currentMode) {
      Serial.println("->Exit");
      eventLog(0x40);
    }
    currentMode = 0;
    attachDrumListener();
    progRandom();
  }
}


void progCycle() {
  //Serial.println("Prog!");
  // interrupt 1 is program button
  // the button may trigger multiple times each press
  // count multiple hits within a very short time span as a single press
  
  // reset counter if we've exceed a very short time span
  // this is also used to eliminate cross-talk on twisted pair lines which gives false readings
  if (millis() - modeSpanTime > 10) {
    modeInterrupt = 0;
  }
  
  // start counting
  if (modeInterrupt == 0) {
    // start timing
    modeSpanTime = millis();
  }
  modeInterrupt++;
  
  // ignore additional input for a time, so we don't produce extra triggers
  if (modeInterrupt > 4  &&  millis() - modeLastTime > 500) {
    modeInterrupt = 0;
    modeLastTime = millis();
    
    Serial.println("->Prog");
    eventLog(0x30);
    progIncrement();
  }
}


void progRandom() {
  while (currentProg == lastRandomProg) {
    currentProg = random(maxProgs[currentMode]);
  }
  lastRandomProg = currentProg;
  
  progChangeAction();
}


void progIncrement() {
  if (currentMode>0  &&  lastRandomProg>0) {
    currentProg = 0;
    lastRandomProg = -1;
  } else {
    currentProg++;
  }
  
  progChangeAction();
}


void progChangeAction() {
//  lastRandomProg = currentProg;
  currentProgTime = 60;
  setGlobalAction(0);
  progBreak = true;
}


void modeIncrement() {
  if (currentMode > 0) {
    currentMode = 0;
    currentProg = 0;
    attachDrumListener();
  } else {
    currentMode = 1;
    currentProg = 0;
  }
}


void setGlobalAction(uint8_t mode) {
  lastActionTime = millis();
  if (mode && (millis()-lastDrumTime > 100)) {
    lastDrumTime = millis();
    lastDrumCount++;
  } 
}


void showMem() {
  Serial.print("Free Memory: ");
  Serial.println(freeMemory(), DEC);
}


void eventLog(byte event) {
  // use soft serial for logging
  softSerial.write(0x3A);
  softSerial.write(event);
  softSerial.write((lastDrumCount >> 8) & 0xFF);
  softSerial.write(lastDrumCount & 0xFF);
  softSerial.write(0x3A);
  softSerial.println();
  
  lastDrumCount = 0;
}




/*
  LOOP
*/

void loop() {  
  progBreak = false;
  if (currentMode) {
    // "drum" mode
    currentProgTime = 45;
    switch (currentProg) {
      case 0: pulseInit(); break;
      case 1: sparkleBarsInit(0); break;
      case 2: sparkleFullInit(); break;
      case 3: bitBarsInit(); break;
      default: modeIncrement(); break;
    };
  } else {
    // "light" mode
    switch (currentProg) {
      case -1: flashReset(); break;
      case 0: xmas4SparkleInit(); break;
      case 1: rainbowFull(); break;
      case 2: randomBuild(0); break;
      case 3: doCylon(0); break;
      case 4: randomFill(); break;
      case 5: rainbowCycle(); break;
      case 6: randomBuild(1); break;
      case 7: doCylon(1); break;
      case 8: xmas7Wipe(); break;
      default: modeIncrement(); break;
    };
  }
}

