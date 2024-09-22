SET(SOURCES components/ui_comp_btn_bg.c
    components/ui_comp.c
    screens/ui_Screen1.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    images/ui_img_1548360417.c
    images/ui_img_pad_bg_png.c
    images/ui_img_pad_single_png.c
    images/ui_img_pad_single_v_png.c
    images/ui_img_pad_singl_h_png.c
    images/ui_img_selected_png.c
    fonts/ui_font_Pix12.c
    fonts/ui_font_Pix16.c)

add_library(ui ${SOURCES})
