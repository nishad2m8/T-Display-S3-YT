SET(SOURCES screens/ui_Screen1.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    images/ui_img_img_slider_bg_png.c
    images/ui_img_img_slider_knob_png.c
    images/ui_img_img_bg_png.c
    images/ui_img_img_ac_wind_png.c
    images/ui_img_img_ac_png.c
    images/ui_img_img_lamp_png.c
    images/ui_img_img_lamp_lux_png.c
    images/ui_img_2024126593.c
    images/ui_img_img_blind_pos_png.c
    images/ui_img_img_blind_png.c
    images/ui_img_img_icon_active_png.c
    images/ui_img_img_icon_ac_png.c
    images/ui_img_img_icon_lamp_png.c
    images/ui_img_img_icon_blind_png.c
    fonts/ui_font_be30.c
    fonts/ui_font_be55.c)

add_library(ui ${SOURCES})
