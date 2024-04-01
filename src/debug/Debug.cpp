#include "Debug.h"

void Debug::debugHeapTask(void *pv) {
    for (;;) {
        log_i("free heap: %i", esp_get_free_heap_size());
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void Debug::startDebugHeapTask() {
    xTaskCreate(&debugHeapTask,
                "heap_debug",
                256,
                nullptr,
                0,
                nullptr);
}
