#pragma once
extern "C" {
#include <lvgl.h>
}

typedef struct Position {
    uint32_t x;
    uint32_t y;
};

typedef struct Size {
    uint32_t height;
    uint32_t width;
};

void setup_splash();

/*
    Working in progress idea for pop-ups
*/
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

class base_shape 
{
protected:
    lv_obj_t* m_obj;
    lv_obj_t* m_parent;
    Position m_pos;
    Size m_size;

    base_shape(lv_obj_t* parent) { 
        m_parent = parent; 
        m_obj = lv_obj_create(m_parent);
        lv_obj_add_flag(m_obj, LV_OBJ_FLAG_HIDDEN);
    }
public:
    void set_pos(uint32_t x, uint32_t y) {
        m_pos.x = x; 
        m_pos.y = y; 

        lv_obj_set_pos(m_obj, m_pos.x, m_pos.y);
    }
    void set_x(uint32_t x) { set_pos(x, m_pos.y); }
    void set_y(uint32_t y) { set_pos(m_pos.x, y); }
    Position get_pos() const { return m_pos; }

    lv_obj_t* get_lv_obj() const { return m_obj; }

    void set_size(uint32_t height, uint32_t width) { 
        m_size.height = height; 
        m_size.width = width; 

        lv_obj_set_size(m_obj, m_size.width, m_size.height);
    }
    void set_height(uint32_t height) { set_size(height, m_size.width); }
    void set_width(uint32_t width) { set_size(m_size.height, width); }
    Size get_size() const { return m_size; }

    void set_visibility(bool visible) { 
        lv_obj_set_flag(m_obj, LV_OBJ_FLAG_HIDDEN, !visible); 
    }
};

class box_widget : public base_shape {
public:
    box_widget(lv_obj_t* parent, uint32_t height, uint32_t width, uint32_t x, uint32_t y, bool center=false);
};

// public:
//     PopUpMessage(lv_obj_t* parent, Size size ,lv_point_t** points, uint32_t points_count);

//     void drawMessage(const char* message);
// };
