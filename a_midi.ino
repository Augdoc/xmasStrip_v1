// midi needs dedicated serial
// currently using serial1 on mega
// MIDI uses a non-standard baud rate of 31250
// had problems getting softwareSertial to use this baud rate effectively

const int8_t midiLowVol = 32;
const byte midiNoteValue[5] = {0x28, 0x2a, 0x36, 0x3c, 0x33};
int8_t midiVolume[4] = {midiLowVol, midiLowVol, midiLowVol, midiLowVol};


// each piezo pulse adds just a bit of volume
void midiInput(int8_t c) {
  midiVolume[c] = constrain(midiVolume[c] + 16, midiLowVol, 127);
}


void midiOutput() {
  for (int8_t c=0; c<4; c++) {
    if (midiVolume[c] > midiLowVol) {
      //Serial.print(c, DEC);
      //Serial.print(" : ");
      //Serial.println(midiVolume[c], DEC);
      
      midiNoteOn(midiNoteValue[c], midiVolume[c]);
      delayMicroseconds(250);
      midiNoteOff(midiNoteValue[c]);
      midiVolume[c] = midiLowVol;
    }
  }
} 

void midiProgChange() {
  midiClearAll();
  delayMicroseconds(250);
  midiNoteOn(midiNoteValue[4], 0x60);
  delayMicroseconds(250);
  midiNoteOff(midiNoteValue[4]);
}


void midiNoteOn(int pitch, int vel) {
#if defined(__AVR_ATmega2560__)
  Serial1.write(0x90);
  Serial1.write(pitch);
  Serial1.write(vel);
#endif
}

void midiNoteOff(int pitch) {
#if defined(__AVR_ATmega2560__)
  Serial1.write(0x80);
  Serial1.write(pitch);
  Serial1.write(byte(0x00));
#endif
}

void midiClearAll() {
#if defined(__AVR_ATmega2560__)
  Serial1.write(0xB0);
  Serial1.write(0x7B);
  Serial1.write(byte(0x00));

  for (int8_t c=0; c<4; c++) {
    midiVolume[c] = midiLowVol;
  }
#endif
}

