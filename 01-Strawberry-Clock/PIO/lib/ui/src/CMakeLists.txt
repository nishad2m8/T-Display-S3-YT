SET(SOURCES screens/ui_Screen1.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_bg_png.c
    images/ui_img_strawberry_png.c
    images/ui_img_wifi_png.c
    images/ui_img_temp01_png.c
    images/ui_img_feelslike2_png.c
    images/ui_img_range_png.c
    fonts/ui_font_vag18.c
    fonts/ui_font_vag20.c
    fonts/ui_font_vag45.c
    fonts/ui_font_vag50.c
    fonts/ui_font_vag70.c
    fonts/ui_font_vag80.c)

add_library(ui ${SOURCES})
