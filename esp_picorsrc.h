#ifndef ESP_PICORSRC_H
#define ESP_PICORSRC_H

#include "picorsrc.h"

pico_status_t esp_pico_loadResource(pico_System sys, const void *raw, pico_Resource *resource);

pico_status_t esp_pico_unloadResource(pico_System sys, pico_Resource *inResource);

#endif
