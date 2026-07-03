#pragma once
extern "C" {
#include <lvgl.h>
}

void setup_splash();

lv_obj_t * battery_temp_box();

lv_obj_t * break_temp_box();

lv_obj_t * motor_temp_box();

lv_obj_t * speed_box();

lv_obj_t * throttle_box();

lv_obj_t * error_box();

// class PopUpMessage {
// private:
//     lv_obj_t* m_canvas;
//     lv_obj_t* m_label;
//     lv_color_t m_bg_color;
//     lv_point_t** m_points;
//     uint32_t m_points_count;

//     struct Size {
//         uint32_t w = 0;
//         uint32_t h = 0;
//     } m_size;


// public:
//     PopUpMessage(lv_obj_t* parent, Size size ,lv_point_t** points, uint32_t points_count);

//     void drawMessage(const char* message);
// };
