#ifndef HEADER_INCLUDED_GPSCLOCK
#define HEADER_INCLUDED_GPSCLOCK

struct gpsclock_status_t {
    char tracking;
    char verified_2kpps;
};

struct system_clock_t {
    uint32_t microsecond;
    uint8_t  second;
    uint8_t  minute;
    uint8_t  hour;
    uint8_t  day;
    uint8_t  month;
    uint16_t year;
};
struct system_clock_t SYSTEM_CLOCK = {0,0,0,0,1,1,1970};
struct system_clock_t SYSTEM_CLOCK_NEXT_OVERRIDE;
char   gpsclock_override_on_next_1pps = 0;

void gpsclock_init(void);
char gpsclock_interrupt(void);
struct gpsclock_status_t gpsclock_status(void);
void gpsclock_increase_by_microseconds(uint16_t delta);

uint16_t PLL_PRELOAD = 6000;

#endif