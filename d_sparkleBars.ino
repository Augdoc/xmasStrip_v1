// DRUM MODE
// divides the strip into four colors
// each drum hit brightens the color(s) hit, which then fades


//sparkle mode data
uint8_t DSBcolorDim = 8;
uint8_t DSBcolorMax[4] = {DSBcolorDim, DSBcolorDim, DSBcolorDim, DSBcolorDim};
uint8_t DSBcolorFadeIncr = 12;
uint16_t DSBsize = int(NPIX / 4);



void sparkleBarsSetup() {
  DSBcolorMax[0] = DSBcolorDim;
  DSBcolorMax[1] = DSBcolorDim;
  DSBcolorMax[2] = DSBcolorDim;
  DSBcolorMax[3] = DSBcolorDim;
  
  for (uint16_t p=0; p<NPIX; p++) {
    pixelBrightness[p] = random(DSBcolorDim);
    uint8_t dir = random(2);
    if (dir < 1) {
      pixelBrightness[p] *= -1;
    }
  }
}


void sparkleBarsInit(uint8_t m) {
  midiClearAll();
  detachInterrupt(0);
  attachInterrupt(0, DSBinput, FALLING);
  DSBinput();
  
  for (;;) {
    if (m) {
     // sparkleBarsLoop_sb2012();
    } else {
      sparkleBarsLoop();
    }
    
    if (progBreak) {
      return;
    }
  }
}


void sparkleBarsLoop() {
  midiOutput();
  
  
  // beta: testing receiving drum data from an external source
  /*
  if (Serial.available()) {
    int8_t c = -1;
    byte ch = Serial.read();
    if (ch == 0x52) {
      maxInputAction(0);
    }
    if (ch == 0x59) {
      maxInputAction(1);
    }
    if (ch == 0x42) {
      maxInputAction(2);
    }
    if (ch == 0x47) {
      maxInputAction(3);
    }
  }
  */
  
  
  for (int p=0; p<DSBsize; p++) {
    DSBnextVal(0, p);
    strip.setPixelColor(p, abs(pixelBrightness[p]), 0, 0);
  }
  for (int p=DSBsize; p<DSBsize*2; p++) {
    DSBnextVal(1, p);
    strip.setPixelColor(p, abs(pixelBrightness[p]), abs(pixelBrightness[p]), 0);
  }
  for (int p=DSBsize*2; p<DSBsize*3; p++) {
    DSBnextVal(2, p);
    strip.setPixelColor(p, 0, 0, abs(pixelBrightness[p]));
  }
  for (int p=DSBsize*3; p<DSBsize*4; p++) {
    DSBnextVal(3, p);
    strip.setPixelColor(p, 0, abs(pixelBrightness[p]), 0);
  }
  strip.show();
  
  // fade each color on each cycle
  for (uint8_t c=0; c<4; c++) {
    if (DSBcolorMax[c] > DSBcolorDim) {
      DSBcolorMax[c] = constrain(DSBcolorMax[c] - 4, DSBcolorDim, 127);
    }
  }
  //delay(5);
}


void DSBnextVal(uint8_t c, uint16_t p) {
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
  } else if (abs(pixelBrightness[p]) >= DSBcolorMax[c]) {
    // exceed current max brightness
    pixelBrightness[p] = (DSBcolorMax[c] * -1) + 1;
  } else {
    // normal increment
    pixelBrightness[p]++;
  }
}  


void DSBinput() {
    if (!digitalRead(A0)) {
      DSBinputAction(0);
    }
    if (!digitalRead(A3)) {
      DSBinputAction(1);
    }
    if (!digitalRead(A2)) {
      DSBinputAction(2);
    }
    if (!digitalRead(A1)) {
      DSBinputAction(3);
    }
}


void DSBinputAction(uint8_t c) {
  // each pieze pulse adds a bit of brightness
  // this way, harder hits trigger brighter jumps
  
  // increment new max value
  DSBcolorMax[c] = constrain(DSBcolorMax[c]+6, 0, 127);
  
  // increment each pixel
  for (uint16_t p=c*DSBsize; p<c*DSBsize+DSBsize; p++) {
    pixelBrightness[p] = constrain(abs(pixelBrightness[p]) + 6, 0, 127)  * -1;
  }
  
  midiInput(c);
  setGlobalAction(1);
}


void maxInputAction(uint8_t c) {
  DSBcolorMax[c] = 127;
  for (uint16_t p=c*DSBsize; p<c*DSBsize+DSBsize; p++) {
    pixelBrightness[p] = 127 * -1;
  }
}

