// LIGHT MODE
// general LED patterns
uint16_t LGNrainbowPos = 0;


void progDelay(uint16_t t) {
  for (uint16_t i=0; i<(t/10); i++) {
    delay(10);
    if (progBreak) {
      return;
    }
  }
}


void pixelNextVal(uint16_t p, uint8_t min, uint8_t max, uint8_t step) {
  if (abs(pixelBrightness[p]) <= min) {
    // low-end; cause some randomness
    int8_t pauseMe = random(2);
    if (pauseMe > 0) {
      pixelBrightness[p] = min;
    } else {
      pixelBrightness[p] = min + step;
      pixelVal[p] = !pixelVal[p];
    }
  } else if (pixelBrightness[p] == 127) {
    // top end; increment should wrap a signed int, but we trap just to be safe
    pixelBrightness[p] = -126;
  } else if (abs(pixelBrightness[p]) >= max) {
    // exceed current max brightness
    pixelBrightness[p] = (max * -1) + step;
  } else {
    // normal increment
    pixelBrightness[p] += step;
  }
}


// set a color based on hue (base color) and brightness (0-127)
uint32_t pixelSetHue(uint32_t c, int8_t v) {
  uint32_t data;
  v = abs(v);
  
  uint8_t g = map((c>>16 & 0x7f), 0,127, 0,v);
  uint8_t r = map((c>>8 & 0x7f), 0,127, 0,v);
  uint8_t b = map((c & 0x7f), 0,127, 0,v);
  
  data = g | 0x80;
  data <<= 8;
  data |= r | 0x80;
  data <<= 8;
  data |= b | 0x80;

  return data;
}



void flashReset() {
  uint32_t c = strip.Color(1,1,1);
  
  clearStrip();
  delay(400);
  stripColor(c);
  delay(25);
  clearStrip();
  delay(400);
  stripColor(c);
  delay(25);
  clearStrip();
  delay(400);
  
  currentProg++;
}

void fadeReset() {
  int8_t r;
  int8_t g;
  int8_t b;
  
  for (uint8_t i=0; i<128; i++) {
    for (uint16_t p=0; p<NPIX; p++) {
      g = ((strip.pixels[p]>>16) & 0x7F);
      r = ((strip.pixels[p]>>8) & 0x7F);
      b = ((strip.pixels[p]) & 0x7F);
      
      g = constrain(g-1, 0, 127);
      r = constrain(r-1, 0, 127);
      b = constrain(b-1, 0, 127);
      
      strip.setPixelColor(p, r,g,b);
    }
    strip.show();
    delay(10);
  }
}


// strip one color
void stripColor(uint32_t c) {
  for (uint16_t p=0; p<NPIX; p++) {
    strip.setPixelColor(p, c);
  }
  strip.show();
}



// strip rainbow; using ADA wheel
void rainbowFull() {
  stripColor(Wheel(LGNrainbowPos));
  strip.show();		// write all the pixels out
  delay(60);
  for (;;);
  rainbowWheelIncr();  
}

// faded rainbow
void rainbowCurve() {
  uint32_t c = Wheel(LGNrainbowPos);
  for (uint16_t p=0; p<NPIX; p++) {
    strip.setPixelColor(p, pixelSetHue(c, map(abs(p-80), 80,0, 1,127)));
  }
  strip.show();		// write all the pixels out
  delay(60);
  rainbowWheelIncr();  
}

// moving rainbow
void rainbowCycle() {
  for (uint16_t p=0; p<NPIX; p++) {
    strip.setPixelColor(p, Wheel((LGNrainbowPos+(384*4)-p) % 384));
  }
  delay(10);
  strip.show();
  rainbowWheelIncr();
}

void rainbowWheelIncr() {
  LGNrainbowPos++;
  if (LGNrainbowPos >= 384) {
    LGNrainbowPos = 0;
  }
}

