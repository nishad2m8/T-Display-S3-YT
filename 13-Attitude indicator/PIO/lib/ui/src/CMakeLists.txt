SET(SOURCES screens/ui_Screen1.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    ui_events.c
    images/ui_img_images_dial_bg_png.c
    images/ui_img_images_pitch_scale_png.c
    images/ui_img_images_roll_scale_png.c
    images/ui_img_images_mask_png.c
    images/ui_img_images_dial_png.c
    images/ui_img_images_pointer_png.c)

add_library(ui ${SOURCES})
