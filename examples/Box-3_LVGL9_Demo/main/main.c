#include "esp_log.h"
#include "bsp/esp-bsp.h"
#include "lvgl.h"
#include "picotts.h"
#include <string.h>
#include "esp_log.h"

#define TTS_CORE 1

static esp_codec_dev_handle_t spk_codec;

const char greeting[] = "Hello. Welcome back to my project. This is Eric! You are listening to the voice of TTS. I love this so much.\0";
const char* greetings[] = {
    "Hello.\0", 
    "Good Morning.\0", 
    "Good Bye.\0",
    "Good Night.\0",
    "How Are you?\0",
    "Nice to see you.\0",
    "Have a nice day.\0",
    "I have to go.\0",
    "Talk to you soon.\0",
    "I'm using this device to speak.\0",
    "See you later.\0",
    "How was your day?\0",
    "How was your weekend?\0",
    "How was work?\0",
    "What are you doing?\0",
};
const char* long_sentence = "There is lots of ways to be, as a person. And some people express their deep appreciation in different ways. But one of the ways that I believe people express their appreciation to the rest of humanity is to make something wonderful and put it out there.\0";        

static lv_disp_t *display;
static lv_obj_t * currentButton = NULL;

static void on_samples(int16_t *buf, unsigned count)
{
  esp_codec_dev_write(spk_codec, buf, count*2);
}

void triggerTTS(const char * text){
  printf("Task says: %s\n", text);  // Use the text
  const size_t length = strlen(text) + 1;  // +1 for null terminator
  char arr[length];  // Allocate array large enough to hold the string
  strcpy(arr, text);  // Copy the string to 
  picotts_add(arr, sizeof(arr));
  vTaskDelay(pdMS_TO_TICKS(2000));
}

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = (lv_obj_t *)lv_event_get_target(e);
    lv_obj_t * list1 = (lv_obj_t *)lv_event_get_user_data(e);

    if(code == LV_EVENT_CLICKED) {
        if(list1 != NULL){
            if(currentButton == obj) {
                currentButton = NULL;
            }
            else {
                currentButton = obj;
            }
            lv_obj_t * parent = lv_obj_get_parent(obj);
            for(uint32_t i = 0; i < lv_obj_get_child_count(parent); i++) {
                lv_obj_t * child = lv_obj_get_child(parent, i);
                if(child == currentButton) {
                    lv_obj_add_state(child, LV_STATE_CHECKED);
                    lv_obj_remove_flag(child, LV_OBJ_FLAG_CLICKABLE);
                    triggerTTS(lv_list_get_button_text(list1, obj));
                }
                else {
                    lv_obj_remove_state(child, LV_STATE_CHECKED);
                    lv_obj_add_flag(child, LV_OBJ_FLAG_CLICKABLE);  
                }
            }
        }else{
            triggerTTS(long_sentence);
        }
    }
}

void lv_ui_menu(void)
{
    static lv_style_t style_base;
    lv_style_init(&style_base);
    lv_style_set_border_width(&style_base, 0);
    
    lv_obj_t *scr = lv_scr_act();
    lv_obj_add_style(scr, &style_base, LV_PART_MAIN);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    lv_obj_t * header_label = lv_label_create(scr);
    lv_label_set_text(header_label, "  Text-to-Speech (TTS)");
    lv_obj_align(header_label, LV_ALIGN_TOP_LEFT, 0, 0);

    LV_IMAGE_DECLARE(us_flag);
    lv_obj_t * lang_flag = lv_image_create(scr);
    lv_image_set_src(lang_flag, &us_flag);
    lv_obj_align(lang_flag, LV_ALIGN_TOP_RIGHT, -8, 1);

    lv_obj_t * lang_label = lv_label_create(scr);
    lv_label_set_text(lang_label, " English (US) ");
    lv_obj_align_to(lang_label, lang_flag, LV_ALIGN_OUT_LEFT_MID, 0, 0);

    lv_obj_t * tabview;
    tabview = lv_tabview_create(scr);
    lv_tabview_set_tab_bar_position(tabview, LV_DIR_LEFT);
    lv_tabview_set_tab_bar_size(tabview, 80);
    lv_obj_align(tabview, LV_ALIGN_TOP_LEFT, 0, 20);
    lv_obj_set_height(tabview, 220);
    lv_obj_set_style_bg_color(tabview, lv_palette_lighten(LV_PALETTE_RED, 2), 0);

    lv_obj_t * tab_buttons = lv_tabview_get_tab_bar(tabview);
    lv_obj_set_style_bg_color(tab_buttons, lv_palette_darken(LV_PALETTE_GREY, 3), 0);
    lv_obj_set_style_text_color(tab_buttons, lv_palette_lighten(LV_PALETTE_GREY, 5), 0);
    lv_obj_set_style_border_side(tab_buttons, LV_BORDER_SIDE_RIGHT, LV_PART_ITEMS | LV_STATE_CHECKED);

    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Greetings");
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "E-Book\nStyle");
    lv_obj_set_style_bg_color(tab1, lv_palette_lighten(LV_PALETTE_DEEP_ORANGE, 3), 0);
    lv_obj_set_style_bg_opa(tab1, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(tab2, lv_palette_lighten(LV_PALETTE_INDIGO, 3), 0);
    lv_obj_set_style_bg_opa(tab2, LV_OPA_COVER, 0);
    lv_obj_remove_flag(lv_tabview_get_content(tabview), LV_OBJ_FLAG_SCROLLABLE);

    // Tab1
    lv_obj_t * list1 = lv_list_create(tab1);
    lv_obj_set_size(list1, lv_pct(100), lv_pct(100));
    lv_obj_set_style_pad_row(list1, 5, 0);
    int numGreetings = sizeof(greetings) / sizeof(greetings[0]);
    for (int i = 0; i < numGreetings; i++) {
        lv_obj_t * btn = lv_button_create(list1);
        lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, list1);
        lv_obj_t * lab = lv_label_create(btn);
        if(strlen(greetings[i]) > 20) lv_obj_set_width(lab, 150);
        lv_label_set_text(lab, greetings[i]);
        lv_label_set_long_mode(lab, LV_LABEL_LONG_SCROLL);
    }

    // Tab2 
    lv_obj_t * spans = lv_spangroup_create(tab2);
    lv_obj_set_size(spans, lv_pct(100), lv_pct(100));
    lv_spangroup_set_align(spans, LV_TEXT_ALIGN_LEFT);

    lv_span_t * span = lv_spangroup_new_span(spans);
    lv_span_set_text(span, long_sentence);
    lv_style_set_text_color(lv_span_get_style(span), lv_color_black());
    lv_style_set_text_decor(lv_span_get_style(span), LV_TEXT_DECOR_UNDERLINE);

    // Create floating btn
    lv_obj_t * float_btn = lv_button_create(tab2);
    lv_obj_set_size(float_btn, 48, 48);
    lv_obj_add_flag(float_btn, LV_OBJ_FLAG_FLOATING);
    lv_obj_align(float_btn, LV_ALIGN_BOTTOM_RIGHT, -6, -6);
    lv_obj_add_event_cb(float_btn, event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_radius(float_btn, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_image_src(float_btn, LV_SYMBOL_PLAY, 0);
    lv_obj_set_style_text_font(float_btn, lv_theme_get_font_large(float_btn), 0);
}

static void app_lvgl_display(void)
{
    bsp_display_lock(0);
    lv_ui_menu();    
    bsp_display_unlock();
}

void app_main(void)
{
  ESP_ERROR_CHECK(bsp_i2c_init());
  ESP_ERROR_CHECK(bsp_audio_init(NULL));
  spk_codec = bsp_audio_codec_speaker_init();
  assert(spk_codec);

  esp_codec_dev_set_out_vol(spk_codec, 100);
  esp_codec_dev_sample_info_t fs = {
      .sample_rate = PICOTTS_SAMPLE_FREQ_HZ,
      .channel = 1,
      .bits_per_sample = PICOTTS_SAMPLE_BITS,
  };
  esp_codec_dev_open(spk_codec, &fs);

  display = bsp_display_start();
  bsp_display_backlight_on();
  app_lvgl_display();

 unsigned prio = uxTaskPriorityGet(NULL);
  if (picotts_init(prio, on_samples, TTS_CORE))
  {
    picotts_add(greeting, sizeof(greeting));
    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}