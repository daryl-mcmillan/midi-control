#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

static volatile uint8_t state;
static volatile uint8_t pending_byte;
static volatile uint8_t ready_byte;
static volatile uint8_t ready;

static volatile uint8_t stateMachine[256];

#define BIT_0 0
#define BIT_1 0x80

#define LAST_CAPTURE_BIT_STATE 255
#define CAPTURE_BIT_0 0
#define CAPTURE_BIT_1 1
#define CAPTURE_BIT_2 2
#define CAPTURE_BIT_3 3
#define CAPTURE_BIT_4 4
#define CAPTURE_BIT_5 5
#define CAPTURE_BIT_6 6
#define CAPTURE_BIT_7 7

#define INITIAL 8

#define FIRST_CAPTURE_BYTE_STATE 0
#define CAPTURE_BYTE_0 127

void setup() {
  stateMachine[ INITIAL | BIT_0 ] = INITIAL;
  stateMachine[ INITIAL | BIT_1 ] = INITIAL;

  state = INITIAL;

  // input and pull up on D2 to D7
  DDRD &= 0b00000011;
  PORTD |= 0b11111100;

  // output on port B
  DDRB = 0xFF;

  // timer 1 at 16 MHz, CTC
  TCCR1A = 0;
  TCCR1B = (1<<CS10) | (1<<WGM12);
  TCCR1C = 0;

  // timer 1 interrupt at 171 ( 3 * 31250 Hz )
  OCR1A = 171;
  TIMSK1 |= 1 << OCIE1A;

  sei();
}

ISR(TIMER1_COMPA_vect) {
  PORTB = 0xFF;
  uint8_t sample = PORTD;
  uint8_t ready_bit = 0x80;
  for( uint8_t i = 0; i<4; i++ ) {
    uint8_t bit = sample & 0x80;
    state = stateMachine[ state | bit ];
    if( state <= LAST_CAPTURE_BIT_STATE ) {
      pending_byte = bit | ( pending_byte >> 1 );
    }
    if( state >= FIRST_CAPTURE_BYTE_STATE ) {
      ready_byte = pending_byte;
      ready |= ready_bit;
    }
    sample <<= 1;
    ready_bit >>= 1;
  }
}

int main(void) {

  setup();
  for( ;; ) {
    PORTB = 0x00;
  }
}
