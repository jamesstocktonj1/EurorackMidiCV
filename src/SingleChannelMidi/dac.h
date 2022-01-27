#include <SPI.h>

#define CS 7
#define SCK 4
#define MISO 5
#define MOSI 6

class Dac {
  public:
  

  void initDac() {
    pinMode(CS, OUTPUT);
    digitalWrite(CS, HIGH);

    SPI.begin();
  }

  void writeChannelA(uint16_t v) {
    digitalWrite(CS, LOW);
    SPI.transfer16(0x1000 | v);
    digitalWrite(CS, HIGH);
  }

  void writeChannelB(uint16_t v) {
    digitalWrite(CS, LOW);
    SPI.transfer16(0x9000 | v);
    digitalWrite(CS, HIGH);
  }
};
