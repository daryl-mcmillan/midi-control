#include <avr/io.h>

.data
.global next_index
.global buffer
.global stateMachine

.balign 256
buffer: .space 256
stateMachine: .space 256
next_index: .space 1

.text
.global TIMER1_COMPA_vect
TIMER1_COMPA_vect:
  push r1
  push r30
  push r31
  ldi r31, hi8(buffer)
  lds r30, next_index
  in r1, _SFR_IO_ADDR(PIND)
  st Z+, r1
  sts next_index, r30
  pop r31
  pop r30
  pop r1
  reti
