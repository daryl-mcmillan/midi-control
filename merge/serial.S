#include <avr/io.h>

.data
.balign 256
.global buffer
buffer: .space 256
.global stateMachine
stateMachine: .space 256

.text
.global TIMER1_COMPA_vect
TIMER1_COMPA_vect:
  reti
