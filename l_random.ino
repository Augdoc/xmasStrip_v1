// LIGHT MODE
// "random" fill modes


void randomBuild(uint8_t m) {
  currentProgTime = 120;
  uint16_t sizes[11] = {  1,  2,  4,  8, 16, 32, 40, 80,160, 320, 640};
  uint16_t waits[11] = {100,160,190,230,270,320,400,520,760,1000,1500};
  
  for (uint8_t s=0; s<11; s++) {
    for (uint8_t i=1; i<(map(s, 0,10, 48,6)); i++) {
      switch (m) {
        case 1: primaryRandom(sizes[s]); break;
        default: completelyRandom(sizes[s]); break;
      };
      
      delay(waits[s]);
      if (progBreak) {
        return;
      }
    }
  }
}


void primaryRandom(uint16_t size) {
  for (uint16_t p=0; p<NPIX; p+=size) {
    uint32_t c = Wheel(random(384));
    for (uint16_t i=0; i<size; i++) {
      strip.setPixelColor(p+i, c);
    }
  }
  strip.show();
  strip.show();
}

    
void completelyRandom(uint16_t size) {
  uint32_t c;
  
  for (uint16_t p=0; p<NPIX; p+=size) {
    c = getRandomColor(288);
    for (uint16_t i=0; i<size; i++) {
      strip.setPixelColor(p+i, c);
    }
  }
  strip.show();
  strip.show();
}


uint32_t getRandomColor(uint16_t sat) {
  uint8_t y = 0;
  uint8_t r = 128;
  uint8_t g = 128;
  uint8_t b = 128;
   
  // attempt to limit "whiteness" of random numbers
  while (!y || (r+g+b > sat)) {
    y++;
    r = random(128);
    g = random(128);
    b = random(128);
  }
  
  return strip.Color(r, g, b);
}

