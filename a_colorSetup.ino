// pixel storage
int8_t pixelBrightness[NPIX];
boolean pixelVal[NPIX];


// basic colors
const uint32_t colorIdle = 0x909090;
const uint32_t colorBlack = 0x808080;

const uint32_t colorRed = 0x80FF80;
const uint32_t colorOrange = 0xC0FF80;
const uint32_t colorYellow = 0xFFFF80;
const uint32_t colorGreen = 0xFF8080;
const uint32_t colorBlue = 0x8080FF;
const uint32_t colorViolet = 0x80FFFF;

const uint32_t colorTeal = 0xFF80FF;
const uint32_t colorGreeny = 0xFFC080;
const uint32_t colorWhite = 0xFFFFFF;


// four-bit colors (rgby)
uint8_t fourBitHits = 0;

uint32_t fourBitColors[16] = {
  colorBlack,
  colorRed,
  colorGreen,
  colorWhite,
  colorBlue,
  colorViolet,
  colorTeal,
  colorWhite,
  colorYellow,
  colorOrange,
  colorGreeny,
  colorWhite,
  colorGreen,
  colorWhite,
  colorWhite,
  colorWhite
};

uint32_t xMas4Colors[4] = {
  colorRed,
  colorYellow,
  colorGreen,
  colorBlue
};

uint32_t xMas7Colors[7] = {
  colorRed,
  colorYellow,
  colorBlue,
  colorOrange,
  colorGreen,
  colorViolet,
  1
};



// read drum input as colors
// combined hits within a "cycle" can "mix" colors
void fourBitInput() {
  if (!digitalRead(A0)) {
    fourBitHits |= B0001;
    midiInput(0);
    setGlobalAction(1);
    //Serial.println(">R");
  }
  if (!digitalRead(A1)) {
    fourBitHits |= B0010;
    midiInput(3);
    setGlobalAction(1);
    //Serial.println(">G");
  }
  if (!digitalRead(A2)) {
    fourBitHits |= B0100;
    midiInput(2);
    setGlobalAction(1);
    //Serial.println(">B");
  }
  if (!digitalRead(A3)) {
    fourBitHits |= B1000;
    midiInput(1);
    setGlobalAction(1);
    //Serial.println(">Y");
  }
}



// reset strip
void clearStrip() {
  for (uint16_t p=0; p<NPIX; p++) {
    strip.setPixelColor(p, colorBlack);
  }
  strip.show();
}



// ADA RGB wheel library
uint32_t Wheel(uint16_t WheelPos) {
  byte r, g, b;
  switch(WheelPos / 128) {
    case 0:
      r = 127 - WheelPos % 128;		//Red down
      g = WheelPos % 128;			 // Green up
      b = 0;									//blue off
      break; 
    case 1:
      g = 127 - WheelPos % 128;	 //green down
      b = WheelPos % 128;			 //blue up
      r = 0;									//red off
      break; 
    case 2:
      b = 127 - WheelPos % 128;	 //blue down 
      r = WheelPos % 128;			 //red up
      g = 0;									//green off
      break; 
  }
  return(strip.Color(r,g,b));
}

