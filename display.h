#ifndef HEADER_INCLUDED_DISPLAY
#define HEADER_INCLUDED_DISPLAY

#include <stdio.h>

char display_framebuffer[4][20] = { ' ' };

void display_init();
void display_set_line(char line, char data[]);
void display_refresh();

#define display_sprintf(line, ...) sprintf(display_framebuffer[line], __VA_ARGS__)

#endif