# ESP32 Text-to-Speech (PicoTTS) with LVGL UI

This project demonstrates how to implement PicoTTS system on an ESP32 device using the PicoTTS engine and LVGL for graphical user interface (GUI). The TTS system speaks predefined text when interacting with buttons in a tabbed UI. 


## How It Works

* The project starts with a predefined greeting, "Hello. Welcome back to my project. This is Eric!".

* The main UI is composed of two tabs:
    * Greetings Tab: Contains a list of buttons, each with a predefined greeting. When a button is clicked, the corresponding text is spoken via the TTS engine.
    * E-Book Style Tab: Contains a longer text, and a floating button at the bottom triggers the TTS system to read the long sentence.
* The audio output is handled by the ESP32’s codec, and the TTS engine converts the text into speech using the triggerTTS function.

## Hardware 

* [ESP32-S3-BOX-3](https://github.com/espressif/esp-box)

## Demo

* [Demo Video](https://youtu.be/feHGNeRQMss)
