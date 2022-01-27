#include <SoftwareSerial.h>
#include "dac.h"
#include <MIDI.h>


//pin constants
#define MIDI_TX 10
#define MIDI_RX 0
#define LED 3

#define GATE1 1
#define GATE2 2
#define CLK 8
#define CLK_LED 3

#define CLK_PULSE 10
#define CLK_LED_PULSE 50
#define CLK_LED_DIVIDE 4


//dac / midi constants
#define startVoltage 0
#define endVoltage 4000
#define voltageRange (endVoltage - startVoltage)

#define startNote 24
#define endNote 72
#define noteRange (endNote - startNote)


Dac dac;
SoftwareSerial serialMidi(MIDI_RX, MIDI_TX);

byte bufIn[3];

uint8_t curNote = 0;

bool clockEnable = false;
uint8_t clockCount = 0;
uint32_t gateTime = 0;
uint32_t ledTime = 0;





void midiNoteOn(byte channel, byte pitch, byte velocity);
void midiNoteOff(byte channel, byte pitch, byte velocity);

void handleClockPulse(void);
void handleStartClock(void);
void handleStopClock(void);

void setup() {
  pinMode(LED, OUTPUT);

  pinMode(GATE1, OUTPUT);
  pinMode(GATE2, OUTPUT);

  serialMidi.begin(31250);

  dac.initDac();

  dac.writeChannelA(0);
  dac.writeChannelB(0);
}


void loop() {

  if(serialMidi.available() > 0) {
    bufIn[0] = serialMidi.read();
    
    switch(bufIn[0]) {
      case 0xF8:
        handleClockPulse();
        break;
      case 0xFA:
        handleStartClock();
        break;
      case 0xFB:
        handleStartClock();
        break;
      case 0xFC:
        handleStopClock();
        break;
      default:
        while(serialMidi.available() < 2);
        bufIn[1] = serialMidi.read();
        bufIn[2] = serialMidi.read();
        if((bufIn[0] >> 4) == 0x8) {
          midiNoteOff(bufIn[0] & 0x0F, bufIn[1], bufIn[2]);
        }
        else if((bufIn[0] >> 4) == 0x9) {
          midiNoteOn(bufIn[0] & 0x0F, bufIn[1], bufIn[2]);
        }
        //digitalWrite(LED, HIGH);
        //delay(5);
        //digitalWrite(LED, LOW);
        break;
    }
  }
  
  digitalWrite(CLK, (millis() < gateTime) && clockEnable);
  digitalWrite(CLK_LED, millis() < ledTime);
}

void handleClockPulse() {

  //every quarter beat
  if((clockCount % 24) == 0) {
    ledTime = (clockEnable) ? (millis() + CLK_LED_PULSE) : ledTime;
  }

  //every sixteenth
  if((clockCount % 6) == 0) {
    gateTime = millis() + CLK_PULSE;
  }

  clockCount = (clockCount == 23) ? 0 : clockCount + 1;
}

void handleStartClock() {
  clockCount = 0;
  clockEnable = true;
}

void handleStopClock() {
  clockEnable = false;
}


void midiNoteOn(byte channel, byte pitch, byte velocity) {

  if((startNote <= pitch <= endNote) && (channel == 0)) {
  //if((pitch >= startNote) && (pitch <= endNote) && (channel == 0)) {
    digitalWrite(GATE2, HIGH);
    ledTime = millis() + CLK_LED_PULSE;
    
    dac.writeChannelA((pitch - startNote) * (voltageRange / noteRange));
    curNote = pitch;
  }
}

void midiNoteOff(byte channel, byte pitch, byte velocity) {

  if(channel == 0) {
    digitalWrite(GATE2, LOW);
  }
}
