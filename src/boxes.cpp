#include "boxes.h"
#include "core/lv_obj_pos.h"
#include "core/lv_obj_style.h"
#include "core/lv_obj_style_gen.h"
#include "display/lv_display.h"
#include "lv_api_map_v8.h"
#include "widgets/image/lv_image.h"

#include <Arduino.h>

// Render a splash
void setup_splash() {
    LV_IMAGE_DECLARE(Splash_Screen);

    lv_obj_t* splash = lv_image_create(lv_screen_active());

    lv_img_set_src(splash, &Splash_Screen);

    lv_obj_fade_out(splash, 1000, 500);
}

lv_obj_t * battery_temp_box() {
    Serial.println("Battery temp debug line....");
    lv_obj_t * my_box = lv_obj_create(lv_screen_active());
    lv_obj_set_size(my_box, 80, 80);
    lv_obj_set_pos(my_box, 100, 336);

    lv_obj_set_style_bg_color(my_box, lv_color_make(255, 255, 255), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(my_box, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(my_box, 200, LV_PART_MAIN);

    lv_obj_set_style_outline_color(my_box, lv_color_make(43, 114, 194), LV_PART_MAIN);
    lv_obj_set_style_outline_width(my_box, 3, LV_PART_MAIN);
    lv_obj_set_style_outline_pad(my_box, 2, LV_PART_MAIN);
    lv_obj_set_style_outline_opa(my_box, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_fade_in(my_box, 1000, 2000); 

    return my_box;
}

lv_obj_t * break_temp_box() {
    lv_obj_t * my_box_2 = lv_obj_create(lv_screen_active());

    lv_obj_set_size(my_box_2, 80, 80);
    lv_obj_set_x(my_box_2, 200);
    lv_obj_set_y(my_box_2, 336);

    lv_obj_set_style_bg_color(my_box_2, lv_color_make(255, 255, 255), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(my_box_2, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(my_box_2, 200, LV_PART_MAIN);
    
    lv_obj_set_style_outline_color(my_box_2, lv_color_make(43, 114, 194), LV_PART_MAIN);
    lv_obj_set_style_outline_width(my_box_2, 3, LV_PART_MAIN);
    lv_obj_set_style_outline_pad(my_box_2, 2, LV_PART_MAIN);
    lv_obj_set_style_outline_opa(my_box_2, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_fade_in(my_box_2, 1000, 2000); 

    return my_box_2;
}

lv_obj_t * motor_temp_box() {
    lv_obj_t * my_box_3 = lv_obj_create(lv_screen_active());
    
    lv_obj_set_size(my_box_3, 80, 80);
    lv_obj_set_x(my_box_3, 300);
    lv_obj_set_y(my_box_3, 336);

    lv_obj_set_style_bg_color(my_box_3, lv_color_make(255, 255 , 255), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(my_box_3, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_radius(my_box_3, 200, LV_PART_MAIN);
    
    lv_obj_set_style_outline_color(my_box_3, lv_color_make(43, 114, 194), LV_PART_MAIN);
    lv_obj_set_style_outline_width(my_box_3, 3, LV_PART_MAIN);
    lv_obj_set_style_outline_pad(my_box_3, 2, LV_PART_MAIN);
    lv_obj_set_style_outline_opa(my_box_3, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_fade_in(my_box_3, 1000, 2000); 

    return my_box_3;
}

lv_obj_t * speed_box() {
    lv_obj_t * box_c = lv_obj_create(lv_screen_active());
    lv_obj_set_size(box_c, 300, 150);
    
    lv_obj_set_style_radius(box_c, 50, LV_PART_MAIN);
    lv_obj_set_style_bg_color(box_c, lv_color_make(255, 255 , 255), LV_PART_MAIN);
    lv_obj_center(box_c);

    lv_obj_set_style_outline_color(box_c, lv_color_make(43, 114, 194), LV_PART_MAIN);
    lv_obj_set_style_outline_width(box_c, 3, LV_PART_MAIN);
    lv_obj_set_style_outline_pad(box_c, 2, LV_PART_MAIN);
    lv_obj_set_style_outline_opa(box_c, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_fade_in(box_c, 1000, 2000); 

    return box_c;
}

lv_obj_t * throttle_box() {
    lv_obj_t * box_throt = lv_obj_create(lv_screen_active());
    lv_obj_set_size(box_throt, 300, 90);
    
    lv_obj_set_style_radius(box_throt, 200, LV_PART_MAIN);
    lv_obj_set_x(box_throt, 90);
    lv_obj_set_y(box_throt, 50);
    lv_obj_set_style_bg_color(box_throt, lv_color_make(255, 255 , 255), LV_PART_MAIN);
    
    lv_obj_set_style_outline_color(box_throt, lv_color_make(43, 114, 194), LV_PART_MAIN);
    lv_obj_set_style_outline_width(box_throt, 3, LV_PART_MAIN);
    lv_obj_set_style_outline_pad(box_throt, 2, LV_PART_MAIN);
    lv_obj_set_style_outline_opa(box_throt, LV_OPA_COVER, LV_PART_MAIN);

    lv_obj_fade_in(box_throt, 1000, 2000); 

    return box_throt;
}

// lv_obj_t * error_box() {
//     lv_obj_t * box_error = lv_obj_create(lv_screen_active());
//     lv_obj_set_size(box_error, 400, 400);
//     lv_obj_center(box_error);

//     lv_obj_set_style_bg_color(box_error, lv_color_make(2, 50 , 80), LV_PART_MAIN);

//     lv_obj_set_style_bg_color(box_error, lv_color_make(2, 50 , 80), LV_PART_MAIN);

//     return box_error;
// }

// lv_obj_t * racing_logo() {

// }

// THINGS WE NEED TO INCLUDE AS DATA PARTS ON THE DISPLAY:

// Speed in center
// Battery temp (bottom left)
// Breaks temp (bottom center)
// Motor temp (bottom right)
// Throttle (top center)
// GENERAL RED ERROR MESSAGE (everywhere)
