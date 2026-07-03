#include "boxes.h"
#include "core/lv_obj_pos.h"
#include "core/lv_obj_style.h"
#include "core/lv_obj_style_gen.h"
#include "display/lv_display.h"
#include "lv_api_map_v8.h"
#include "widgets/image/lv_image.h"

#ifdef ROUND_DISPLAY
#include <Arduino.h>
#endif

// Render a splash
void setup_splash() {
    LV_IMAGE_DECLARE(Splash_Screen);

    lv_obj_t* splash = lv_image_create(lv_screen_active());

    lv_img_set_src(splash, &Splash_Screen);

    lv_obj_fade_out(splash, 1000, 500);
}

box_widget::box_widget(lv_obj_t* parent ,uint32_t height, uint32_t width, uint32_t x, uint32_t y, bool center) 
    : base_shape(parent)
{
    set_size(height, width);

    if(center) {
        lv_obj_center(m_obj);
        m_pos.x = lv_obj_get_x(m_obj);
        m_pos.y = lv_obj_get_y(m_obj);
    } else
        set_pos(x, y);

    lv_obj_set_style_radius(m_obj, 50, LV_PART_MAIN);
    lv_obj_set_style_bg_color(m_obj, lv_color_make(255, 255, 255), LV_PART_MAIN);

    lv_obj_set_style_outline_color(m_obj, lv_color_make(43, 114, 194), LV_PART_MAIN);
    lv_obj_set_style_outline_width(m_obj, 3, LV_PART_MAIN);
    lv_obj_set_style_outline_pad(m_obj, 2, LV_PART_MAIN);
    lv_obj_set_style_outline_opa(m_obj, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_fade_in(m_obj, 1000, 2000);

    set_visibility(true);
}

// lv_obj_t * error_box() {
//     lv_obj_t * box_error = lv_obj_create(lv_screen_active());
//     lv_obj_set_size(box_error, 400, 400);
//     lv_obj_center(box_error);

//     lv_obj_set_style_bg_color(box_error, lv_color_make(2, 50 , 80), LV_PART_MAIN);

//     lv_obj_set_style_bg_color(box_error, lv_color_make(2, 50 , 80), LV_PART_MAIN);

//     return box_error;
// }

// THINGS WE NEED TO INCLUDE AS DATA PARTS ON THE DISPLAY:

// Speed in center
// Battery temp (bottom left)
// Breaks temp (bottom center)
// Motor temp (bottom right)
// Throttle (top center)
// GENERAL RED ERROR MESSAGE (everywhere)
