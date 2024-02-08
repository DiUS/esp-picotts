#include "picotts.h"
#include "bsp/esp-bsp.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>

#define TTS_CORE 1

const char greeting[] = CONFIG_BOOT_GREETING_MSG;

static esp_codec_dev_handle_t spk_codec;

static void on_samples(int16_t *buf, unsigned count)
{
  esp_codec_dev_write(spk_codec, buf, count*2);
}


void app_main()
{
  ESP_ERROR_CHECK(bsp_i2c_init());

  ESP_ERROR_CHECK(bsp_audio_init(NULL));

  spk_codec = bsp_audio_codec_speaker_init();
  assert(spk_codec);
  esp_codec_dev_set_out_vol(spk_codec, CONFIG_BOOT_GREETING_VOLUME);
  esp_codec_dev_sample_info_t fs = {
    .sample_rate = PICOTTS_SAMPLE_FREQ_HZ,
    .channel = 1,
    .bits_per_sample = PICOTTS_SAMPLE_BITS,
  };
  esp_codec_dev_open(spk_codec, &fs);

  unsigned prio = uxTaskPriorityGet(NULL);
  if (picotts_init(prio, on_samples, TTS_CORE))
  {
    picotts_add(greeting, sizeof(greeting));

    vTaskDelay(pdMS_TO_TICKS(10000));

    picotts_shutdown();
  }
  else
    printf("Failed to initialise TTS\n");

  esp_codec_dev_close(spk_codec);
}
