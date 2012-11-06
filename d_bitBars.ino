// DRUM MODE
// each drum hit adds a "building block"


//sparkle mode data
uint16_t DBBsize = int(NPIX / 4);

uint8_t DBBwhiteDim = 4;
uint8_t DBBwhiteMax = 64;
uint8_t DBBcolorDim = 32;
uint8_t DBBcolorMax = 120;
uint8_t DBBcolorStep = 4;
uint8_t DBBcolorHue[161];
uint8_t DBBbarPos = 0;



void bitBarsSetup() {
  for (uint16_t b=0; b<DBBsize; b++) {
    DBBcolorHue[b] = 15;
  }
  
  DBBbarPos = DBBsize;
  
  for (uint16_t p=0; p<NPIX; p++) {
    pixelBrightness[p] = random(DBBwhiteDim, DBBwhiteMax);
    uint8_t dir = random(2);
    if (dir < 1) {
      pixelBrightness[p] *= -1;
    }
  }
}


void bitBarsInit() {
  midiClearAll();
  detachInterrupt(0);
  attachInterrupt(0, fourBitInput, FALLING);
  fourBitInput();
  
  bitBarsSetup();
  
  for (;;) {
    bitBarsLoop();
    if (progBreak) {
      return;
    }
  }
}


void bitBarsLoop() {
  midiOutput();
  
  if (fourBitHits > 0) {
    DBBbarPos++;
    if (DBBbarPos >= DBBsize) {
      DBBbarPos = 0;
    }
    DBBcolorHue[DBBbarPos] = fourBitHits;
    DBBcolorHue[(DBBbarPos+1)] = 15;  //colorWhite
    fourBitHits = 0;
  } else {
    //for (int p=0; p<NPIX; p++) {
    //  pixelNextVal(p, DBBcolorDim, DBBcolorMax, DBBcolorStep);
    //  strip.setPixelColor(p, pixelSetHue(fourBitColors[DBBcolorHue[p/4]], pixelBrightness[p]));
    //}
    for (uint16_t b=0; b<DBBsize; b++) {
      pixelNextVal(b, DBBcolorDim, DBBcolorMax, DBBcolorStep);
      
      uint32_t c = pixelSetHue(fourBitColors[DBBcolorHue[b]], pixelBrightness[b]);
      uint16_t p = b*4;
      
      for (uint8_t i=0; i<4; i++) {
        strip.setPixelColor(p+i, c);
      }
    }
  }
  strip.show();
  
  //delay(5);  
}

