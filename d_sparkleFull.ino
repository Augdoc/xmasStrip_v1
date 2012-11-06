// DRUM MODE
// uses the entire strip as a single color pulse
// each drum hit brightens the strip, based on the color(s) hit


//sparkle mode data
uint8_t DSFcolorDim = 4;
uint8_t DSFcolorHigh = 96;
uint8_t DSFcolorMax = DSFcolorDim;
uint8_t DSFcolorFadeIncr = 12;

uint8_t DSFhueR;
uint8_t DSFhueG;
uint8_t DSFhueB;



void sparkleFullInit() {
  midiClearAll();
  detachInterrupt(0);
  attachInterrupt(0, fourBitInput, FALLING);
  fourBitInput();
  
  DSFsetHue(colorWhite);
  
  for (uint16_t p=0; p<NPIX; p++) {
    pixelBrightness[p] = random(DSFcolorDim);
    uint8_t dir = random(2);
    if (dir < 1) {
      pixelBrightness[p] *= -1;
    }
  }
  
  for (;;) {
    sparkleFullLoop();
    if (progBreak) {
      return;
    }
  }
}


void sparkleFullLoop() {
  midiOutput();
  
  if (fourBitHits > 0) {
    DSFsetHue(fourBitColors[fourBitHits]);
    DSFcolorMax = DSFcolorHigh; //constrain(DSFcolorMax+6, 0, 127);
    fourBitHits = 0;
    
    for (uint16_t p=0; p<NPIX; p++) {
      pixelBrightness[p] = constrain((abs(pixelBrightness[p])*8)+32, 0, DSFcolorHigh)  * -1;
      strip.setPixelColor(p, DSFhueValue(abs(pixelBrightness[p])));
    }
  } else {
    for (uint16_t p=0; p<NPIX; p++) {
      DSFnextVal(0, p);
      strip.setPixelColor(p, DSFhueValue(abs(pixelBrightness[p])));
    }
  }
  strip.show();
  
  if (DSFcolorMax > DSFcolorDim) {
    DSFcolorMax = constrain(DSFcolorMax - 16, DSFcolorDim, 127);
  } else {
    DSFwhitenHue();
  }
  //delay(5);  
}


void DSFwhitenHue() {
  DSFhueR = constrain(DSFhueR+1, 0, 127);
  DSFhueG = constrain(DSFhueG+1, 0, 127);
  DSFhueB = constrain(DSFhueB+1, 0, 127);
}

void DSFsetHue(uint32_t c) {
  DSFhueR = (c >> 8) & B01111111;
  DSFhueG = (c >> 16) & B01111111;
  DSFhueB = (c) & B01111111;
}

uint32_t DSFhueValue(uint8_t v) {
  uint32_t data;
  uint8_t r = map(DSFhueR, 0,127, 0,v);
  uint8_t g = map(DSFhueG, 0,127, 0,v);
  uint8_t b = map(DSFhueB, 0,127, 0,v);
  
  data = g | 0x80;
  data <<= 8;
  data |= r | 0x80;
  data <<= 8;
  data |= b | 0x80;

  return data;
}


void DSFnextVal(uint8_t c, uint16_t p) {
  if (pixelBrightness[p] == 0) {
    // center; cause some randomness
    int8_t pauseMe = random(2);
    if (pauseMe > 0) {
      pixelBrightness[p] = 0;
    } else {
      pixelBrightness[p] = 1;
    }
  } else if (pixelBrightness[p] == 127) {
    // top end; increment should wrap a signed int, but we trap just to be safe
    pixelBrightness[p] = -126;
  } else if (abs(pixelBrightness[p]) >= DSFcolorMax+1) {
    // exceeds current max brightness by "a lot"
    pixelBrightness[p] = random(DSFcolorMax - pixelBrightness[p])+1; // floor((DSFcolorVal[p]+DSFcolorMax)/2);
  } else if (abs(pixelBrightness[p]) >= DSFcolorMax) {
    pixelBrightness[p] = (DSFcolorMax * -1) + 1;
  } else {
    // normal increment
    pixelBrightness[p]++;
  }
}  

