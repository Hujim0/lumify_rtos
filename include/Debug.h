#ifndef LUMIFY_RTOS_DEBUG_H
#define LUMIFY_RTOS_DEBUG_H

#include "Arduino.h"


class Debug {
public:
    static void debugHeapTask(void*pv);
    static void startDebugHeapTask();
};


#endif //LUMIFY_RTOS_DEBUG_H
