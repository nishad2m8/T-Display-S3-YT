// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.3.4
// LVGL version: 8.3.6
// Project name: Bike_dashboard

#include "../ui.h"

#ifndef LV_ATTRIBUTE_MEM_ALIGN
#define LV_ATTRIBUTE_MEM_ALIGN
#endif

// IMAGE DATA: assets/images/range knob.png
const LV_ATTRIBUTE_MEM_ALIGN uint8_t ui_img_images_range_knob_png_data[] = {
0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,
    0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,
    0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,
    0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x5D,0xEF,0x00,0xFB,0xDE,0x00,0x1C,0xE7,0x00,0xBE,0xF7,0x00,0xFF,0xFF,0x00,0x3C,0xE7,0x00,0xDF,0xFF,0x00,0xBE,0xF7,0x00,0x9E,0xF7,0x00,0xBE,0xF7,0x71,0xBE,0xF7,0x96,0x7D,0xEF,0xE1,0x9E,0xF7,0xFF,0x7D,0xEF,0xFF,0x9E,0xF7,0xEC,0xBE,0xF7,0xC5,0xDF,0xFF,0x84,0x9E,0xF7,0x01,0x3C,0xE7,0x00,0x9E,0xF7,0x00,0x7D,0xEF,0x00,0x9E,0xF7,0x00,0x7D,0xEF,0x00,0x38,0xC6,0x00,0x38,0xC6,0x00,0x9E,0xF7,0x00,0x51,0x8C,0x00,0xFF,0xFF,0x00,0x7D,0xEF,0x00,0x9E,0xF7,0x00,0x9E,0xF7,0x00,0xDF,0xFF,0x00,0x3C,0xE7,0x00,0x9E,0xF7,0x00,0xFF,0xFF,0x00,0x34,0xA5,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x5D,0xEF,0x00,0xFB,0xDE,0x00,0x1C,0xE7,0x00,0xBE,0xF7,0x00,0xFF,0xFF,0x00,0x3C,0xE7,0x00,0xDF,0xFF,0x6F,0xBE,0xF7,0xE2,0x9E,0xF7,0xFF,0x7D,0xEF,0xFF,0xDB,0xDE,0xFF,0xFB,0xDE,0xFF,0xFB,0xDE,0xFF,0x1C,0xE7,0xFF,
    0x1C,0xE7,0xFF,0x9A,0xD6,0xFF,0xFB,0xDE,0xFF,0xFB,0xDE,0xFF,0x7D,0xEF,0xFF,0xBE,0xF7,0x54,0x9E,0xF7,0x00,0x9E,0xF7,0x00,0x7D,0xEF,0x00,0x38,0xC6,0x00,0x38,0xC6,0x00,0x9E,0xF7,0x00,0x51,0x8C,0x00,0xFF,0xFF,0x00,0x7D,0xEF,0x00,0x9E,0xF7,0x00,0x9E,0xF7,0x00,0xDF,0xFF,0x00,0x3C,0xE7,0x00,0x9E,0xF7,0x00,0xFF,0xFF,0x00,0x34,0xA5,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xBE,0xF7,0x21,0xFB,0xDE,0x55,0xDB,0xDE,0x30,0xBE,0xF7,0x68,0xDF,0xFF,0xE1,0xFF,0xFF,0xFF,0xBE,0xF7,0xFF,0xBE,0xF7,0xFF,0x9E,0xF7,0xFF,0x1C,0xE7,0xFF,0x7D,0xEF,0xFF,0x5D,0xEF,0xFF,0x3C,0xE7,0xFF,0x3C,0xE7,0xFF,0x1C,0xE7,0xFF,0x3C,0xE7,0xFF,0x1C,0xE7,0xFF,0x1C,0xE7,0xFF,0x1C,0xE7,0xFF,0xFB,0xDE,0xFF,0xFB,0xDE,0xFF,0xBE,0xF7,0xBA,0x7D,0xEF,0x00,0x59,0xCE,0x00,0xF7,0xBD,0x00,0x5D,0xEF,0x00,0xAE,0x73,0x00,0xFF,0xFF,0x00,0x7D,0xEF,0x00,0x9E,0xF7,0x00,0x9E,0xF7,0x00,0xDF,0xFF,0x00,0x3C,0xE7,0x00,0x9E,0xF7,0x00,0xFF,0xFF,0x00,0x34,0xA5,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,
    0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x75,0xAD,0x79,0x5D,0xEF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xDF,0xFF,0xFF,0xDF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xBE,0xF7,0xFF,0x3C,0xE7,0xFF,0x5D,0xEF,0xFF,0x3C,0xE7,0xFF,0x3C,0xE7,0xFF,0x3C,0xE7,0xFF,0x3C,0xE7,0xFF,0x3C,0xE7,0xFF,0x3C,0xE7,0xFF,0x5D,0xEF,0xFF,0x5D,0xEF,0xFF,0x9E,0xF7,0xFF,0x3C,0xE7,0xFF,0xFF,0xFF,0xFF,0x14,0xA5,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xDF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xEA,0xFF,0xFF,0xA1,0xFF,0xFF,0x53,0x5D,0xEF,0x00,0x9E,0xF7,0x00,0xFF,0xFF,0x00,0x34,0xA5,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x92,0x94,0x88,0x59,0xCE,0xFF,0xFB,0xDE,0xFF,0x79,0xCE,0xFF,0x3C,0xE7,0xFF,0x79,0xCE,0xFF,0x0C,0x63,0xFF,0xCF,0x7B,0xFF,0x8E,0x73,0xFF,0x1C,0xE7,0xFF,0xBA,0xD6,0xFF,0x18,0xC6,0xFF,0x79,0xCE,0xFF,0x59,0xCE,0xFF,0x59,0xCE,0xFF,0x38,0xC6,0xFF,0x38,0xC6,0xFF,0x18,0xC6,0xFF,0xF7,0xBD,0xFF,0xB6,0xB5,0xFF,0x96,0xB5,0xFF,0x55,0xAD,0xFF,
    0xD3,0x9C,0xFF,0xD3,0x9C,0xFF,0x18,0xC6,0xFF,0x1C,0xE7,0xFF,0xBA,0xD6,0xFF,0x7D,0xEF,0xFF,0x10,0x84,0xFF,0xEF,0x7B,0xFF,0xAE,0x73,0xFF,0x0C,0x63,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xAC,0xFF,0xFF,0x50,0x34,0xA5,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x14,0xA5,0x97,0xF3,0x9C,0xFF,0x18,0xC6,0xFF,0x9A,0xD6,0xFF,0xB6,0xB5,0xFF,0x8A,0x52,0xFF,0x71,0x8C,0xFF,0x14,0xA5,0xFF,0xD3,0x9C,0xFF,0x28,0x42,0xFF,0xBA,0xD6,0xFF,0x18,0xC6,0xFF,0xF7,0xBD,0xFF,0x18,0xC6,0xFF,0x18,0xC6,0xFF,0xF7,0xBD,0xFF,0xF7,0xBD,0xFF,0xD7,0xBD,0xFF,0xD7,0xBD,0xFF,0x96,0xB5,0xFF,0x75,0xAD,0xFF,0x55,0xAD,0xFF,0xF3,0x9C,0xFF,0x51,0x8C,0xFF,0xCF,0x7B,0xFF,0x9A,0xD6,0xFF,0x79,0xCE,0xFF,0x6D,0x6B,0xFF,0x10,0x84,0xFF,0xB2,0x94,0xFF,0xB2,0x94,0xFF,0xF3,0x9C,0xFF,0x2C,0x63,0xFF,0xFB,0xDE,0xFF,0x2C,0x63,0xFF,0x18,0xC6,0x13,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0x51,0x8C,0x53,0xD3,0x9C,0xFF,0xB6,0xB5,0xFF,0x79,0xCE,0xFF,0x4D,0x6B,0xFF,0x59,0xCE,0xFF,
    0xAE,0x73,0xFF,0x8E,0x73,0xFF,0x69,0x4A,0xFF,0x59,0xCE,0xFF,0xCB,0x5A,0xFF,0x59,0xCE,0xFF,0xB6,0xB5,0xFF,0xF7,0xBD,0xFF,0xF7,0xBD,0xFF,0x18,0xC6,0xFF,0x18,0xC6,0xFF,0x38,0xC6,0xFF,0x59,0xCE,0xFF,0x59,0xCE,0xFF,0x79,0xCE,0xFF,0x79,0xCE,0xFF,0x59,0xCE,0xFF,0x38,0xC6,0xFF,0x6D,0x6B,0xFF,0x18,0xC6,0xFF,0x18,0xC6,0xFF,0x10,0x84,0xFF,0x14,0xA5,0xFF,0x8E,0x73,0xFF,0x08,0x42,0xFF,0xB2,0x94,0xFF,0xCB,0x5A,0xFF,0x38,0xC6,0xFF,0xEB,0x5A,0xFF,0x92,0x94,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xB2,0x94,0x00,0x71,0x8C,0xFF,0x14,0xA5,0xFF,0xB6,0xB5,0xFF,0xD3,0x9C,0xFF,0x8A,0x52,0xFF,0x0C,0x63,0xFF,0x75,0xAD,0xFF,0xEB,0x5A,0xFF,0xAE,0x73,0xFF,0x92,0x94,0xFF,0xB6,0xB5,0xFF,0x75,0xAD,0xFF,0x96,0xB5,0xFF,0xB6,0xB5,0xFF,0xB6,0xB5,0xFF,0xB6,0xB5,0xFF,0xB6,0xB5,0xFF,0xD7,0xBD,0xFF,0xD7,0xBD,0xFF,0xF7,0xBD,0xFF,0xF7,0xBD,0xFF,0xD7,0xBD,0xFF,0xD7,0xBD,0xFF,0x92,0x94,0xFF,0x92,0x94,0xFF,0xD3,0x9C,0xFF,0x71,0x8C,0xFF,0x28,0x42,0xFF,0x92,0x94,0xFF,
    0x92,0x94,0xFF,0x69,0x4A,0xFF,0x10,0x84,0xFF,0x34,0xA5,0xFF,0xF3,0x9C,0xFF,0x9A,0xD6,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xCF,0x7B,0x00,0xCF,0x7B,0x01,0xCB,0x5A,0xFF,0xB2,0x94,0xFF,0x4D,0x6B,0xFF,0x30,0x84,0xFF,0x55,0xAD,0xFF,0xE7,0x39,0xFF,0x96,0xB5,0xFF,0xB2,0x94,0xFF,0x4D,0x6B,0xFF,0x55,0xAD,0xFF,0x34,0xA5,0xFF,0x14,0xA5,0xFF,0xF3,0x9C,0xFF,0xD3,0x9C,0xFF,0xD3,0x9C,0xFF,0xD3,0x9C,0xFF,0xB2,0x94,0xFF,0xD3,0x9C,0xFF,0xD3,0x9C,0xFF,0xD3,0x9C,0xFF,0xB2,0x94,0xFF,0xB2,0x94,0xFF,0x71,0x8C,0xFF,0x10,0x84,0xFF,0xD7,0xBD,0xFF,0xCF,0x7B,0xFF,0xB2,0x94,0xFF,0xEB,0x5A,0xFF,0x92,0x94,0xFF,0x30,0x84,0xFF,0xAE,0x73,0xFF,0x55,0xAD,0xFF,0x55,0xAD,0xFF,0xBA,0xD6,0x2D,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xEF,0x7B,0x00,0xEB,0x5A,0x00,0xFF,0xFF,0x00,0x49,0x4A,0x35,0xC7,0x39,0x7A,0xCF,0x7B,0xCD,0xB6,0xB5,0xFF,0x0C,0x63,0xFF,0xD7,0xBD,0xFF,0x08,0x42,0xFF,0x2C,0x63,0xD1,0x08,0x42,0xCB,0x08,0x42,0xE9,0x08,0x42,0xFA,
    0x28,0x42,0xFF,0x69,0x4A,0xFF,0x8A,0x52,0xFF,0xEB,0x5A,0xFF,0x2C,0x63,0xFF,0xEB,0x5A,0xFF,0x0C,0x63,0xFF,0x6D,0x6B,0xFF,0x4D,0x6B,0xFF,0xCF,0x7B,0xFF,0x8E,0x73,0xFC,0xCF,0x7B,0xF8,0xCF,0x7B,0xF8,0x86,0x31,0xEE,0x14,0xA5,0xFF,0x8E,0x73,0xFF,0x71,0x8C,0xFF,0x55,0xAD,0xFF,0x6D,0x6B,0xFF,0x71,0x8C,0xFF,0x2C,0x63,0xFF,0x34,0xA5,0x75,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xEF,0x7B,0x00,0xEB,0x5A,0x00,0xFF,0xFF,0x00,0x8A,0x52,0x00,0x08,0x42,0x00,0x30,0x84,0x00,0xAA,0x52,0x34,0xB2,0x94,0x54,0xEB,0x5A,0x20,0xCB,0x5A,0x00,0x4D,0x6B,0x00,0x8A,0x52,0x00,0x8A,0x52,0x00,0x8A,0x52,0x00,0xAA,0x52,0x00,0xCB,0x5A,0x00,0xEB,0x5A,0x00,0x4D,0x6B,0x00,0x6D,0x6B,0x00,0x4D,0x6B,0x00,0x6D,0x6B,0x00,0xAE,0x73,0x00,0x8E,0x73,0x00,0xEF,0x7B,0x00,0xCF,0x7B,0x00,0xEF,0x7B,0x00,0x30,0x84,0x00,0x28,0x42,0x00,0x8A,0x52,0x00,0x51,0x8C,0x4E,0xCF,0x7B,0x59,0xAE,0x73,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x71,0x8C,0x00,0x75,0xAD,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,
    0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,
    0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,
    0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF,0x00,};
const lv_img_dsc_t ui_img_images_range_knob_png = {
   .header.always_zero = 0,
   .header.w = 40,
   .header.h = 20,
   .data_size = sizeof(ui_img_images_range_knob_png_data),
   .header.cf = LV_IMG_CF_TRUE_COLOR_ALPHA,
   .data = ui_img_images_range_knob_png_data};
