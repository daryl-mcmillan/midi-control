#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void setup() {

  // timer 1 at 16 MHz, CTC
  TCCR1A = 0;
  TCCR1B = (1<<CS10) | (1<<WGM12);
  TCCR1C = 0;

  // timer 1 interrupt at 244 ( 32768 Hz )
  OCR1A = 488;
  TIMSK1 |= 1 << OCIE1A;

  sei();
}

class OverflowTimer {
  volatile uint16_t val;
  volatile uint16_t step;
  uint8_t toggle;
    public:
    OverflowTimer(uint8_t mask) {
      val = 0;
      step = 0;
      toggle = mask;
    }
    void tick() {
      uint16_t tempval = val + step;
      if( tempval < val ) {
        PORTB ^= toggle;
      }
      val = tempval;
    }
    void setFreq( uint16_t f ) {
      val = 0;
      step = f * 4;
    }
};

static OverflowTimer timer1(1), timer2(2), timer3(4), timer4(8), timer5(16), timer6(32);

ISR(TIMER1_COMPA_vect) {
  PORTC = 0xFF;
  timer1.tick();
  timer2.tick();
  timer3.tick();
  timer4.tick();
  timer5.tick();
  timer6.tick();
}

int main(void) {

  DDRB = 0xFF;
  DDRC = 0xFF;
  PORTB = 0x00;

  setup();

  timer1.setFreq( 131 );
  timer2.setFreq( 147 );
  timer3.setFreq( 164 );
  timer4.setFreq( 175 );
  timer5.setFreq( 196 );
  timer6.setFreq( 220 );
  for( ;; ) {
    PORTC = 0x00;
  }
}
