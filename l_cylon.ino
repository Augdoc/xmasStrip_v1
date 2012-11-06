// LIGHT MODE
// the ever-popular larsen scanner
// in standard red, or in rainbow


// cylon mode
int LCYcenterPos = 0;
int LCYcenterDir = 1;
uint16_t LCYwheelPos = 0;
uint8_t LCYcolorValues[28] = {127, 96, 72, 64, 56, 48,44, 40, 36, 32, 28, 24, 22, 20, 18, 16, 14, 12, 10, 8, 7, 6, 5, 4, 3, 2, 1, 0};
uint8_t LCYcolorSize = 28;  


void doCylon(uint8_t m) {
  switch (m) {
    case 1: currentProgTime = 60; break;
    default: currentProgTime = 30; LCYwheelPos = 0; break;
  };
  
  // set
  for (int i=(LCYcolorSize-1); i>=0; i--) {
    int myPos = LCYcenterPos - (i*LCYcenterDir);
    if (myPos < 0) {
      myPos = 0 - myPos;
    }
    if (myPos > (NPIX-1)) {
      myPos = ((NPIX-1)*2) - myPos;
    }
    
    if (m  &&  LCYcolorValues[i]==0) {
      strip.setPixelColor(myPos, 1,1,1);
    } else {
      strip.setPixelColor(myPos, pixelSetHue(Wheel(LCYwheelPos), LCYcolorValues[i]));
    }
  }
  
  strip.show();
  
  
  // increment position
  if (LCYcenterDir > 0) {
    LCYcenterPos++;
    if (LCYcenterPos > (NPIX-1)) {
      LCYcenterDir = -1;
      LCYcenterPos = NPIX - 2;
    }
  } else {
    LCYcenterPos--;
    if (LCYcenterPos < 0) {
      LCYcenterDir = 1;
      LCYcenterPos - 1;
    }
  }
  
  
  // increment color
  if (m  &&  !(LCYcenterPos%4)) {
    LCYwheelPos++;
    if (LCYwheelPos >= 384) {
      LCYwheelPos = 0;
    }
  }
  
  //delay(1);
}

