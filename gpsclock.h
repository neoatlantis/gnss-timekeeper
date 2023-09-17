#ifndef HEADER_INCLUDED_GPSCLOCK
#define HEADER_INCLUDED_GPSCLOCK

struct gpsclock_status_t {
    char tracking;
    char verified_2kpps;
};

struct system_clock_t {
    int32_t microsecond;
    int8_t  second;
    int8_t  minute;
    int8_t  hour;
    int8_t  day;
    int8_t  month;
    int16_t year;
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