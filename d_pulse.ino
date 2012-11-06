// DRUM MODE
// constant white pulses down the strip
// each drum hit changes the color and creates a new pulse


// pulse mode data
uint8_t DPLpulseSize = 32;
uint8_t DPLpulseCount = 0;
uint8_t DPLinputCount = 0;
uint16_t DPLpulseTempo = (1000000 / 32);  /* defined in "pixels per second" */

uint32_t DPLpulseColor = colorIdle;
uint32_t DPLtimeA;
uint32_t DPLtimeB;


void pulseInit() {
  midiClearAll();
  detachInterrupt(0);
  attachInterrupt(0, fourBitInput, FALLING);
  fourBitInput();
  
  for (;;) {
    doPulse();
    if (progBreak) {
      return;
    }
  }
}


void doPulse() {
  midiOutput();
  
  DPLtimeA = micros();
  // move everyting over
  for (uint16_t i=NPIX-1; i>0; i--) {
    strip.pixels[(i*3)]   = strip.pixels[(i*3)-3];
    strip.pixels[(i*3)+1] = strip.pixels[(i*3)-2];
    strip.pixels[(i*3)+2] = strip.pixels[(i*3)-1];
  }
  
  strip.setPixelColor(0, DPLpulseColor);
  strip.show();
  
  // determine next iteration
  DPLpulseCount++;
  if (DPLpulseCount >= DPLpulseSize) {
    DPLpulseCount = 0;
    DPLpulseColor = colorIdle;
  } else if (((DPLpulseColor & 0x7F) <= 1) && (((DPLpulseColor>>8) & 0x7F) <= 1) && (((DPLpulseColor>>16) & 0x7F) <= 1)) {
    // do nothing
  } else {
    // "divide" pixel color in half
    // keep left bits 1 to signal color; reset left bits that were shifted into second position (for each byte)
    DPLpulseColor >>= 1;
    DPLpulseColor |= 0x808080;
    DPLpulseColor &= 0xBFBFBF;
  }
  
  
  // at 480 bps, we "ignore" input during the first four pulses
  if (fourBitHits > 0  &&  DPLpulseCount > 4) {
    DPLpulseColor = fourBitColors[fourBitHits];    
    DPLpulseCount = 0;
    DPLinputCount = 0;
    fourBitHits = 0;
  }
  
  // keep time
  DPLtimeB = micros() - DPLtimeA;
  //Serial.println(DPLtimeB);
  uint32_t msDelays = constrain((DPLpulseTempo - DPLtimeB), 1, 500000) / 1000;
  delay(msDelays);
}

