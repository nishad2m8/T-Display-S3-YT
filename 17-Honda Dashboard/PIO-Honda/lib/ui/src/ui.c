// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.5.1
// LVGL version: 8.3.11
// Project name: Honda Watch

#include "ui.h"
#include "ui_helpers.h"

///////////////////// VARIABLES ////////////////////
void speed_Animation( lv_obj_t *TargetObject, int delay);

// SCREEN: ui_Screen1
void ui_Screen1_screen_init(void);
lv_obj_t *ui_Screen1;
lv_obj_t *ui_Image_bg;
lv_obj_t *ui_Panel_SL;
lv_obj_t *ui_Panel_SR;
lv_obj_t *ui_Label_speed;
lv_obj_t *ui_Image_needle_1;
lv_obj_t *ui_Image_needle_2;
lv_obj_t *ui_Image_icon;
lv_obj_t *ui_Label_main;
lv_obj_t *ui_Label_gear;
lv_obj_t *ui_Image_Speed;
// CUSTOM VARIABLES

// EVENTS
lv_obj_t *ui____initial_actions0;

// IMAGES AND IMAGE SETS
const lv_img_dsc_t *ui_imgset_1862877112[13] = {&ui_img_1963052260, &ui_img_1963053285, &ui_img_1963050210, &ui_img_1963051235, &ui_img_1963047912, &ui_img_1963048937, &ui_img_1963045862, &ui_img_1963046887, &ui_img_1963060972, &ui_img_1963061997, &ui_img_1524054327, &ui_img_1524053302, &ui_img_1524056377};

///////////////////// TEST LVGL SETTINGS ////////////////////
#if LV_COLOR_DEPTH != 16
    #error "LV_COLOR_DEPTH should be 16bit to match SquareLine Studio's settings"
#endif
#if LV_COLOR_16_SWAP !=0
    #error "LV_COLOR_16_SWAP should be 0 to match SquareLine Studio's settings"
#endif

///////////////////// ANIMATIONS ////////////////////
void speed_Animation( lv_obj_t *TargetObject, int delay)
{
ui_anim_user_data_t *PropertyAnimation_0_user_data = lv_mem_alloc(sizeof(ui_anim_user_data_t));
PropertyAnimation_0_user_data->target = TargetObject;
PropertyAnimation_0_user_data->imgset = (lv_img_dsc_t **)ui_imgset_1862877112;
PropertyAnimation_0_user_data->imgset_size = sizeof(ui_imgset_1862877112)/(sizeof(lv_img_dsc_t*));
PropertyAnimation_0_user_data->val = -1;
lv_anim_t PropertyAnimation_0;
lv_anim_init(&PropertyAnimation_0);
lv_anim_set_time(&PropertyAnimation_0, 1000);
lv_anim_set_user_data(&PropertyAnimation_0, PropertyAnimation_0_user_data);
lv_anim_set_custom_exec_cb(&PropertyAnimation_0, _ui_anim_callback_set_image_frame );
lv_anim_set_values(&PropertyAnimation_0, 0, 13 );
lv_anim_set_path_cb( &PropertyAnimation_0, lv_anim_path_linear);
lv_anim_set_delay( &PropertyAnimation_0, delay + 0 );
lv_anim_set_deleted_cb( &PropertyAnimation_0, _ui_anim_callback_free_user_data );
lv_anim_set_playback_time(&PropertyAnimation_0, 0);
lv_anim_set_playback_delay(&PropertyAnimation_0, 0);
 lv_anim_set_repeat_count(&PropertyAnimation_0, LV_ANIM_REPEAT_INFINITE );
lv_anim_set_repeat_delay(&PropertyAnimation_0, 0);
lv_anim_set_early_apply( &PropertyAnimation_0, false );
 lv_anim_set_get_value_cb(&PropertyAnimation_0, &_ui_anim_callback_get_image_frame );
lv_anim_start(&PropertyAnimation_0);

}

///////////////////// FUNCTIONS ////////////////////

///////////////////// SCREENS ////////////////////

void ui_init( void )
{
lv_disp_t *dispp = lv_disp_get_default();
lv_theme_t *theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), true, LV_FONT_DEFAULT);
lv_disp_set_theme(dispp, theme);
ui_Screen1_screen_init();
ui____initial_actions0 = lv_obj_create(NULL);
lv_disp_load_scr( ui_Screen1);
}
