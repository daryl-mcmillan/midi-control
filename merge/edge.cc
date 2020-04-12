#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

static volatile uint8_t count;
static volatile uint8_t buffer[256];

static volatile uint8_t blah;

ISR(TIMER1_COMPA_vect) {
  // never wrap
  if( TCNT1 & 0x8000 ) {
    TCNT1 = 256 << 2;
  }
}

void setup() {
  count = 0;

  // timer 1 a 2 MHz
  TCCR1A = 0;
  TCCR1B = 0b00000010;
  TCCR1C = 0;

  // timer 1 interrupt at 0x8000
  OCR1A = 0x8000;
  TIMSK1 |= 1 << OCIE1A;

  // input, pull up, and interrupts on D2 to D7
  DDRD &= 0b00000011;
  PORTD |= 0b11111100;
  PCMSK2 |= 0b11111100;
  PCICR |= 0b00000100;
  sei();
}

ISR(PCINT2_vect) {
  buffer[ count ] = PIND;
  uint16_t time = TCNT1 >> 2;
  TCNT1 = 0;
  if( time & 0xFF00 ) {
    buffer[ count+1 ] = 255;
  } else {
    buffer[ count+1 ] = time;
  }
  count += 2;
}

class Serial {
  uint8_t time;
  uint8_t last;
  public:
    void tick( const uint8_t t ) {
      uint16_t newTime = time + t;
      if( newTime & 0xFF00 ) {
        time = 0xFF;
      } else {
        time = newTime;
      }
    }
    void update( const uint8_t value ) {
      if( value == last ) {
        return;
      }
      PORTB ^= 0xFF;
      last = value;
    }
};

int main(void) {

  setup();

  // output on port b
  DDRB = 0xFF;

  const uint8_t ticks_per_bit = 16;
  uint8_t index = 0;
  Serial serial;
  Serial serial2;
  Serial serial3;
  Serial serial4;
  for( ;; ) {
    // wait for data
    while( index == count ) {}
    uint8_t sample = buffer[index];
    uint8_t time = buffer[index+1];
    index += 2;
    serial.tick(time);
    serial.update(sample & 0x80);
    sample <<= 1;
    serial2.tick(time);
    serial2.update(sample & 0x80);
    sample <<= 1;
    serial3.tick(time);
    serial3.update(sample & 0x80);
    sample <<= 1;
    serial4.tick(time);
    serial4.update(sample & 0x80);
  }
}
