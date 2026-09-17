#include <stdio.h>
#include "boxes.h"
#include "display/lv_display.h"
extern "C" {
#include <lvgl.h>
}

box_widget* battery_temp_box;
box_widget* brake_temp_box;
box_widget* motor_temp_box;
box_widget* speed_box;
box_widget* throttle_box;
box_widget* throttle_fill;

uint32_t size_w = 0;
bool shrink = false;

void Create_UI() {
    setup_splash();

    battery_temp_box = new box_widget(lv_screen_active(), 80, 80, 100, 336);
    brake_temp_box = new box_widget(lv_screen_active(), 80, 80, 200, 336);
    motor_temp_box = new box_widget(lv_screen_active(), 80, 80, 300, 336);
    speed_box = new box_widget(lv_screen_active(), 150, 300, 0, 0, true);
    throttle_box = new box_widget(lv_screen_active(), 90, 300, 90, 50);
    throttle_fill = new box_widget(throttle_box->get_lv_obj(), 88, 5, -15, -17);

    {
        lv_obj_t* throttle_fill_obj = throttle_fill->get_lv_obj();

        lv_obj_set_style_bg_color(throttle_fill_obj, lv_color_make(225, 150, 90), LV_PART_MAIN);

        lv_obj_set_style_outline_color(throttle_fill_obj, lv_color_make(225, 150, 90), LV_PART_MAIN);
        lv_obj_set_style_outline_width(throttle_fill_obj, 0, LV_PART_MAIN);
    }

    size_w = throttle_fill->get_size().width;
}
int32_t angle = 0;
void Update_UI() {
    size_w = size_w + (shrink ? -1 : 1);

    if(size_w > throttle_box->get_size().width - 3)
        shrink = true;
    else if(size_w < 80)
        shrink = false;

    throttle_fill->set_width(size_w);
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
    //lv_obj_set_style_bg_color(scr, lv_color_make(0,0,0), LV_PART_MAIN);

    Create_UI();

    // error_box();

    // text_object();

    // Main LVGL loop to quit program
    bool quit = false;

    // We can control the frequency at which the ui is updated here
    lv_timer_create([](lv_timer_t * t) {
        Update_UI();
    }, 16, NULL);

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

        if (time_till_next == 0) {
            time_till_next = 1; 
        }

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

    // start the code
    init_display_rgb();
    init_touch();
    lv_init();

    uint32_t buf_size = LCD_WIDTH * (LCD_HEIGHT/10) * sizeof(uint16_t);
    void* buf1;
    void* buf2;
    //esp_lcd_rgb_panel_get_frame_buffer(panel_handle, 2, &buf1, &buf2); These are fullscreen buffers

    // MALLOC_CAP_SPIRAM for allocating in the external ram
    // MALLOC_CAP_DMA for allocatin in the internal ram
    // MALLOC_CAP_8BIT for any memory that can hold 8-bits
    // MALLOC_CAP_INTERNAL for only internal memory
    //if(!buf1) 
    buf1 = heap_caps_malloc(buf_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);
    //if(!buf2) 
    buf2 = heap_caps_malloc(buf_size, MALLOC_CAP_DMA | MALLOC_CAP_INTERNAL);

    if(!buf1 || !buf2) {
        Serial.println("Buffer alloc failed! Free up heap.");
        return;
    }

    // temp display window on PC (MUST STAY THE SAME AS IT MODELS THE EXACT PIXEL DIMENSIONS OF THE DISPLAY)
    lv_display_t * disp = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
    lv_obj_t * scr = lv_screen_active();

    lv_display_set_flush_cb(disp, disp_flush);
    lv_display_set_buffers(disp, buf1, buf2, buf_size, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_RGB565);

    Create_UI();

    const esp_timer_create_args_t timer_args = {
        .callback = &lvgl_tick_cb,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "lvgl_tick"
    };
    esp_timer_create(&timer_args, &lvgl_tick_timer);
    esp_timer_start_periodic(lvgl_tick_timer, 2000);

     // We can control the frequency at which the ui is updated here
    lv_timer_create([](lv_timer_t * t) {
        Update_UI();
    }, 16, NULL);

    Serial.println("Setup Complete!");
}


int counter = 0;
void loop() {
    // Main LVGL loop to quit program
    lv_timer_handler();
    vTaskDelay(pdMS_TO_TICKS(10));
}

#endif
