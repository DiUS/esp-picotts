# PicoTTS

This component provides an ESP-IDF port of the popular PicoTTS Text-to-Speech engine. While Espressif provides an Chinese language TTS, to date there has been no support for other languages. PicoTTS fills this gap, and provides Text-To-Speech for the following languages:

  - English (UK)
  - English (US)
  - German
  - French
  - Italian
  - Spanish

## Requirements

The Text-to-Speech engine is quite resource intensive. While the code size is only around 175KB, language resources occupy another 750-1400KB of flash depending on language, and the engine uses just over 1.1MB of RAM while initialised. As such an ESP32-S3 with sufficient amount of PSRAM and flash is a recommended target.

This component does not provide any board-specific audio support. The TTS engine generates 16bit/16KHz samples, and leaves it to the user to direct those to the correct audio device.

## Performance

On an ESP32-S3 not otherwise occupied, real-time speech generation is possible without any particularly noticeable initial latency. A demo of this component in use can be found via [this blog post](https://dius.com.au/machine-learning-on-the-edge-speech-command-recognition/), down towards the bottom.

## Getting started

Using the PicoTTS component is straight forward. Effectively the steps are:

  - Initialise the engine
  - Register a callback function to receive the speech samples
  - Register an idle callback (optional)
  - Register an error callback (optional)
  - Send text to the engine
  - Eventually, shut down the engine

In code, this can look like:
```
  #include "picotts.h"

  #define TTS_TASK_PRIORITY 5
  #define TTS_CORE 1

  void my_sample_cb(int16_t *buf, unsigned count)
  {
    esp_codec_dev_write(speaker_codec_dev, buf, count*2);
  }

  void my_tts_done_cb(void)
  {
    // E.g. resume listening for commands
  }

  void my_tts_error_cb(void)
  {
    // Awooga! Need to schedule a shutdown+initialise to recover if this happens
    // ...
  }

  if (picotts_init(TTS_TASK_PRIORITY, my_sample_cb, TTS_CORE))
  {
    picotts_set_idle_notify(my_tts_done_cb);
    picotts_set_error_notify(my_tts_error_cb);

    static const msg[] = "Hello, world";
    picotts_add(msg, sizeof(msg)); // Include the \0 to tell TTS to go

    // Do other stuff, or at least wait until the msg has been spoken

    picotts_shutdown();
  }
```

API documentation can be found in the [picotts.h](include/picotts.h) header file.

## Resource handling

The PicoTTS engine relies on two resource blobs, a Text Analysis (TA) resource and a Signal Generator (SG) resource. In upstream PicoTTS, these are loaded into RAM from files on disk. As RAM is a very precious resource on a microcontroller, this component has replaced the resource loading routines such that they can be accessed directly from memory-mapped flash instead. This reduces the RAM foot-print from 2.5MB down to 1.1MB.

There are two options on how to bundle the resource files onto flash. The default, and arguably the easiest, is to embed the resource files directly into the application binary. The one downside to this approach is that application size grows significantly, and may present an issue with firmware upgrades. You will definitely use a much larger application partition than usual. Alternatively, the resource files can be placed in dedicated flash partitions and accessed from there instead. The advantage with this approach is that the language resources are no longer directly coupled to the application binary. Which approach is best will depend on the specific project circumstances.

To facilitate this type of resource usage the model loading functions of PicoTTS have been wrapped/replaced (see `esp_picorsrc.c`). The source in the `pico/` directory is unmodified, and is the pristine upstream source.

### Custom paritions for language resources

When this component is configured to load its language resources from partitions rather than having them directly embedded into the application binary itself, you will need to add partition entries to hold the Text Analysis (TA) and Signal Generator (SG) resources. Example entries for `partitions.csv`:

```
picotts_ta, data, undefined,   ,        640K,
picotts_sg, data, undefined,   ,        820K,
```

You are free to use any valid partition type and subtype. This component loads
purely by the partition name. The partition names may be changed via Kconfig if so desired.

The partition sizes may be shrunk to better match the language you're using. What's show here are the maximum partition sizes to fit any language bundle.

## Examples

The [boot\_greeting](examples/boot_greeting/README.md) example is written for ESP-BOX and uses this component to issue a greeting upon boot.
