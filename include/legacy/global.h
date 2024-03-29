#pragma once

class Log
{
public:
    static void println(const char *);
};

// #define NUMPIXELS 300

#include <Arduino.h>
#define sprintln(format...) log_i("legacy: %s", format);
#define line (const char *)"------------------------------------------------------------------"