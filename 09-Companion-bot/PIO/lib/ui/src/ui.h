// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.4.0
// LVGL version: 8.3.11
// Project name: bot

#ifndef _BOT_UI_H
#define _BOT_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined __has_include
  #if __has_include("lvgl.h")
    #include "lvgl.h"
  #elif __has_include("lvgl/lvgl.h")
    #include "lvgl/lvgl.h"
  #else
    #include "lvgl.h"
  #endif
#else
  #include "lvgl.h"
#endif

#include "ui_helpers.h"
#include "ui_events.h"

void full_Animation( lv_obj_t *TargetObject, int delay);
void angry_Animation( lv_obj_t *TargetObject, int delay);
void blink_Animation( lv_obj_t *TargetObject, int delay);
void love_Animation( lv_obj_t *TargetObject, int delay);
void sad_Animation( lv_obj_t *TargetObject, int delay);
void sleepy_Animation( lv_obj_t *TargetObject, int delay);
void awe_Animation( lv_obj_t *TargetObject, int delay);
// SCREEN: ui_Screen1
void ui_Screen1_screen_init(void);
extern lv_obj_t *ui_Screen1;
extern lv_obj_t *ui_blink;
extern lv_obj_t *ui_angry;
extern lv_obj_t *ui_love;
extern lv_obj_t *ui_sad;
extern lv_obj_t *ui_sleepy;
extern lv_obj_t *ui_awe;
extern lv_obj_t *ui____initial_actions0;

LV_IMG_DECLARE( ui_img_440651951);   // assets/eye-00.png
LV_IMG_DECLARE( ui_img_440650926);   // assets/eye-01.png
LV_IMG_DECLARE( ui_img_440649901);   // assets/eye-02.png
LV_IMG_DECLARE( ui_img_440657324);   // assets/eye-03.png
LV_IMG_DECLARE( ui_img_440656299);   // assets/eye-04.png
LV_IMG_DECLARE( ui_img_440655274);   // assets/eye-05.png
LV_IMG_DECLARE( ui_img_440654249);   // assets/eye-06.png
LV_IMG_DECLARE( ui_img_440661672);   // assets/eye-07.png
LV_IMG_DECLARE( ui_img_440643239);   // assets/eye-08.png
LV_IMG_DECLARE( ui_img_440642214);   // assets/eye-09.png
LV_IMG_DECLARE( ui_img_1248512660);   // assets/eye-10.png
LV_IMG_DECLARE( ui_img_1248511635);   // assets/eye-11.png
LV_IMG_DECLARE( ui_img_1248510610);   // assets/eye-12.png
LV_IMG_DECLARE( ui_img_1248518033);   // assets/eye-13.png
LV_IMG_DECLARE( ui_img_1248517008);   // assets/eye-14.png
LV_IMG_DECLARE( ui_img_1248515983);   // assets/eye-15.png
LV_IMG_DECLARE( ui_img_1248514958);   // assets/eye-16.png
LV_IMG_DECLARE( ui_img_1248522381);   // assets/eye-17.png
LV_IMG_DECLARE( ui_img_1248503948);   // assets/eye-18.png
LV_IMG_DECLARE( ui_img_1248502923);   // assets/eye-19.png
LV_IMG_DECLARE( ui_img_1722806407);   // assets/eye-20.png
LV_IMG_DECLARE( ui_img_1722807432);   // assets/eye-21.png
LV_IMG_DECLARE( ui_img_1722808457);   // assets/eye-22.png
LV_IMG_DECLARE( ui_img_1722801034);   // assets/eye-23.png
LV_IMG_DECLARE( ui_img_1722802059);   // assets/eye-24.png
LV_IMG_DECLARE( ui_img_1722803084);   // assets/eye-25.png
LV_IMG_DECLARE( ui_img_1722804109);   // assets/eye-26.png
LV_IMG_DECLARE( ui_img_1722796686);   // assets/eye-27.png
LV_IMG_DECLARE( ui_img_1722815119);   // assets/eye-28.png
LV_IMG_DECLARE( ui_img_1722816144);   // assets/eye-29.png
LV_IMG_DECLARE( ui_img_914945698);   // assets/eye-30.png
LV_IMG_DECLARE( ui_img_914946723);   // assets/eye-31.png
LV_IMG_DECLARE( ui_img_914947748);   // assets/eye-32.png
LV_IMG_DECLARE( ui_img_914940325);   // assets/eye-33.png
LV_IMG_DECLARE( ui_img_914941350);   // assets/eye-34.png
LV_IMG_DECLARE( ui_img_914942375);   // assets/eye-35.png
LV_IMG_DECLARE( ui_img_914943400);   // assets/eye-36.png
LV_IMG_DECLARE( ui_img_914935977);   // assets/eye-37.png
LV_IMG_DECLARE( ui_img_914954410);   // assets/eye-38.png
LV_IMG_DECLARE( ui_img_914955435);   // assets/eye-39.png
LV_IMG_DECLARE( ui_img_408702531);   // assets/eye-40.png
LV_IMG_DECLARE( ui_img_408701506);   // assets/eye-41.png
LV_IMG_DECLARE( ui_img_408700481);   // assets/eye-42.png
LV_IMG_DECLARE( ui_img_408707904);   // assets/eye-43.png
LV_IMG_DECLARE( ui_img_408706879);   // assets/eye-44.png
LV_IMG_DECLARE( ui_img_408705854);   // assets/eye-45.png
LV_IMG_DECLARE( ui_img_408704829);   // assets/eye-46.png
LV_IMG_DECLARE( ui_img_408712252);   // assets/eye-47.png
LV_IMG_DECLARE( ui_img_408693819);   // assets/eye-48.png
LV_IMG_DECLARE( ui_img_408692794);   // assets/eye-49.png
LV_IMG_DECLARE( ui_img_1216563240);   // assets/eye-50.png
LV_IMG_DECLARE( ui_img_1216562215);   // assets/eye-51.png
LV_IMG_DECLARE( ui_img_1216561190);   // assets/eye-52.png
LV_IMG_DECLARE( ui_img_1216568613);   // assets/eye-53.png
LV_IMG_DECLARE( ui_img_1216567588);   // assets/eye-54.png
LV_IMG_DECLARE( ui_img_1216566563);   // assets/eye-55.png
LV_IMG_DECLARE( ui_img_1216565538);   // assets/eye-56.png
LV_IMG_DECLARE( ui_img_1216572961);   // assets/eye-57.png
LV_IMG_DECLARE( ui_img_1216554528);   // assets/eye-58.png
LV_IMG_DECLARE( ui_img_1216553503);   // assets/eye-59.png
LV_IMG_DECLARE( ui_img_1754755827);   // assets/eye-60.png
LV_IMG_DECLARE( ui_img_1754756852);   // assets/eye-61.png
LV_IMG_DECLARE( ui_img_1754757877);   // assets/eye-62.png
LV_IMG_DECLARE( ui_img_1754750454);   // assets/eye-63.png
LV_IMG_DECLARE( ui_img_1754751479);   // assets/eye-64.png
LV_IMG_DECLARE( ui_img_1754752504);   // assets/eye-65.png
LV_IMG_DECLARE( ui_img_1754753529);   // assets/eye-66.png
LV_IMG_DECLARE( ui_img_1754746106);   // assets/eye-67.png
LV_IMG_DECLARE( ui_img_1754764539);   // assets/eye-68.png
LV_IMG_DECLARE( ui_img_1754765564);   // assets/eye-69.png
LV_IMG_DECLARE( ui_img_946895118);   // assets/eye-70.png
LV_IMG_DECLARE( ui_img_946896143);   // assets/eye-71.png
LV_IMG_DECLARE( ui_img_946897168);   // assets/eye-72.png
LV_IMG_DECLARE( ui_img_946889745);   // assets/eye-73.png
LV_IMG_DECLARE( ui_img_946890770);   // assets/eye-74.png
LV_IMG_DECLARE( ui_img_946891795);   // assets/eye-75.png
LV_IMG_DECLARE( ui_img_946892820);   // assets/eye-76.png




void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif
