// LIGHT MODE
// xmas sparkle and random fill modes


uint8_t LXMcolorDim = 7;
uint8_t LXMcolorMax = 127;


void xmas4Static() {
  for (uint16_t p=0; p<NPIX; p+=8) {
    strip.setPixelColor(p, colorRed);
    strip.setPixelColor(p+1, colorBlack);
    strip.setPixelColor(p+2, colorYellow);
    strip.setPixelColor(p+3, colorBlack);
    strip.setPixelColor(p+4, colorGreen);
    strip.setPixelColor(p+5, colorBlack);
    strip.setPixelColor(p+6, colorBlue);
    strip.setPixelColor(p+7, colorBlack);
  }
  strip.show();
  delay(50);
}


void xmas4SparkleInit() {
  for (uint16_t p=0; p<NPIX; p++) {
    pixelBrightness[p] = random(LXMcolorDim, LXMcolorMax);
    pixelVal[p] = random(2);
    uint8_t dir = random(2);
    if (dir < 1) {
      pixelBrightness[p] *= -1;
    }
  }
  
  for (;;) {
    xmas4Sparkle();
    delay(5);
    if (progBreak) {
      return;
    }
  }

}  

void xmas4Sparkle() {
  //uint32_t debugStartTime = micros();
  
  for (uint16_t p=0; p<NPIX; p+=2) {
    pixelNextVal(p, LXMcolorDim, LXMcolorMax, 4);
    if (pixelVal[p]) {
      switch (p % 8) {
        case 0: strip.setPixelColor(p, abs(pixelBrightness[p]), 0, 0); break;
        case 2: strip.setPixelColor(p, abs(pixelBrightness[p]), abs(pixelBrightness[p]), 0); break;
        case 4: strip.setPixelColor(p, 0, abs(pixelBrightness[p]), 0); break;
        case 6: strip.setPixelColor(p, 0, 0, abs(pixelBrightness[p])); break;
      };
      strip.setPixelColor(p+1, 0,0,0);
    } else {
      switch (p % 8) {
        case 0: strip.setPixelColor(p+1, abs(pixelBrightness[p]), 0, 0); break;
        case 2: strip.setPixelColor(p+1, abs(pixelBrightness[p]), abs(pixelBrightness[p]), 0); break;
        case 4: strip.setPixelColor(p+1, 0, abs(pixelBrightness[p]), 0); break;
        case 6: strip.setPixelColor(p+1, 0, 0, abs(pixelBrightness[p])); break;
      };
      strip.setPixelColor(p, 0,0,0);
    }
  }
  strip.show();
  
  //uint32_t debugRunTime = micros() - debugStartTime;
 // Serial.println(debugRunTime);
}


void xmas4Wipe_x() {
  for (uint8_t c=0; c<4; c++) {
    for (uint16_t p=0; p<NPIX; p++) {
      strip.setPixelColor(p, xMas4Colors[c]);
      strip.show();
      delay(12);
    }
  }
}


void xmas4Wipe() {
  for (uint8_t c=0; c<4; c++) {
    randomWipe(xMas4Colors[c], 15);
    progDelay(1000);
    randomWipe(colorBlack, 0);
  }
}

void xmas7Wipe() {
  currentProgTime = 105;
  for (uint8_t c=0; c<7; c++) {
    randomWipe(xMas7Colors[c], 15);
    progDelay(1000);
    randomWipe(colorBlack, 0);
  }
}

void randomFill() {
  randomWipe(0, 15);
  progDelay(1000);
  randomWipe(colorBlack, 0);
}


void randomWipe(uint32_t c, uint8_t ms) {
  uint16_t i;
  uint16_t j;
  uint16_t p;
  
  uint16_t tList[NPIX];
  uint16_t tSize;
  
  // "reset" full array
  for (i=0; i<NPIX; i++) {
    tList[i] = i;
  }
  tSize = NPIX;
  
  // each pixel
  for (i=0; i<NPIX; i++) {
    p = random(tSize);
    switch (c) {
      case 0: strip.setPixelColor(tList[p], Wheel(random(384)) ); break;
      case 1: strip.setPixelColor(tList[p], Wheel(map(tList[p], 0,159, 0,383)) ); break;
      default: strip.setPixelColor(tList[p], c); break;
    }
    strip.show();
    
    // remove used element from array
    boolean splice = false;
    for (j=0; j<NPIX; j++) {
      if (j == p) {
        splice = true;
      }
      if (splice) {
        tList[j] = tList[j+1];
      }
    }
    tSize--;
    
    delay(ms);
    if (progBreak) {
      return;
    }
  }
}



