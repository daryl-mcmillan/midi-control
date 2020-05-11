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

typedef void Action();

template<Action action>
class OverflowTimer {
  volatile uint16_t val;
  volatile uint16_t step;
    public:
    OverflowTimer() {
      val = 0;
      step = 0;
    }
    void tick() {
      uint16_t tempval = val + step;
      if( tempval < val ) {
        action();
      }
      val = tempval;
    }
    void setFreq( uint16_t f ) {
      val = 0;
      step = f * 4;
    }
};

template<uint8_t bits> void PORTB_toggle() {
  PORTB ^= bits;
}

void toggle0() { PORTB_toggle<1>(); }
void toggle1() { PORTB_toggle<2>(); }
void toggle2() { PORTB_toggle<4>(); }
void toggle3() { PORTB_toggle<8>(); }
void toggle4() { PORTB_toggle<16>(); }
void toggle5() { PORTB_toggle<32>(); }

static OverflowTimer<toggle0> timer1;
static OverflowTimer<toggle1> timer2;
static OverflowTimer<toggle2> timer3;
static OverflowTimer<toggle3> timer4;
static OverflowTimer<toggle4> timer5;
static OverflowTimer<toggle5> timer6;

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
