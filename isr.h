#ifndef HEADER_INCLUDED_ISR
#define HEADER_INCLUDED_ISR
void __interrupt(high_priority) h_isr(void);
void __interrupt(low_priority) l_isr(void);
#endif
