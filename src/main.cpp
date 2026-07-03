#include <stdio.h>
#include "boxes.h"
#include "display/lv_display.h"
extern "C" {
#include <lvgl.h>
}

#ifdef NATIVE
    #include <SDL2/SDL.h>

// void text_object() {
//     lv_obj_t * label = lv_label_create(lv_screen_active());
//     lv_label_set_text(label, "HI!");

//     lv_obj_set_x(label, 17);
//     lv_obj_set_y(label, 18);
// }

int main() {
    // start the code
    lv_init();

    // temp display window on PC (MUST STAY THE SAME AS IT MODELS THE EXACT PIXEL DIMENSIONS OF THE DISPLAY)
    lv_display_t * disp = lv_sdl_window_create(480, 480);
    lv_obj_t * scr = lv_display_get_screen_active(disp);
    lv_obj_set_style_bg_color(scr, lv_color_make(0,0,0), LV_PART_MAIN);
    

    battery_temp_box();
    break_temp_box();
    motor_temp_box();
    speed_box();
    throttle_box();
    // error_box();

    // text_object();

    // Main LVGL loop to quit program
    bool quit = false;

    while (!quit) {
        SDL_Event e;

        // User requests quit
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_QUIT)
            {
                quit = true;
            }
        }
    
    uint32_t time_till_next = lv_timer_handler();
    SDL_Delay(time_till_next);

    }
    return 0;
}

#endif

#ifdef ROUND_DISPLAY

#include "display_config.h"

static esp_timer_handle_t lvgl_tick_timer;

static void lvgl_tick_cb(void* args) {
    lv_tick_inc(2); // 2ms
}

void disp_flush(lv_display_t *display, const lv_area_t* area, uint8_t *px_map) {
    // LVGL provides the exact coordinates of the redrawn area.
    int x_start = area->x1;
    int y_start = area->y1;
    
    // esp_lcd_panel_draw_bitmap expects the end coordinates to be exclusive (+1)
    int x_end = area->x2 + 1;
    int y_end = area->y2 + 1;

    // Write this specific chunk to the display/hardware buffer
    esp_lcd_panel_draw_bitmap(panel_handle, x_start, y_start, x_end, y_end, px_map);
    
    // Tell LVGL that we are done flushing this area
    lv_display_flush_ready(display);
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println("================================");
    Serial.printf("Flash: %u\n", ESP.getFlashChipSize());
    Serial.printf("PSRAM: %u\n", ESP.getPsramSize());
    Serial.printf("Free PSRAM: %u\n", ESP.getFreePsram());
    Serial.printf("Heap: %u\n", ESP.getFreeHeap());
    Serial.println("================================");

    Serial.begin(9600);

    // start the code
    init_display_rgb();
    init_touch();
    lv_init();

    uint32_t buf_size = LCD_WIDTH * (LCD_HEIGHT/10) * sizeof(uint16_t);
    void* buf1 = (uint16_t *)heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM);
    void* buf2 = (uint16_t *)heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM);
    esp_lcd_rgb_panel_get_frame_buffer(panel_handle, 2, &buf1, &buf2);

    if(!buf1) buf1 = heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if(!buf2) buf2 = heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    if(!buf1) {
        Serial.println("Buffer alloc failed! Free up heap.");
        return;
    }

    // temp display window on PC (MUST STAY THE SAME AS IT MODELS THE EXACT PIXEL DIMENSIONS OF THE DISPLAY)
    lv_display_t * disp = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
    lv_obj_t * scr = lv_screen_active();

    lv_display_set_flush_cb(disp, disp_flush);
    lv_display_set_buffers(disp, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);
    
    setup_splash();

    battery_temp_box();
    break_temp_box();
    motor_temp_box();
    speed_box();
    throttle_box();
    // error_box();



    const esp_timer_create_args_t timer_args = {
        .callback = &lvgl_tick_cb,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "lvgl_tick"
    };
    esp_timer_create(&timer_args, &lvgl_tick_timer);
    esp_timer_start_periodic(lvgl_tick_timer, 2000);

    Serial.println("Setup Complete!");
}


int counter = 0;
void loop() {
    // Main LVGL loop to quit program
    lv_timer_handler();

    delay(5);
}

#endif
