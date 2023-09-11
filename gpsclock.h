#ifndef HEADER_INCLUDED_GPSCLOCK
#define HEADER_INCLUDED_GPSCLOCK

void gpsclock_init(void);
char gpsclock_interrupt(void);
uint16_t gpsclock_pll_freq(void);

#endif