# PicoTTS Boot Greeting Example

This example is for ESP-BOX, and demonstrates how to initialise and use the PicoTTS component.

When booting, the ESP-BOX will issue a spoken greeting.

The example uses a minimal Board Support Package (BSP) derived from the official Espressif [esp-bsp repo](https://github.com/espressif/esp-bsp/tree/master/bsp/esp-box). Porting the example to other boards is hopefully pretty easy.

## Configuration

The greeting message can be customised via Kconfig, as can the volume.

## Building and flashing

The default picotts component configuration is to embed the language resource files into the binary, so to build and flash you only need to:

```
idf.py build
idf.py flash
```

The partition table in this example also makes allowance for having the picotts language resource files stored in separate partitions.

As usual, the console log can be accessed with `idf.py monitor`.
