// assets.c
#include "assets.h"

static const uint32_t COLOR_PALETTE[16] = {
    0x00000000,  // 0: 透明
    0xFFD2B48C,  // 1: 皮肤
    0xFF4169E1,  // 2: 蓝色
    0xFF8B4513,  // 3: 棕色
    0xFFFFD700,  // 4: 金色
    0xFFDC143C,  // 5: 红色
    0xFF00FF7F,  // 6: 绿色
    0xFFFFFFFF,  // 7: 白色
    0xFF000000,  // 8: 黑色
    0xFF808080,  // 9: 灰色
    0xFFFFA500,  // 10: 橙色
    0xFF9370DB,  // 11: 紫色
    0xFF00CED1,  // 12: 青色
    0xFFFF69B4,  // 13: 粉色
    0xFFA0522D,  // 14: 棕色
    0xFFF0E68C,  // 15: 卡其色
};

uint32_t get_color(int idx) {
    if (idx < 0 || idx >= 16) return COLOR_PALETTE[0];
    return COLOR_PALETTE[idx];
}

char get_ascii_char(int color_idx, int body_color) {
    if (color_idx == 0) return ' ';
    if (color_idx == 1) return '.';
    
    const char body_chars[] = {'.', '#', '@', '%'};
    if (color_idx >= 2 && color_idx <= 5) {
        return body_chars[body_color % 4];
    }
    
    const char special_chars[] = {'^', 'o', '#', '#'};
    if (color_idx >= 7 && color_idx <= 10) {
        return special_chars[color_idx - 7];
    }
    return '?';
}