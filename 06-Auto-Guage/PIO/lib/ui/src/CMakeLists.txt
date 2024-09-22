SET(SOURCES screens/ui_Screen1.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    images/ui_img_images_bg_png.c
    images/ui_img_images_guage_bg_png.c
    images/ui_img_images_compas_png.c
    images/ui_img_images_compass_overlay_png.c
    images/ui_img_images_devider_png.c
    images/ui_img_images_guage_indicator_png.c
    images/ui_img_images_range_bg_png.c
    images/ui_img_images_range_knob_png.c
    images/ui_img_images_battery_indicator_png.c
    images/ui_img_images_battery_bg_png.c
    images/ui_img_images_guage_bg_1_png.c
    fonts/ui_font_Hollow18.c
    fonts/ui_font_Hollow22.c
    fonts/ui_font_Hollow38.c
    fonts/ui_font_Hollow85.c
    fonts/ui_font_Inter12.c)

add_library(ui ${SOURCES})
